#pragma once


//----------------------------------------------------------
//1、推荐使用自定义的模型文件类型，提高读取速度，而不是每次都导入obj文件。
//-----------------------class define-----------------------  

//----------------------------------------------------------
//Blender导出的Obj文件是右手坐标系CCW。和本引擎一致。    
//----------------------------------------------------------


//-----------------------class define-----------------------

class SubMesh
{
public:
	//Material material; //更新：材质和子网格分离
	int idx;
	std::vector<int> inds;
};

//-----------------------class define-----------------------

//带索引结构的Mesh(U3D风格)  

class Mesh: public Component
{
	OBJECT_TYPE_DECLARE(Mesh)

public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	
	//index(element)(ccw right-handed)
	std::vector<int> inds;
	//sub-meshes
	std::vector<SubMesh> subMeshes;

	int numVertices;
public:
	GLuint vbo[4];
	GLuint ebo;
	GLuint vao;

	bool binding_vbo;
public:
	Mesh();

	void SetupAPI();

public:
	rew::AABB CalculateAABB();

};



// ----------------------------------------------------------------------

//非索引结构的Mesh(未加入SubMesh)

class IndexlessMesh : public Component
{
public:
	int numVertices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec3> normals;
public:
	IndexlessMesh();

	int bufferId;
};

//--------------------- Functions ----------------------------------
glm::vec3 GetTangent(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec3 normal);





