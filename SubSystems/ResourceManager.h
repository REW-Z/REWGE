#pragma once


// ---------------------------------------------------------------------------------------

///CRE��������Ϸ��������ƣ�����н�ѹ��Ŀ¼��ʼ��Ϸʱ��Ŀ¼��ȡ�ļ���  �����ȡ�Ѵ�����ļ���  
///



class ResourceManager
{
private :
public:
	static std::unordered_map<std::string, Object*> loadedResources;

	static Object* Load(TypeEnum type, std::string path);

	static bool Unload(TypeEnum type, std::string path);

#pragma region Legacy Load Method

	//template<class T> static  T* Load(std::string path)
	//{

	//	T* asset = nullptr;
	//	bool load_successful = false;

	//	//Debug::Log("����������Դ- ��Դ·����");
	//	//Debug::Log(path);

	//	//Try Find In Loaded Assets
	//	std::unordered_map<std::string, Object*>::const_iterator it = loadedResources.find(path);
	//	if (it != loadedResources.end())
	//	{
	//		//Debug::Log("�Ѿ��ҵ���������Դ- ��Դ����");
	//		//Debug::WriteLogFile();
	//		//Debug::Log(path);
	//		return reinterpret_cast<T*>( it->second );
	//	}
	//	else
	//	{
	//		//Debug::Log("δ�ҵ���������Դ��������");
	//		//Debug::WriteLogFile();
	//	}


	//	// IS TEXTRUE2D
	//	if (typeid(T) == typeid(Texture2D))
	//	{
	//		//Debug::Log("����������Դ");
	//		//Debug::WriteLogFile();

	//		//�½���Դ����
	//		Texture2D* tex_asset = new Texture2D();
	//		//tex_asset->stringId = StringId();
	//		//tex_asset->stringId.string = path;
	//		GLuint textureID;


	//		//��ȡ�ļ����ڴ滺����
	//		FilePtr* filePtr = FileSystem::LoadFile(path, FileSystem::IOSLIB::Fstream);

	//		//�ӻ���������ת��ΪOpenGL����( �ú�����glInit()֮ǰ���û�������� �� )
	//		try
	//		{
	//			//һ��ֱ�Ӵ��ļ���������ȡΪ���
	//			textureID = SOIL_load_OGL_texture_from_memory(reinterpret_cast<const unsigned char*>(filePtr->buffer), filePtr->length, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	//			////�������ļ���������ȡ Ȼ��ת��ΪGL���
	//			//int chanels;
	//			//int width;
	//			//int height;
	//			//unsigned char * ptr = SOIL_load_image_from_memory(reinterpret_cast<const unsigned char*>(filePtr->buffer), filePtr->length, &width, &height, &chanels, SOIL_LOAD_AUTO);
	//			//textureID = SOIL_create_OGL_texture(ptr, &width, &height, chanels, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	//			//SOIL_free_image_data(ptr);

	//		}
	//		catch(std::exception exc)
	//		{
	//			//Debug::Log("SOILת������1");
	//			Debug::WriteLogFile();
	//		}

	//		if (textureID == 0)
	//		{
	//			//Debug::Log("SOILת������2");
	//		}
	//		tex_asset->handle = textureID;


	//		//�ͷ��ļ�������
	//		Allocator::GetInstance()->Deallocate((byte*)(filePtr->buffer), 0);
	//		


	//		//��ʼ������
	//		tex_asset->Init();


	//		//SUCCESS
	//		asset = reinterpret_cast<T*>(tex_asset);
	//		load_successful = true;


	//		//Debug::Log("����ɹ�");
	//		//Debug::WriteLogFile();
	//	}
	//	else
	//	{
	//	}



	//	//��ӵ���Դ��
	//	if (load_successful)
	//	{
	//		auto item = std::pair<std::string, Object*>(path, dynamic_cast<Object*>( asset ) );
	//		ResourceManager::loadedResources.insert(item);
	//	}

	//	return asset;
	//}

#pragma endregion

};






// ---------------------------------------------------------------------------------------

//CRE:U3D�����Importer����ÿ����Դ���Ͷ�Ӧһ��Importer���ࡣ    

class ResourceImporter
{
public:
	enum ImageFileType
	{
		PNG = 0,
		JPG = 1,
	};

	static MaterialShader* ImportGLSLShader(std::string path);

	static Texture2D * ImportTexture2D(std::string path);

	static GameObject* ImportObjModel(std::string filePath);

	static IndexlessMesh* ImportObjIndexlessMesh(std::string filePath);
};





// ---------------------------------------------------------------------------------------

class MeshImporter
{
public:
	//TEMP INFOS
	std::vector<float> rawVertVals;
	std::vector<float> rawTexcoordsVals;
	std::vector<float> rawNormVals;

	std::vector<float> triangleVerts;
	std::vector<float> textureCoords;
	std::vector<float> normals;
	std::vector<float> tangents;

	std::vector<int> inds;
	std::vector<SubMesh> meshParts;

	//Parsed Object
	std::vector<std::string> _obj_names;
	std::vector<Mesh*> _meshes;
	std::vector<Material*> _materials;
	std::unordered_map<int, Material*> _submesh_mat_map;


public:
	MeshImporter();
	void parseOBJ(const char* filePath);
	void parseMTL(const char* filePath);
};





// -------------------------class define-----------------------

class IndexlessModelImporter
{
private:
	std::vector<float> vertVals;
	std::vector<float> stVals;
	std::vector<float> normVals;

	std::vector<float> triangleVerts;
	std::vector<float> textureCoords;
	std::vector<float> normals;
public:
	IndexlessModelImporter();
	void parseOBJ(const char* filePath);
	int getNumVertices();
	std::vector<float> getVertices();
	std::vector<float> getTextureCoordinates();
	std::vector<float> getNormals();
};
