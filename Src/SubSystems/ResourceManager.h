#pragma once


// ---------------------------------------------------------------------------------------

///CRE：部分游戏的载入机制：如果有解压的目录则开始游戏时从目录读取文件。  否则读取已打包的文件。  
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

	//	//Debug::Log("尝试载入资源- 资源路径：");
	//	//Debug::Log(path);

	//	//Try Find In Loaded Assets
	//	std::unordered_map<std::string, Object*>::const_iterator it = loadedResources.find(path);
	//	if (it != loadedResources.end())
	//	{
	//		//Debug::Log("已经找到已载入资源- 资源名：");
	//		//Debug::WriteLogFile();
	//		//Debug::Log(path);
	//		return reinterpret_cast<T*>( it->second );
	//	}
	//	else
	//	{
	//		//Debug::Log("未找到已载入资源，载入中");
	//		//Debug::WriteLogFile();
	//	}


	//	// IS TEXTRUE2D
	//	if (typeid(T) == typeid(Texture2D))
	//	{
	//		//Debug::Log("尝试载入资源");
	//		//Debug::WriteLogFile();

	//		//新建资源对象
	//		Texture2D* tex_asset = new Texture2D();
	//		//tex_asset->stringId = StringId();
	//		//tex_asset->stringId.string = path;
	//		GLuint textureID;


	//		//读取文件到内存缓冲区
	//		FilePtr* filePtr = FileSystem::LoadFile(path, FileSystem::IOSLIB::Fstream);

	//		//从缓冲区数据转换为OpenGL纹理( 该函数在glInit()之前调用会崩溃闪退 ！ )
	//		try
	//		{
	//			//一、直接从文件缓冲区读取为句柄
	//			textureID = SOIL_load_OGL_texture_from_memory(reinterpret_cast<const unsigned char*>(filePtr->buffer), filePtr->length, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	//			////二、从文件缓冲区读取 然后转换为GL句柄
	//			//int chanels;
	//			//int width;
	//			//int height;
	//			//unsigned char * ptr = SOIL_load_image_from_memory(reinterpret_cast<const unsigned char*>(filePtr->buffer), filePtr->length, &width, &height, &chanels, SOIL_LOAD_AUTO);
	//			//textureID = SOIL_create_OGL_texture(ptr, &width, &height, chanels, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	//			//SOIL_free_image_data(ptr);

	//		}
	//		catch(std::exception exc)
	//		{
	//			//Debug::Log("SOIL转换错误1");
	//			Debug::WriteLogFile();
	//		}

	//		if (textureID == 0)
	//		{
	//			//Debug::Log("SOIL转换错误2");
	//		}
	//		tex_asset->handle = textureID;


	//		//释放文件缓冲区
	//		Allocator::GetInstance()->Deallocate((byte*)(filePtr->buffer), 0);
	//		


	//		//初始化设置
	//		tex_asset->Init();


	//		//SUCCESS
	//		asset = reinterpret_cast<T*>(tex_asset);
	//		load_successful = true;


	//		//Debug::Log("载入成功");
	//		//Debug::WriteLogFile();
	//	}
	//	else
	//	{
	//	}



	//	//添加到资源表
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

//CRE:U3D引擎的Importer类是每种资源类型对应一个Importer子类。    

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
