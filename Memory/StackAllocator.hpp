#pragma once

#include "REWGE.h"
//#include <process.h>
#include <fstream>
#include <string> 
#include <assert.h>
#include "Allocator.h"


/// <summary>
/// ջ������
/// �������ѷ�������
/// </summary>
class StackAllocator : Allocator
{
public :
	/// <summary>
	/// ջ����СSize
	/// </summary>
	uint32 minStackSize = 4 * 1024;

	/// <summary>
	/// ����ջ
	/// </summary>
	struct Stack
	{
		byte* stackBottom;
		uint32 dataSize;
		Stack* prevStack;
	};

	byte* stackTop = NULL;//ջ��
	byte* stackEnd = NULL;//ջ����
	Stack* currentStack = NULL;//��ǰ����ջ

	/// <summary>
	/// ����
	/// </summary>
	/// <returns></returns>
	static StackAllocator* GetInstance()
	{
		static StackAllocator intance;
		return &intance;
	}

	/// <summary>
	/// �������д�����Stack
	/// </summary>
	/// <param name="datasize"></param>
	void NewStack(uint32 datasize)
	{
		//��С����
		uint32 minDataSize = minStackSize - sizeof(Stack);
		//�����Ҫ���������
		uint32 newDataSize = datasize > minDataSize ? datasize : minStackSize;

		//�����µ�ջ
		Stack* newstack = (Stack*) (Allocator::GetAllocator()->Allocate(newDataSize + sizeof(Stack), 0) );
		newstack->dataSize = newDataSize;
		newstack->stackBottom = (byte*)newstack + sizeof(Stack);
		newstack->prevStack = currentStack;
		currentStack = newstack;
		stackTop = newstack->stackBottom;
		stackEnd = stackTop + newstack->dataSize;
	}

	/// <summary>
	/// ɾ��newTop���������Stack(����ΪNULLʱȫ��ɾ��)
	/// </summary>
	/// <param name="newTop">�µ�topStack</param>
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

		//��ǰStack�ռ��ж�
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
		


		//��Ҫ�����µ�Stack
		if (needNewStack)
		{
			//����Ŀռ����ڶ���
			NewStack(size + alignment);

			result = stackTop;
			if (alignment > 0)
			{
				//����ƫ��
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

		//ջ�ڴ��ͷű��
		this->freeMarkOldTop = StackAllocator::GetInstance()->stackTop;
		this->freeMarkOldStack = StackAllocator::GetInstance()->currentStack;

		//�����ڴ�ռ�
		this->data = (T*)StackAllocator::GetInstance()->Allocate(datasize, alignment);
	}

	~StackAllocation()
	{
		//�ͷ�ջ�ռ�
		if (freeMarkOldStack != StackAllocator::GetInstance()->currentStack)
			StackAllocator::GetInstance()->DelStacks(freeMarkOldStack);

		StackAllocator::GetInstance()->stackTop = this->freeMarkOldTop;
		this->freeMarkOldTop = NULL;
	}

};

