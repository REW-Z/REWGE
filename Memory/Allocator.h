#pragma once

#include "REWGE.h"

class Allocator;
class SimpleAllocator;
class Block;




enum class AllocateLabel
{
	Default,
};





#pragma region 内存管理器基类

/// <summary>
/// 内存管理器基类
/// </summary>
class Allocator
{
public:
	/// <summary>
	/// 默认堆分配器
	/// </summary>
	static Allocator* GetAllocator(AllocateLabel label = AllocateLabel::Default);

	virtual void* Allocate(uint32 size, uint32 alignment = 0, bool isArray = false) = 0;

	virtual void Deallocate(std::byte* addr, uint32 alignment = 0, bool isArray = false) = 0;
};


#pragma endregion



#pragma region 简单堆分配器

/// <summary>
/// 简单内存块
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
/// 简单内存管理器基类
/// （alignment未实现）
/// </summary>
class SimpleAllocator : public Allocator
{
private:
	uint32 allocateCount = 0;
	uint32 deallocateCount = 0;
public:

	// 头尾Block指针
	Block* headBlock;
	Block* tailBlock;

	// 上次插入的Block
	static Block* lastInsertBlock;

	enum
	{
		MARK_BEG = 0xAAAAAAAA,
		MARK_END = 0xAAAAAAAA,
	};

	//析构函数
	~SimpleAllocator();

	void Analysis();

	//堆分配
	void* Allocate(uint32 size, uint32 alignment = 0, bool isArray = false);
	//释放
	void Deallocate(std::byte* addr, uint32 alignment = 0, bool isArray = false);


	void Insert(Block* blockPointer);

	void Remove(Block* blockPointer);

	static void WriteInfo(const char* cstr);

	uint32 GetBlockCount();
};
#pragma endregion






//（不建议）全局重载new操作符
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