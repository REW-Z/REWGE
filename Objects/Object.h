#pragma once



class ResourceManager;
//------------------------------------
class Type;
class Activator;
//------------------------------------


enum TypeEnum
{
	//未定义
	TYPE_UNDEFINED = -1,

	//基本类型
	TYPE_Object = 0,
	TYPE_GameObject = 1,
	TYPE_World = 2,

	//组件类型  
	TYPE_Component = 1000,
	TYPE_Transform = 1001,
	TYPE_Behaviour = 1002,
	TYPE_MonoScript = 1003,
	TYPE_Camera = 1100,
	TYPE_Light = 1101,
	TYPE_Renderer = 1200,
	TYPE_MeshRenderer = 1201,

	TYPE_Mesh = 2000,
	TYPE_Texture = 2001,
	TYPE_Texture2D = 2002,
	TYPE_RenderTexture = 2003,
	TYPE_MaterialShader = 2010,
	TYPE_Material = 2011,
};

//------------------------------------

#define OBJECT_T2TYPE(XXX)\
	class XXX;\
	template<> struct T2TYPE<XXX*>{ enum{ value = TypeEnum::TYPE_##XXX }; };

//------------------------------------

template<typename T> struct T2TYPE { enum { value = TypeEnum::TYPE_UNDEFINED }; };//默认情况
OBJECT_T2TYPE(Object)
OBJECT_T2TYPE(GameObject)
OBJECT_T2TYPE(World)

OBJECT_T2TYPE(Component)
OBJECT_T2TYPE(Transform)
OBJECT_T2TYPE(Behaviour)
OBJECT_T2TYPE(MonoScript)
OBJECT_T2TYPE(Camera)
OBJECT_T2TYPE(Light)
OBJECT_T2TYPE(Renderer)
OBJECT_T2TYPE(MeshRenderer)

OBJECT_T2TYPE(Mesh)
OBJECT_T2TYPE(Texture)
OBJECT_T2TYPE(Texture2D)
OBJECT_T2TYPE(RenderTexture)
OBJECT_T2TYPE(MaterialShader)
OBJECT_T2TYPE(Material)


//------------------------------------


//类型声明
#define OBJECT_TYPE_DECLARE(XXX) \
public:\
	static Type type;\
	virtual Type* GetType();\
	static Object* Construct();\
	virtual int GetTypeEnum(){ return TypeEnum::TYPE_##XXX; }


//类型实现							(！！需要内存分配器头文件！！)
#define OBJECT_TYPE_IMPLEMENT(XXX, YYY)\
Type XXX::type = Type(#XXX, TypeEnum::TYPE_##XXX, &YYY::type, XXX::Construct);\
Type* XXX::GetType(){  return &XXX::type;  }\
Object* XXX::Construct(){  return NEW(XXX) XXX;  }

//类到枚举
#define TYPEENUM(XXX) TypeEnum::TYPE_##XXX

//类型获取
#define typeof(XXX) (*Object::GetTypeMap())[#XXX]

//类型判断
#define is(XXX) ->GetType()->Is(TYPEENUM(XXX))

//类型转换
#define as(XXX) ->Cast<XXX>()











//------------------------------------
// StringID
//------------------------------------

class StringId
{
private:
	mutable std::string path;  //std::string默认值为""
	mutable size_t pathHash;
public:

	static size_t CalHash(std::string input)
	{
		std::hash<std::string> h;
		return h(input);
	}

	StringId()
	{
		pathHash = 0;
	}
	StringId(std::string newstr)
	{
		this->path = newstr;
		this->pathHash = CalHash(newstr);
	}



	size_t gethash()
	{
		return this->pathHash;
	}

	std::string getpath()
	{
		return this->path;
	}
};

//--------------------
//  Pointer
// 
// （CRE：引擎内部对象互相引用使用立即指针，而对于外部对象依赖可以使用这个ObjectPtr对象）    
//--------------------

template<class T>
class OPtr
{
public:
	StringId instanceId;
	T* obj;


	OPtr() 
	{
		this->instanceId = StringId();
		obj = NULL;
	}
	OPtr(StringId newid) 
	{ 
		instanceId = newid;
		obj = NULL;
	}
	OPtr(const T* newobj)
	{
		SetObjectPointer(newobj); 
	}
	OPtr(const OPtr<T>& ptr) 
	{   
		this->instanceId = ptr.instanceId; 
		this->obj = ptr.obj;
	}
	OPtr& operator = (const T* newobj)
	{ 
		SetObjectPointer(newobj);
		return *this; 
	}
	OPtr& operator = (const OPtr<T>& ptr)
	{  
		this->instanceId = ptr.instanceId;
		this->obj = ptr.obj;
		return *this;
	}

	//"*"解引用重载
	T& operator*() 
	{ 
		return (*(GetObjectPointer())); 
	}
	//"->"操作符重载
	T* operator ->()
	{ 
		return GetObjectPointer(); 
	}
	//T*隐式转换重载
	operator T* () 
	{ 
		return GetObjectPointer();
	} 

private:
	void SetObjectPointer(Object* newobj) 
	{
		if (newobj)
		{
			this->instanceId = newobj.instID;
			this->obj = newobj;
		}
		else
		{
			this->instanceId = StringId();
			this->obj = NULL;
		}
	}
	T* GetObjectPointer() 
	{
		if (instanceId.gethash() == 0)
			return NULL;

		if (obj)
			return obj;

		if(obj == NULL)
			obj = dynamic_cast<T*>( Object::FindObject(this->instanceId.gethash()) );
		
		if(obj == NULL)
			obj = dynamic_cast<T*> (ResourceManager::Load((TypeEnum)(T2TYPE<T*>().value), this->instanceId.getpath()) );

		return dynamic_cast<T*>( obj );
	}

};

//typedef OPtr<Object> Object※;

//--------------------
//  Type
//--------------------

typedef Object* (*ConstructorFunction)(void);

class Type
{
public:
	//...static
public:
	//类型名
	const char* name;

	//类型枚举
	TypeEnum typeEnum;

	//基类型(当类型等于本类型时认为是无基类)  
	Type* baseType;  

	//类型构造函数
	ConstructorFunction constructor;
public:

	Type(const char* name, TypeEnum typeEnum, Type* base, ConstructorFunction ctor);

	bool Is(Type* type);
	bool Is(TypeEnum type);

};

//--------------------
//  Activator
//--------------------

class Activator
{

	///CRE：
	///如果不用GetTypeMap()获取哈希表
	///而是定义静态哈希表
	///则会出现静态初始化顺序问题，因为Object的type字段静态初始化调用了Type()，Type()访问了类型哈希表。    

};


//--------------------
// Object
//--------------------





class Object
{
	OBJECT_TYPE_DECLARE(Object)
public:
	//显示名称
	std::string name;
	//全局唯一实例名称（一般使用路径名）
	StringId instID;
private:
	int refNum;
public:
	//void RefAd();
	//void RefRm();
public:
	Object();

	static std::unordered_map<int, Type*>* GetTypeMap();
	static std::unordered_map<size_t, Object*>* GetIDToObjectMap();
	static Object* FindObject(size_t hash);


	static Object* CreateInstance(TypeEnum type, std::string stringid = "");
	static bool DeleteInstance(Object* obj);

	static void InsertToObjectMap(size_t guid, Object* obj);
public:
	bool Is(TypeEnum type);

	template<typename T>
	T* Cast() { return dynamic_cast<T*>(this); }


	void Foo() {}
};
