#pragma once

#include "REWGE.h"
//#include <ctime>
//#include <typeinfo>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <stack>
#include <map>
#include <regex>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <functional>
#include <algorithm>

#include "REWCPPL.h"
#include "Utils.h"

#include "Memory/Allocator.h"

#include "Rendering/FBO.h"

#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/BaseComponents.h"
#include "Objects/Camera.h"
#include "Objects/Light.h"
#include "Objects/Shader.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"
#include "Objects/Mesh.h"
#include "Objects/Renderer.h"
#include "Objects/World.h"  


#include "SubSystems/Debug.h"
#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"







// ----------------------------------- Resource Mgr ------------------------------------------


std::unordered_map<std::string, Object*> ResourceManager::loadedResources;


Object* ResourceManager::Load(TypeEnum type, std::string path)
{
	Object* asset = nullptr;
	bool load_successful = false;

	//Debug::Log("尝试载入资源- 资源路径：");
	//Debug::Log(path);

	//Try Find In Loaded Assets
	std::unordered_map<std::string, Object*>::const_iterator it = loadedResources.find(path);
	if (it != loadedResources.end())
	{
		//Debug::Log("已经找到已载入资源- 资源名：");
		//Debug::WriteLogFile();
		//Debug::Log(path);
		return reinterpret_cast<Object*>(it->second);
	}
	else
	{
		//Debug::Log("未找到已载入资源，载入中");
		//Debug::WriteLogFile();
	}


	switch (type)
	{
		// IS TEXTRUE2D
		case TypeEnum::TYPE_Texture2D:
		{
			Texture2D* importedTex = ResourceImporter::ImportTexture2D(path);
			asset = importedTex as(Object);
		}
		break;
		case TypeEnum::TYPE_MaterialShader:
		{
			MaterialShader* importedShader = ResourceImporter::ImportGLSLShader(path);
			asset = importedShader as (Object);
		}
		break;
		default:
		{
			//按后缀名过滤示例
			//if (Path::GetExtension(path) == ".obj")
			//{
			//	//...
			//}
			//else
			//{
			//	Debug::Log("未支持该格式的模型！" + path);
			//}

			Debug::Log("未实现该类型载入的实现！！");
			return nullptr;
		}
		break;
	}


	if (asset != nullptr)
	{
		load_successful = true;
	}


	//添加到资源表
	if (load_successful)
	{
		auto item = std::pair<std::string, Object*>(path, dynamic_cast<Object*>(asset));
		ResourceManager::loadedResources.insert(item);
	}
	std::cout << "LoadEnd: " << path << std::endl;
	return asset;
}

bool ResourceManager::Unload(TypeEnum type, std::string path)
{
	Object* objToUnload = nullptr;
	std::unordered_map<std::string, Object*>::const_iterator it = loadedResources.find(path);
	if (it != loadedResources.end())
	{
		Object* obj = (*it).second;

		loadedResources.erase(it);

		Object::DeleteInstance(obj);

		return true;
	}
	return false;
}



// ----------------------------------- Resource Importer ------------------------------------------

MaterialShader* ResourceImporter::ImportGLSLShader(std::string path)
{
	MaterialShader* newshader = Object::CreateInstance(TypeEnum::TYPE_MaterialShader,  path) as (MaterialShader);
	newshader->name = Path::GetNameWithOutExtension(path);

	assert(FileSystem::Exist(path + ".vert.glsl"));
	assert(FileSystem::Exist(path + ".frag.glsl"));


	auto pass = NEW(ShaderPass)ShaderPass(); newshader->passes.push_back(pass);

	pass->shaders[static_cast<int>(ShaderType::VertShader)] = NEW(Shader)Shader(ShaderType::VertShader);
	pass->shaders[static_cast<int>(ShaderType::FragShader)] = NEW(Shader)Shader(ShaderType::FragShader);

	pass->shaders[static_cast<int>(ShaderType::VertShader)] ->content = FileSystem::LoadText(path + ".vert.glsl");
	pass->shaders[static_cast<int>(ShaderType::FragShader)]->content = FileSystem::LoadText(path + ".frag.glsl");

	newshader->Compile();

	return newshader;
}

Texture2D * ResourceImporter::ImportTexture2D(std::string path)
{
	//新建资源对象
	Texture2D* tex = Object::CreateInstance(TypeEnum::TYPE_Texture2D, path) as (Texture2D);
	tex->name = Path::GetNameWithOutExtension(path);


	GLuint textureID;


	//读取文件到内存缓冲区
	FileBinary* filePtr = (FileBinary*)FileSystem::LoadFile(path, FileSystem::IOSLIB::Fstream); cout << "TEST PATH : " << path << endl;

	//从缓冲区数据转换为OpenGL纹理( 该函数在glInit()之前调用会崩溃闪退 ！ )
	try
	{
		////一、直接从文件缓冲区读取为句柄
		//textureID = SOIL_load_OGL_texture_from_memory(reinterpret_cast<const unsigned char*>(filePtr->buffer), filePtr->length, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

		//二、从文件缓冲区读取 然后转换为GL句柄 (需要先初始化glew，否则会报错！)
		int channels;
		int width;
		int height;
		unsigned char* rawData = SOIL_load_image_from_memory(reinterpret_cast<const unsigned char*>(filePtr->buffer), filePtr->length, &width, &height, &channels, SOIL_LOAD_AUTO);
		textureID = SOIL_create_OGL_texture(rawData, &width, &height, channels, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

		tex->rawData = (std::byte*)rawData;
		tex->width = width;
		tex->height = height;
		tex->channels = channels;
		//SOIL_free_image_data(ptr); //释放原始数据（不会导致OpenGL纹理丢失）（仅作为原始数据用于其他用途）

	}
	catch (std::exception exc)
	{
		Debug::Log("SOIL转换错误1");
		Debug::WriteLogFile();
	}


	//设置OGL Handle
	if (textureID == 0)
	{
		Debug::Log("SOIL转换错误2");
	}
	tex->handle = textureID;


	//释放文件缓冲区
	Allocator::GetAllocator()->Deallocate((byte*)(filePtr->buffer), 0);

	//初始化设置
	tex->Init();

	cout << "已经载入Texture2D：" << path << endl;

	return tex;
}

GameObject* ResourceImporter::ImportObjModel(std::string filePath)
{
	MeshImporter modelImporter = MeshImporter();

	//先导入材质，再导入网格
	modelImporter.parseMTL(filePath.c_str());
	modelImporter.parseOBJ(filePath.c_str());


	assert(modelImporter._meshes.size() == modelImporter._obj_names.size());

	GameObject& rootObject = GameObjectUtility::CreateGameObject(string(filePath) + ".gameobject");
	
	//default shader
	MaterialShader* defaultShader = ResourceManager::Load(TypeEnum::TYPE_MaterialShader, "Shaders\\BlinnPhong") as (MaterialShader);

	//meshes
	for (size_t i = 0; i < modelImporter._meshes.size(); ++i)
	{
		GameObject& child = GameObjectUtility::CreateGameObject(string(filePath) + "&" + modelImporter._obj_names[i] + ".gameobject");
		MeshRenderer* renderer = rootObject.AddComponent(TypeEnum::TYPE_MeshRenderer) as(MeshRenderer);

		//mesh
		renderer->mesh = modelImporter._meshes[i];

		//submeshes and mats
		for (size_t s = 0; s < modelImporter._meshes[i]->subMeshes.size(); ++s)
		{
			Material* targetMat = nullptr;
			unordered_map<int, Material*>::iterator itor = modelImporter._submesh_mat_map.find(s);
			if (itor != modelImporter._submesh_mat_map.end())
			{
				targetMat = (*itor).second;
				//set default shader
				targetMat->shader = defaultShader;
			}
			//std::cout << renderer->gameObject->name << "的第"<< s <<"个子网格的材质名称：" << targetMat->name << std::endl;
			renderer->materials.push_back(targetMat);
		}

		child.transform->SetParent(rootObject.transform);
	}

	return &rootObject;
}

IndexlessMesh* ResourceImporter::ImportObjIndexlessMesh(std::string filePath)
{
	IndexlessMesh* mesh = NEW(IndexlessMesh) IndexlessMesh();
	IndexlessModelImporter modelImporter = IndexlessModelImporter();
	modelImporter.parseOBJ(filePath.c_str());
	mesh->numVertices = modelImporter.getNumVertices();
	std::vector<float> verts = modelImporter.getVertices();
	std::vector<float> tcs = modelImporter.getTextureCoordinates();
	std::vector<float> normals = modelImporter.getNormals();

	for (int i = 0; i < mesh->numVertices; i++)
	{
		mesh->vertices.push_back(glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]));
		mesh->texcoords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
		mesh->normals.push_back(glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]));
	}

	mesh->bufferId = -1;

	return mesh;
}




//--------------------------------------------------------------------------------------------

#pragma region MeshImporter
MeshImporter::MeshImporter() {}

void MeshImporter::parseOBJ(const char* filePath)
{
	//NEW MESH
	Mesh* mesh = Object::CreateInstance(TypeEnum::TYPE_Mesh, string(filePath) + "$" + "rootObj") as (Mesh);

	//fs
	ifstream fileStream(filePath, ios::in);

	//temp
	float x, y, z;
	string content;
	string line = "";

	// define vars about mesh part
	int indexPart = -1;//index of current part
	vector<int>* meshPartIndsPtr = nullptr; //part inds array pointer 

	// define vars about vert/tc/norm/tangent
	vector<glm::ivec4> v_t_n;//vtc数组长度为处理后可索引顶点数。
	bool isExist;

	glm::vec3 pos_temp[3];
	glm::vec2 tc_temp[3];
	glm::vec3 norm_temp[3];
	bool need_push[3];
	glm::vec3 tangent1, tangent2, tangent3;

	while (!fileStream.eof())
	{
		getline(fileStream, line);
		if (line.compare(0, 2, "v ") == 0)
		{
			stringstream ss(line.erase(0, 1));
			ss >> x; ss >> y; ss >> z;
			rawVertVals.push_back(x);
			rawVertVals.push_back(y);
			rawVertVals.push_back(z);
		}
		if (line.compare(0, 2, "vt") == 0)
		{
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			rawTexcoordsVals.push_back(x);
			rawTexcoordsVals.push_back(y);
		}
		if (line.compare(0, 2, "vn") == 0)
		{
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			rawNormVals.push_back(x);
			rawNormVals.push_back(y);
			rawNormVals.push_back(z);
		}
		if (line.compare(0, 7, "usemtl ") == 0)
		{
			SubMesh part;
			meshParts.push_back(part); indexPart++;
			part.idx = indexPart;
			meshPartIndsPtr = &(meshParts[indexPart].inds);

			string matName = line.erase(0, 7);
			Material* targetMat = nullptr;
			for (auto m : _materials)
			{
				if (m->name == matName) { targetMat = m; break; }
			}
			if (targetMat == nullptr) Debug::Log(string("没找到子网格的材质：" + matName));
			this->_submesh_mat_map.insert({ part.idx, targetMat });
		}
		if (line.compare(0, 2, "f ") == 0)
		{
			string oneCorner, v, t, n;
			stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; i++)
			{
				getline(ss, oneCorner, ' ');
				stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');

				int vertInd = 0;
				int tcInd = 0;
				int normInd = 0;

				if (v != "") vertInd = stoi(v);
				if (t != "") tcInd = stoi(t);
				if (n != "") normInd = stoi(n);

				//顶点数据查找检索（从原始数据）
				pos_temp[i].x = rawVertVals[(vertInd - 1) * 3];
				pos_temp[i].y = rawVertVals[(vertInd - 1) * 3 + 1];
				pos_temp[i].z = rawVertVals[(vertInd - 1) * 3 + 2];

				tc_temp[i].x = rawTexcoordsVals[(tcInd - 1) * 2];
				tc_temp[i].y = rawTexcoordsVals[(tcInd - 1) * 2 + 1];

				norm_temp[i].x = rawNormVals[(normInd - 1) * 3];
				norm_temp[i].y = rawNormVals[(normInd - 1) * 3 + 1];
				norm_temp[i].z = rawNormVals[(normInd - 1) * 3 + 2];

				//判断
				isExist = false;
				int ind = 0;
				for (size_t i = 0; i < v_t_n.size(); i++)
				{
					if (v_t_n[i].x != vertInd)
					{
						break;
					}
					if (v_t_n[i].y != tcInd)
					{
						break;
					}
					if (v_t_n[i].z == normInd)
					{
						isExist = true;
						ind = v_t_n[i].w;
					}
				}
				//根据判断结果
				if (!isExist)
				{
					need_push[i] = true;

					triangleVerts.push_back(pos_temp[i].x);
					triangleVerts.push_back(pos_temp[i].y);
					triangleVerts.push_back(pos_temp[i].z);

					textureCoords.push_back(tc_temp[i].x);
					textureCoords.push_back(tc_temp[i].y);

					normals.push_back(norm_temp[i].x);
					normals.push_back(norm_temp[i].y);
					normals.push_back(norm_temp[i].z);
					//v_t_n存储
					int lastIndex = v_t_n.size() - 1;
					v_t_n.push_back(glm::ivec4(vertInd, tcInd, normInd, lastIndex + 1));
					//总索引和分部索引存储
					inds.push_back(lastIndex + 1);
					(*meshPartIndsPtr).push_back(lastIndex + 1);
				}
				else
				{
					need_push[i] = false;

					//总索引和分部索引压入
					inds.push_back(ind);
					(*meshPartIndsPtr).push_back(ind);
				}
				//pos/tc/norm read end
			}
			//triangle read end
			//面信息读取后 分别计算三个切线
			tangent1 = GetTangent(pos_temp[0], pos_temp[1], pos_temp[2], tc_temp[0], tc_temp[1], tc_temp[2], norm_temp[0]);
			tangent2 = GetTangent(pos_temp[1], pos_temp[2], pos_temp[0], tc_temp[1], tc_temp[2], tc_temp[0], norm_temp[1]);
			tangent3 = GetTangent(pos_temp[2], pos_temp[0], pos_temp[1], tc_temp[2], tc_temp[0], tc_temp[1], norm_temp[2]);
			if (need_push[0])
			{
				tangents.push_back(tangent1.x);
				tangents.push_back(tangent1.y);
				tangents.push_back(tangent1.z);
			}
			if (need_push[1])
			{
				tangents.push_back(tangent2.x);
				tangents.push_back(tangent2.y);
				tangents.push_back(tangent2.z);
			}
			if (need_push[2])
			{
				tangents.push_back(tangent3.x);
				tangents.push_back(tangent3.y);
				tangents.push_back(tangent3.z);
			}
		}
		//line read end
	}
	//关闭文件流
	fileStream.close();


	//填充MESH
	mesh->numVertices = (triangleVerts.size() / 3);
	for (int i = 0; i < mesh->numVertices; i++)
	{
		mesh->vertices.push_back(glm::vec3(triangleVerts[i * 3], triangleVerts[i * 3 + 1], triangleVerts[i * 3 + 2]));
		mesh->texcoords.push_back(glm::vec2(textureCoords[i * 2], textureCoords[i * 2 + 1]));
		mesh->normals.push_back(glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]));
		mesh->tangents.push_back(glm::vec3(tangents[i * 3], tangents[i * 3 + 1], tangents[i * 3 + 2]));
	}
	mesh->inds = inds;
	mesh->subMeshes = meshParts;




	//加入到导入器的Mesh列表
	this->_meshes.push_back(mesh);
	this->_obj_names.push_back("???");
}
void MeshImporter::parseMTL(const char* filePath)
{
	string objPath = filePath;
	string mtlPath = objPath.substr(0, objPath.rfind(".")) + ".mtl";
	string dir = objPath.substr(0, objPath.rfind("\\"));

	ifstream fileStream(mtlPath, ios::in);
	string line = "";


	Material * currentMat = NULL;
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		if (line.compare(0, 7, "newmtl ") == 0)
		{
			string name = line.erase(0, 7);

			Material* newmat = Object::CreateInstance(TypeEnum::TYPE_Material, string(filePath) + "$" + name) as (Material);
			newmat->name = name;
			newmat->hasDiffTex = false;
			newmat->hasBumpTex = false;
			this->_materials.push_back(newmat);


			//默认空纹理
			assert(Texture2D::emptyTexture);
			newmat->textureDiffuse = Texture2D::emptyTexture;
			newmat->textureBump = Texture2D::emptyTexture;


			currentMat = newmat;
		}
		if (line.compare(0, 2, "Ka") == 0)
		{
			float x, y, z;
			stringstream ss(line.erase(0, 3));
			ss >> x;
			ss >> y;
			ss >> z;
			(*currentMat).ambient = glm::vec3(x, y, z);
		}
		if (line.compare(0, 2, "Kd") == 0)
		{
			float x, y, z;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			(*currentMat).diffuse = glm::vec3(x, y, z);
		}
		if (line.compare(0, 2, "Ks") == 0)
		{
			float x, y, z;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			(*currentMat).specular = glm::vec3(x, y, z);
		}
		if (line.compare(0, 2, "Ke") == 0)
		{
			float x, y, z;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			(*currentMat).emission = glm::vec3(x, y, z);
		}
		if (line.compare(0, 2, "Ns") == 0)
		{
			float Ns = stof(line.erase(0, 2));
			(*currentMat).Ns = Ns;
		}
		if (line.compare(0, 2, "Ni") == 0)
		{

		}
		if (line.compare(0, 2, "d ") == 0)
		{

		}
		if (line.compare(0, 6, "map_Kd") == 0)
		{
			string texpath = dir + "\\" + line.erase(0, 7);

			Texture2D* texMain = dynamic_cast<Texture2D*>(ResourceManager::Load(TypeEnum::TYPE_Texture2D, texpath));


			//设置纹理
			(*currentMat).textureDiffuse = texMain;
			(*currentMat).hasDiffTex = true;

		}
		if (line.compare(0, 8, "map_Bump") == 0)
		{
			string texpath = dir + "\\" + line.erase(0, 9);

			Texture2D* texBump = dynamic_cast<Texture2D*>(ResourceManager::Load(TypeEnum::TYPE_Texture2D, texpath.c_str()));


			//设置纹理
			(*currentMat).textureBump = texBump;
			(*currentMat).hasBumpTex = true;
		}
	}
	fileStream.close();
}

#pragma endregion

// -------------------------------------------------------------------------------------------



#pragma region IndexlessMeshImporter
IndexlessModelImporter::IndexlessModelImporter() {}

void IndexlessModelImporter::parseOBJ(const char* filePath)
{
	float x, y, z;
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		if (line.compare(0, 2, "v ") == 0)
		{
			stringstream ss(line.erase(0, 1));
			ss >> x; ss >> y; ss >> z;
			vertVals.push_back(x);
			vertVals.push_back(y);
			vertVals.push_back(z);
		}
		if (line.compare(0, 2, "vt") == 0)
		{
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			stVals.push_back(x);
			stVals.push_back(y);
		}
		if (line.compare(0, 2, "vn") == 0)
		{
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			normVals.push_back(x);
			normVals.push_back(y);
			normVals.push_back(z);
		}
		if (line.compare(0, 2, "f ") == 0)
		{
			string oneCorner, v, t, n;
			stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; i++)
			{
				getline(ss, oneCorner, ' ');
				stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');

				int vertRef = 0;
				int tcRef = 0;
				int normRef = 0;

				if (v != "") vertRef = (stoi(v) - 1) * 3;
				if (t != "") tcRef = (stoi(t) - 1) * 2;
				if (n != "") normRef = (stoi(n) - 1) * 3;

				triangleVerts.push_back(vertVals[vertRef]);
				triangleVerts.push_back(vertVals[vertRef + 1]);
				triangleVerts.push_back(vertVals[vertRef + 2]);

				textureCoords.push_back(stVals[tcRef]);
				textureCoords.push_back(stVals[tcRef + 1]);

				normals.push_back(normVals[normRef]);
				normals.push_back(normVals[normRef + 1]);
				normals.push_back(normVals[normRef + 2]);
			}
		}
	}
	fileStream.close();
}
int IndexlessModelImporter::getNumVertices() { return (triangleVerts.size() / 3); }
std::vector<float> IndexlessModelImporter::getVertices() { return triangleVerts; }
std::vector<float> IndexlessModelImporter::getTextureCoordinates() { return textureCoords; }
std::vector<float> IndexlessModelImporter::getNormals() { return normals; }
#pragma endregion




//--------------------------------------------------------------------------------------------
