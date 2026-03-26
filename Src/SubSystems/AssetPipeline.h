#pragma once


class AssetCollection
{

};


// ---------------------------------------------------------------------------------------

//CRE:U3D引擎的Importer类是每种资源类型对应一个Importer子类。    

class AssetPipeline
{
public:
	enum ImageFileType
	{
		PNG = 0,
		JPG = 1,
	};
public:
	static MaterialShader* Import_GLSL(std::string path);

	static Texture2D* Import_IMG(std::string path);

	static GameObject* Import_OBJ(std::string filePath);

	static IndexlessMesh* Import_AsIndexlessMesh_OBJ(std::string filePath);
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
