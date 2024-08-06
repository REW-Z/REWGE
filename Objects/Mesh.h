#pragma once


//----------------------------------------------------------
//1���Ƽ�ʹ���Զ����ģ���ļ����ͣ���߶�ȡ�ٶȣ�������ÿ�ζ�����obj�ļ���
//-----------------------class define-----------------------  

//----------------------------------------------------------
//Blender������Obj�ļ�����������ϵCCW���ͱ�����һ�¡�    
//----------------------------------------------------------


//-----------------------class define-----------------------

class SubMesh
{
public:
	//Material material; //���£����ʺ����������
	int idx;
	std::vector<int> inds;
};

//-----------------------class define-----------------------

//�������ṹ��Mesh(U3D���)  

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

//�������ṹ��Mesh(δ����SubMesh)

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





