#pragma once

#include "REWGE.h"
//#include <process.h>
#include <fstream>
#include <string> 
#include <assert.h>
#include "Allocator.h"


/// <summary>
/// 栈分配器
/// （依赖堆分配器）
/// </summary>
class StackAllocator : Allocator
{
public :
	/// <summary>
	/// 栈的最小Size
	/// </summary>
	uint32 minStackSize = 4 * 1024;

	/// <summary>
	/// 分配栈
	/// </summary>
	struct Stack
	{
		byte* stackBottom;
		uint32 dataSize;
		Stack* prevStack;
	};

	byte* stackTop = NULL;//栈顶
	byte* stackEnd = NULL;//栈结束
	Stack* currentStack = NULL;//当前分配栈

	/// <summary>
	/// 单例
	/// </summary>
	/// <returns></returns>
	static StackAllocator* GetInstance()
	{
		static StackAllocator intance;
		return &intance;
	}

	/// <summary>
	/// 在链表中创建新Stack
	/// </summary>
	/// <param name="datasize"></param>
	void NewStack(uint32 datasize)
	{
		//最小容量
		uint32 minDataSize = minStackSize - sizeof(Stack);
		//最后需要分配的容量
		uint32 newDataSize = datasize > minDataSize ? datasize : minStackSize;

		//创建新的栈
		Stack* newstack = (Stack*) (Allocator::GetAllocator()->Allocate(newDataSize + sizeof(Stack), 0) );
		newstack->dataSize = newDataSize;
		newstack->stackBottom = (byte*)newstack + sizeof(Stack);
		newstack->prevStack = currentStack;
		currentStack = newstack;
		stackTop = newstack->stackBottom;
		stackEnd = stackTop + newstack->dataSize;
	}

	/// <summary>
	/// 删除newTop后面的所有Stack(参数为NULL时全部删除)
	/// </summary>
	/// <param name="newTop">新的topStack</param>
	void DelStacks(Stack* newTop)
	{
		while (currentStack && currentStack != newTop)
		{
			byte* addrToFree = (byte*)currentStack;
			currentStack = currentStack->prevStack;
			Allocator::GetAllocator()->Deallocate(addrToFree, 0);
		}

		if (currentStack)
		{
			stackTop = currentStack->stackBottom;
			stackEnd = stackTop + currentStack->dataSize;
		}
		else
		{
			stackTop = NULL;
			stackEnd = NULL;
		}
	}


	void* Allocate(uint32 size, uint32 alignment = 0, bool isArray = false)
	{
		assert(size >= 0);
		assert(stackTop <= stackEnd);

		byte* result = stackTop;

		//当前Stack空间判断
		bool needNewStack;
		if (currentStack)
		{
			if (alignment > 0)
			{
				uint32 mask = (alignment - 1);
				uint32 misAlignment = ((uint32)stackTop & mask);
				uint32 adjustment = alignment - misAlignment;

				result = (byte*)stackTop + adjustment;
			}
			stackTop = result + size;
			needNewStack = stackTop > stackEnd;
		}
		else
		{
			needNewStack = true;
		}
		


		//需要分配新的Stack
		if (needNewStack)
		{
			//多余的空间用于对齐
			NewStack(size + alignment);

			result = stackTop;
			if (alignment > 0)
			{
				//对齐偏移
				uint32 mask = (alignment - 1);
				uint32 misAlignment = ((uint32)stackTop & mask);
				uint32 adjustment = alignment - misAlignment;

				result = (byte*)stackTop + adjustment;
			}
			stackTop = result + size;
		}

		return result;
	}

	void Deallocate(byte* addr, uint32 alignment = 0, bool isArray = false) {}
};

template<class T>
class StackAllocation
{
private :
	StackAllocator::Stack* freeMarkOldStack;
	byte* freeMarkOldTop;
	T* data;
public:

	T* Data()
	{
		return data;
	}

	StackAllocation(uint32 datasize , uint32 alignment = 0)
	{
		assert(datasize > 0);

		//栈内存释放标记
		this->freeMarkOldTop = StackAllocator::GetInstance()->stackTop;
		this->freeMarkOldStack = StackAllocator::GetInstance()->currentStack;

		//分配内存空间
		this->data = (T*)StackAllocator::GetInstance()->Allocate(datasize, alignment);
	}

	~StackAllocation()
	{
		//释放栈空间
		if (freeMarkOldStack != StackAllocator::GetInstance()->currentStack)
			StackAllocator::GetInstance()->DelStacks(freeMarkOldStack);

		StackAllocator::GetInstance()->stackTop = this->freeMarkOldTop;
		this->freeMarkOldTop = NULL;
	}

};

