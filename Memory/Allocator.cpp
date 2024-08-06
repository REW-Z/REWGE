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
/// ����ģʽ
/// </summary>
/// <returns></returns>
Allocator* Allocator:: GetAllocator(AllocateLabel label)
{
	static SimpleAllocator instance;
	return &instance;
}







Block* SimpleAllocator::lastInsertBlock = NULL;

/// <summary>
/// ����������
/// </summary>
SimpleAllocator::~SimpleAllocator()
{
	Debug::Log("�����ڴ�й¶...");
	
	Debug::Log(string("�ܷ��������") + to_string(this->allocateCount));
	Debug::Log(string("���ͷŴ�����") + to_string(this->deallocateCount));

	this->Analysis();
}

void SimpleAllocator::Analysis()
{
	//��ǰ�ڴ�������
	Debug::Log("�����ڴ�������...");
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
/// �ڴ����
/// </summary>
void* SimpleAllocator::Allocate( uint32 size, uint32 alignment, bool isArray)
{
	//   | (Block) | BEG(uint) |  adjustment with ajustmentMark  |  allocSize    | END(uint) |


	//������ܿռ� (�����������������ֽڶ���)  ��CRE������˵alignment��Ϊ(alignment-1)Ҳ���� ��
	uint32 totalSize = sizeof(Block) + sizeof(uint32) + (alignment + sizeof(uint32)  + size) + sizeof(uint32);

	//(�����ڴ�ռ�)
	byte* addr = (byte*)malloc(totalSize);//addr ----->  �ڴ����ʼ

	assert(addr);

	//Block��Ϣ 
	Block* blockPtr = (Block*)addr;
	blockPtr->dataSize = size;
	blockPtr->alignment = alignment;
	blockPtr->isArray = isArray;
	
	for (int i = 0; i < sizeof(blockPtr->info); i++)
		blockPtr->info[i] = '\0';

	//Block����
	Insert(blockPtr);
	addr += sizeof(Block);//addr ----->  Block��

	//���ݶε�ǰ����
	uint32* beginMark = (uint32*)(addr);
	*beginMark = SimpleAllocator::MARK_BEG;
	addr += sizeof(uint32);//addr ----->  ʵ�ʷ����ڴ����ʼ��rawAddr��

	uint32* endMark= (uint32*)(addr + size + alignment + sizeof(uint32));
	*endMark = SimpleAllocator::MARK_END;

	//��ʱ��ַ���ڼ���ƫ�� ��Ԥ��ƫ���ֽ����Ĵ��λ�ã�
	byte* tmpAddr = addr + sizeof(uint32);

	//�������ĵ�ַ
	uint32 byteAdjustment = 0;//0�ֽڶ���ʱ��ƫ��Ϊ0
	if (alignment > 0)
	{
		//***�������***
		uint32 mask = (alignment - 1);
		uint32 misAlignment = ((uint32)tmpAddr & mask);
		unsigned  adjustment = alignment - misAlignment;
		//**************

		byteAdjustment = adjustment;
	}

	byte* alignedAddr = tmpAddr + byteAdjustment;

	//�Ѷ����ַ��ǰ��4�ֽ�--���ڴ��ƫ���ֽ���
	uint32* adjustmentMark = (uint32*)(alignedAddr - 4);
	*adjustmentMark = byteAdjustment + 4;


	//�����¼
	this->allocateCount++;

	assert(alignedAddr != 0);

	return (void*)alignedAddr;
}

void SimpleAllocator::Deallocate(byte* addr, uint32 alignment, bool isArray)
{
	if (!addr) return;


	//ȡ��ƫ��ֵ (�����ǰ��4�ֽ�)
	uint32* adjustmentMark = (uint32*)(addr - 4);
	uint32 totalAdjustment = *adjustmentMark;

	//raw Addrλ��
	addr -= totalAdjustment;		//addr ---------> raw Addrλ��

	//�ж�
	addr -= sizeof(uint32);  //addr ---------> BEGλ��
	uint32* beginMark = (uint32*)(addr);
	assert(*beginMark == SimpleAllocator::MARK_BEG);

	addr -= sizeof(Block);		//addr ---------> Blockλ��
	Block* blockPtr = (Block*)addr;
	assert(blockPtr->alignment == alignment);
	assert(blockPtr->isArray == isArray);

	uint32* endMark = (uint32*)(addr + sizeof(Block) + sizeof(uint32) + alignment + sizeof(uint32) + blockPtr->dataSize);
	assert(*endMark == SimpleAllocator::MARK_END);

	//ɾ���ڵ�
	Remove(blockPtr);

	//�ͷſռ�
	free(addr);  


	//�ͷŴ�����¼
	this->deallocateCount++;
}



/// <summary>
/// д���ڴ��ע�ͣ���������ڴ�飩
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
/// �����ڴ��ڵ�
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
/// �Ƴ��ڴ��ڵ�
/// </summary>
void SimpleAllocator::Remove(Block* blockPtr)
{
	//��ǰ�ýڵ�
	if (blockPtr->prevBlock)
	{
		blockPtr->prevBlock->nextBlock = blockPtr->nextBlock;
	}
	else
	{
		this->headBlock = blockPtr->nextBlock;
	}

	//�к��ýڵ�
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





