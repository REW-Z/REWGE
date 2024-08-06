#pragma once

#include "REWGE.h"

class Allocator;
class SimpleAllocator;
class Block;




enum class AllocateLabel
{
	Default,
};





#pragma region �ڴ����������

/// <summary>
/// �ڴ����������
/// </summary>
class Allocator
{
public:
	/// <summary>
	/// Ĭ�϶ѷ�����
	/// </summary>
	static Allocator* GetAllocator(AllocateLabel label = AllocateLabel::Default);

	virtual void* Allocate(uint32 size, uint32 alignment = 0, bool isArray = false) = 0;

	virtual void Deallocate(std::byte* addr, uint32 alignment = 0, bool isArray = false) = 0;
};


#pragma endregion



#pragma region �򵥶ѷ�����

/// <summary>
/// ���ڴ��
/// </summary>
class Block
{
public:
	Block* prevBlock;
	Block* nextBlock;

	uint32 dataSize;
	uint32 alignment;
	bool isArray;

	char info[16];
};

/// <summary>
/// ���ڴ����������
/// ��alignmentδʵ�֣�
/// </summary>
class SimpleAllocator : public Allocator
{
private:
	uint32 allocateCount = 0;
	uint32 deallocateCount = 0;
public:

	// ͷβBlockָ��
	Block* headBlock;
	Block* tailBlock;

	// �ϴβ����Block
	static Block* lastInsertBlock;

	enum
	{
		MARK_BEG = 0xAAAAAAAA,
		MARK_END = 0xAAAAAAAA,
	};

	//��������
	~SimpleAllocator();

	void Analysis();

	//�ѷ���
	void* Allocate(uint32 size, uint32 alignment = 0, bool isArray = false);
	//�ͷ�
	void Deallocate(std::byte* addr, uint32 alignment = 0, bool isArray = false);


	void Insert(Block* blockPointer);

	void Remove(Block* blockPointer);

	static void WriteInfo(const char* cstr);

	uint32 GetBlockCount();
};
#pragma endregion






//�������飩ȫ������new������
// void* operator new(size_t size)
//{
//	return Allocator::GetInstance()->Allocate(size, 0);
//}
// void operator delete(void* ptr)
//{
//	Allocator::GetInstance()->Deallocate((byte*)ptr, 0);
//}
// void* operator new[](size_t size)
//{
//	return Allocator::GetInstance()->Allocate(size, 0, true);
//}
// void operator delete[](void* ptr)
//{
//	Allocator::GetInstance()->Deallocate((byte*)ptr, 0, true);
//}



#define MALLOC(size)		Allocator::GetAllocator()->Allocate(size)   
#define FREE(ptr)			if(dynamic_cast<Object*>(ptr)) Allocator::GetAllocator()->Deallocate((std::byte*)ptr);

#define NEW(T)				new (MALLOC(sizeof(T)))
#define DEL(ptr)			FREE(ptr)