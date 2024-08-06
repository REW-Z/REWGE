#pragma once



class ResourceManager;
//------------------------------------
class Type;
class Activator;
//------------------------------------


enum TypeEnum
{
	//δ����
	TYPE_UNDEFINED = -1,

	//��������
	TYPE_Object = 0,
	TYPE_GameObject = 1,
	TYPE_World = 2,

	//�������  
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

template<typename T> struct T2TYPE { enum { value = TypeEnum::TYPE_UNDEFINED }; };//Ĭ�����
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


//��������
#define OBJECT_TYPE_DECLARE(XXX) \
public:\
	static Type type;\
	virtual Type* GetType();\
	static Object* Construct();\
	virtual int GetTypeEnum(){ return TypeEnum::TYPE_##XXX; }


//����ʵ��							(������Ҫ�ڴ������ͷ�ļ�����)
#define OBJECT_TYPE_IMPLEMENT(XXX, YYY)\
Type XXX::type = Type(#XXX, TypeEnum::TYPE_##XXX, &YYY::type, XXX::Construct);\
Type* XXX::GetType(){  return &XXX::type;  }\
Object* XXX::Construct(){  return NEW(XXX) XXX;  }

//�ൽö��
#define TYPEENUM(XXX) TypeEnum::TYPE_##XXX

//���ͻ�ȡ
#define typeof(XXX) (*Object::GetTypeMap())[#XXX]

//�����ж�
#define is(XXX) ->GetType()->Is(TYPEENUM(XXX))

//����ת��
#define as(XXX) ->Cast<XXX>()











//------------------------------------
// StringID
//------------------------------------

class StringId
{
private:
	mutable std::string path;  //std::stringĬ��ֵΪ""
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
// ��CRE�������ڲ�����������ʹ������ָ�룬�������ⲿ������������ʹ�����ObjectPtr����    
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

	//"*"����������
	T& operator*() 
	{ 
		return (*(GetObjectPointer())); 
	}
	//"->"����������
	T* operator ->()
	{ 
		return GetObjectPointer(); 
	}
	//T*��ʽת������
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

//typedef OPtr<Object> Object��;

//--------------------
//  Type
//--------------------

typedef Object* (*ConstructorFunction)(void);

class Type
{
public:
	//...static
public:
	//������
	const char* name;

	//����ö��
	TypeEnum typeEnum;

	//������(�����͵��ڱ�����ʱ��Ϊ���޻���)  
	Type* baseType;  

	//���͹��캯��
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

	///CRE��
	///�������GetTypeMap()��ȡ��ϣ��
	///���Ƕ��徲̬��ϣ��
	///�����־�̬��ʼ��˳�����⣬��ΪObject��type�ֶξ�̬��ʼ��������Type()��Type()���������͹�ϣ��    

};


//--------------------
// Object
//--------------------





class Object
{
	OBJECT_TYPE_DECLARE(Object)
public:
	//��ʾ����
	std::string name;
	//ȫ��Ψһʵ�����ƣ�һ��ʹ��·������
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
