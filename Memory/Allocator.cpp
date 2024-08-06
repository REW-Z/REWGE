#pragma once

#include "REWGE.h"

//#include <process.h>
#include <fstream>
#include <string> 
#include <assert.h>
#include "SubSystems/Debug.h"
#include "Allocator.h"

using namespace std;


/// <summary>
/// 单例模式
/// </summary>
/// <returns></returns>
Allocator* Allocator:: GetAllocator(AllocateLabel label)
{
	static SimpleAllocator instance;
	return &instance;
}







Block* SimpleAllocator::lastInsertBlock = NULL;

/// <summary>
/// 分配器析构
/// </summary>
SimpleAllocator::~SimpleAllocator()
{
	Debug::Log("分析内存泄露...");
	
	Debug::Log(string("总分配次数：") + to_string(this->allocateCount));
	Debug::Log(string("总释放次数：") + to_string(this->deallocateCount));

	this->Analysis();
}

void SimpleAllocator::Analysis()
{
	//当前内存分配情况
	Debug::Log("分析内存分配情况...");
	Block* b = tailBlock;
	while(b)
	{
		string str = "\n[Block]";

		str += "\ninfo:";
		str += b->info;
		
		str += "\n size: ";
		str += to_string( b->dataSize);
		str += "\n";

		Debug::Log(str);

		b = b->prevBlock;
	}
}

/// <summary>
/// 内存分配
/// </summary>
void* SimpleAllocator::Allocate( uint32 size, uint32 alignment, bool isArray)
{
	//   | (Block) | BEG(uint) |  adjustment with ajustmentMark  |  allocSize    | END(uint) |


	//申请的总空间 (包含冗余的填充用于字节对齐)  （CRE：按理说alignment改为(alignment-1)也可以 ）
	uint32 totalSize = sizeof(Block) + sizeof(uint32) + (alignment + sizeof(uint32)  + size) + sizeof(uint32);

	//(分配内存空间)
	byte* addr = (byte*)malloc(totalSize);//addr ----->  内存块起始

	assert(addr);

	//Block信息 
	Block* blockPtr = (Block*)addr;
	blockPtr->dataSize = size;
	blockPtr->alignment = alignment;
	blockPtr->isArray = isArray;
	
	for (int i = 0; i < sizeof(blockPtr->info); i++)
		blockPtr->info[i] = '\0';

	//Block插入
	Insert(blockPtr);
	addr += sizeof(Block);//addr ----->  Block后

	//数据段的前后标记
	uint32* beginMark = (uint32*)(addr);
	*beginMark = SimpleAllocator::MARK_BEG;
	addr += sizeof(uint32);//addr ----->  实际分配内存的起始（rawAddr）

	uint32* endMark= (uint32*)(addr + size + alignment + sizeof(uint32));
	*endMark = SimpleAllocator::MARK_END;

	//临时地址用于计算偏移 （预留偏移字节数的存放位置）
	byte* tmpAddr = addr + sizeof(uint32);

	//计算对齐的地址
	uint32 byteAdjustment = 0;//0字节对齐时的偏移为0
	if (alignment > 0)
	{
		//***对齐计算***
		uint32 mask = (alignment - 1);
		uint32 misAlignment = ((uint32)tmpAddr & mask);
		unsigned  adjustment = alignment - misAlignment;
		//**************

		byteAdjustment = adjustment;
	}

	byte* alignedAddr = tmpAddr + byteAdjustment;

	//已对齐地址的前面4字节--用于存放偏移字节数
	uint32* adjustmentMark = (uint32*)(alignedAddr - 4);
	*adjustmentMark = byteAdjustment + 4;


	//分配记录
	this->allocateCount++;

	assert(alignedAddr != 0);

	return (void*)alignedAddr;
}

void SimpleAllocator::Deallocate(byte* addr, uint32 alignment, bool isArray)
{
	if (!addr) return;


	//取回偏移值 (存放在前面4字节)
	uint32* adjustmentMark = (uint32*)(addr - 4);
	uint32 totalAdjustment = *adjustmentMark;

	//raw Addr位置
	addr -= totalAdjustment;		//addr ---------> raw Addr位置

	//判断
	addr -= sizeof(uint32);  //addr ---------> BEG位置
	uint32* beginMark = (uint32*)(addr);
	assert(*beginMark == SimpleAllocator::MARK_BEG);

	addr -= sizeof(Block);		//addr ---------> Block位置
	Block* blockPtr = (Block*)addr;
	assert(blockPtr->alignment == alignment);
	assert(blockPtr->isArray == isArray);

	uint32* endMark = (uint32*)(addr + sizeof(Block) + sizeof(uint32) + alignment + sizeof(uint32) + blockPtr->dataSize);
	assert(*endMark == SimpleAllocator::MARK_END);

	//删除节点
	Remove(blockPtr);

	//释放空间
	free(addr);  


	//释放次数记录
	this->deallocateCount++;
}



/// <summary>
/// 写入内存块注释（最后分配的内存块）
/// </summary>
/// <param name="cstr"></param>
void SimpleAllocator::WriteInfo(const char* cstr)
{
	Block* lastInsert = SimpleAllocator::lastInsertBlock;
	if (lastInsert)
	{
		uint32 minLen = (strlen(cstr) + 1) < (sizeof(lastInsert->info) - 1) ? (strlen(cstr) + 1) : (sizeof(lastInsert->info) - 1);
		memcpy(lastInsert->info, cstr, minLen);
	}
}


/// <summary>
/// 插入内存块节点
/// </summary>
void SimpleAllocator::Insert(Block* blockPtr)
{
	//Any Blocks?
	if (tailBlock)
	{
		//Set As Last Node
		blockPtr->prevBlock = tailBlock;
		blockPtr->nextBlock = 0;
		this->tailBlock->nextBlock = blockPtr;
		this->tailBlock = blockPtr;

		//as last insert block
		SimpleAllocator::lastInsertBlock = blockPtr;
	}
	//No Blocks
	else
	{
		//New Node As Prev&Tail
		blockPtr->prevBlock = 0;
		blockPtr->nextBlock = 0;
		this->headBlock = blockPtr;
		this->tailBlock = blockPtr;

		//as last insert block
		SimpleAllocator::lastInsertBlock = blockPtr;
	}
}

/// <summary>
/// 移除内存块节点
/// </summary>
void SimpleAllocator::Remove(Block* blockPtr)
{
	//有前置节点
	if (blockPtr->prevBlock)
	{
		blockPtr->prevBlock->nextBlock = blockPtr->nextBlock;
	}
	else
	{
		this->headBlock = blockPtr->nextBlock;
	}

	//有后置节点
	if (blockPtr->nextBlock)
	{
		blockPtr->nextBlock->prevBlock = blockPtr->prevBlock;
	}
	else
	{
		this->tailBlock = blockPtr->prevBlock;
	}
}

uint32 SimpleAllocator::GetBlockCount()
{
	uint32 count = 0;
	Block* b = this->headBlock;
	while (b)
	{
		count++;
		b = b->nextBlock;
	}
	return count;
}





