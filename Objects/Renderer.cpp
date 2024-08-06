
#include "REWGE.h"

#include <ctime>
#include <typeinfo>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <fstream>

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <stack>
#include <regex>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <functional>

#include "REWCPPL.h"
#include "Utils.h"

#include "Memory/Allocator.h"
//#include "Memory//StackAllocator.hpp"

#include "Rendering/FBO.h"

#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/BaseComponents.h"
#include "Objects/Camera.h"
#include "Objects/Light.h"
#include "Objects/Texture.h"
#include "Objects/Shader.h"
#include "Objects/Material.h"
#include "Objects/Mesh.h"
#include "Objects/World.h"


#include "SubSystems/FileSystem.h"
#include "SubSystems/ResourceManager.h"
#include "SubSystems/RenderSystem.h"


#include "Objects/Renderer.h"




//----------------------ͨ�ñ���----------------------

GLuint mLoc, vLoc, mvLoc, projLoc, nLoc, sLoc;
GLuint envAmbLoc, dirLightDirLoc, dirLightColorLoc, mAmbLoc, mDiffLoc, mSpecLoc, mNsLoc;

GLuint widthLoc, heightLoc;
glm::mat4 mMat, vMat, mvMat, pMat, invTrMat;

//----------------------��Ӱ���ͨ�ñ���----------------------

glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
















//---------------------------------------------- Renderer -------------------------------------------
OBJECT_TYPE_IMPLEMENT(Renderer, Component)

list<Renderer*>* Renderer::GetRendererList()
{
	static list<Renderer*> list;
	return &list;
}


Renderer::Renderer()
{
	enable = true;
	isInScene = false;

	isBoundsDirty = true;//Ĭ��û�м����AABB

	//����Renderers�б�
	GetRendererList()->push_front(this);
}

void Renderer::Init()
{
	assert(gameObject);

	gameObject->onTransformChange.Add(this, std::bind(&Renderer::SetDirty, this));
}





bool Renderer::IsInScene()
{
	return this->isInScene;
}

bool Renderer::ShouldInScene()
{
	return (gameObject != nullptr && gameObject->active && this->enable);
}

void Renderer::SetIsInScene(bool value, int newhandle)
{
	this->isInScene = value;
	this->scenehandle = newhandle;
}

void Renderer::SetEnable(bool value)
{
	this->enable = value;
}

rew::AABB Renderer::GetWorldAABB()
{
	UpdateAABB();
	return this->worldAABB;
}

void Renderer::Draw() //abstract
{	
	return;
}

void Renderer::DrawShadow()//abstract
{
	return;
}


//---------------------------------------------- MeshRenderer -------------------------------------------
OBJECT_TYPE_IMPLEMENT(MeshRenderer, Renderer)


void MeshRenderer::UpdateAABB()
{
	assert(mesh);

	if (isBoundsDirty)
	{
		//local
		localAABB = mesh->CalculateAABB();


		//std::cout << "CalLocalAABB:" << gameObject->name << std::endl;
		//std::cout << "center:" << localAABB.center.x << "," << localAABB.center.y << "," << localAABB.center.z << std::endl;
		//std::cout << "hextend:" << localAABB.half_length.x << "," << localAABB.half_length.y << "," << localAABB.half_length.z << std::endl;

		//world
		auto worldCenter = (gameObject->transform->GetLocalToWorldMat() * glm::vec4(localAABB.center.x, localAABB.center.y, localAABB.center.z, 1.0f));
		auto worldHalfLen = (gameObject->transform->GetLocalToWorldMat() * glm::vec4(localAABB.half_length.x, localAABB.half_length.y, localAABB.half_length.z, 0.0f));
		worldAABB.center = rew::Vector3(worldCenter.x, worldCenter.y, worldCenter.z);
		worldAABB.half_length = rew::Vector3(worldHalfLen.x, worldHalfLen.y, worldHalfLen.z);

		//std::cout << "CalAABB:" << gameObject->name << std::endl;
		//std::cout << "center:" << worldAABB.center.x << "  ,  " << worldAABB.center.y << "  ,  " << worldAABB.center.z << std::endl;
		//std::cout << "hextend:" << worldAABB.half_length.x << "  ,  " << worldAABB.half_length.y << "  ,  " << worldAABB.half_length.z << std::endl;
	}
}

void MeshRenderer::Draw()
{
	MeshRenderer* renderer = this;


	//ʹ����ɫ�� 
	//TODO:��ͬ�Ĳ���ʹ�ò�ͬ��ɫ����������Ĭ�ϵ�0����
	assert(renderer->materials.size() > 0);
	renderer->materials[0]->shader->passes[0]->Use();

	if (renderer != nullptr && renderer->mesh != nullptr)
	{
		//��Mesh��VBO�����δ��
		if (renderer->mesh->binding_vbo == false)
		{
			renderer->mesh->SetupAPI();
		}
		assert(renderer->mesh->binding_vbo);

		//���绷������
		World& currentWorld = World::GetCurrentWorld();
		WorldSettings * envSettings = &World::GetCurrentWorld().worldSettings;

		// ���Ƶ�ǰģ��
		mMat = renderer->gameObject->transform->GetLocalToWorldMat();
		mvMat = vMat * mMat;
		invTrMat = transpose(inverse(mvMat));//��ת�þ���
		shadowMVP2 = RenderSystem::Instance()->b * lightPmatrix * lightVmatrix * mMat;//b�������ڽ���Դ�ռ�任��������ͼ�ռ�

		assert(materials.size() > 0);//������һ������  

		mLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "m_matrix");
		vLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "v_matrix");
		mvLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "mv_matrix");
		projLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "proj_matrix");
		nLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "norm_matrix");
		sLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "shadowMVP");

		envAmbLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "env_ambient");
		dirLightDirLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "light.dir");
		dirLightColorLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "light.color");

		widthLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "window_width");
		heightLoc = glGetUniformLocation(materials[0]->shader->passes[0]->renderProgram, "window_height");



		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
		glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
		glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));
		glUniform4fv(envAmbLoc, 1, glm::value_ptr(envSettings->envionmentAmbient));
		glUniform4fv(dirLightColorLoc, 1, glm::value_ptr(RenderScene::Instance()->mainLight->color));
		glUniform3fv(dirLightDirLoc, 1, glm::value_ptr(RenderScene::Instance()->mainLight->dir()));

		glUniform1iv(widthLoc, 1, &RenderSystem::Instance()->frame_buffer_width);
		glUniform1iv(heightLoc, 1, &RenderSystem::Instance()->frame_buffer_height);


		//��ʵ�����ǻ��ǿ�����glUseProgram֮������ݵġ�������ֱ���ڳ�ʼ��ʱ���������glProgramUniformϵ�к��������룬������������glUniformҪ��һ��������������һ��ShaderProgram��ID��
		/*glProgramUniform4fv(renderingProgram, ambLoc, 1, glm::value_ptr(ambientColor));
		glProgramUniform4fv(renderingProgram, dirLightColorLoc, 1, glm::value_ptr(lightColor));
		glProgramUniform3fv(renderingProgram, dirLightDirLoc, 1, glm::value_ptr(lightDir));
		glProgramUniform4fv(renderingProgram, mDiffLoc, 1, glm::value_ptr(materialDiff));
		glProgramUniform4fv(renderingProgram, mSpecLoc, 1, glm::value_ptr(materialSpec));
		glProgramUniform1fv(renderingProgram, mGlosLoc, 1, &materialGloss);*/


		//��VAO��Ͳ�����Ҫ�ظ����е�����ָ��
		glBindVertexArray(renderer->mesh->vao);

		////���ݶ�������-����λ��
		//glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vbo[0]);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(0);

		////���ݶ�������-������������
		//glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vbo[1]);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(1);

		////���ݶ�������-���㷨��
		//glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vbo[2]);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(2);

		////���ݶ�������-��������
		//glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vbo[3]);
		//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(3);





		//���������
		glActiveTexture(GL_TEXTURE1);			//����Ԫ1-�󶨻�������
		glBindTexture(GL_TEXTURE_CUBE_MAP, envSettings->texSkybox);

		//����ǰ������������
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->mesh->ebo);
		size_t submeshCount = renderer->mesh->subMeshes.size();
		GLuint lastCount = 0;

		//cout << renderer->gameObject->name << "��������������" << submeshCount << endl;

		for (size_t j = 0; j < submeshCount; j++)
		{
			mAmbLoc = glGetUniformLocation(materials[j]->shader->passes[0]->renderProgram, "material.ambient");
			mDiffLoc = glGetUniformLocation(materials[j]->shader->passes[0]->renderProgram, "material.diffuse");
			mSpecLoc = glGetUniformLocation(materials[j]->shader->passes[0]->renderProgram, "material.specular");
			mNsLoc = glGetUniformLocation(materials[j]->shader->passes[0]->renderProgram, "material.Ns");

			Material* matOfSub = renderer->materials[j];

			glUniform4fv(mAmbLoc, 1, glm::value_ptr(matOfSub->ambient));
			glUniform4fv(mDiffLoc, 1, glm::value_ptr(matOfSub->diffuse));
			glUniform4fv(mSpecLoc, 1, glm::value_ptr(matOfSub->specular));
			glUniform1fv(mNsLoc, 1, &(matOfSub->Ns));

			//std::cout << gameObject->name << "�ĵ�" << j << "�����������������ɫ��" << matOfSub->diffuse.x << "," << matOfSub->diffuse.y << "," << matOfSub->diffuse.z << std::endl;


			glActiveTexture(GL_TEXTURE2);			//����Ԫ2-��������������ͼ
			glBindTexture(GL_TEXTURE_2D, matOfSub->textureDiffuse->handle);
			glActiveTexture(GL_TEXTURE3);			//����Ԫ3-�����巨����ͼ
			glBindTexture(GL_TEXTURE_2D, matOfSub->textureBump->handle);

			//std::cout << this->gameObject->name << "���ڻ���������" << j  <<  "�����ζ�������" << renderer->mesh->subMeshes[j].inds.size() << std::endl;

			glDrawElements(
				GL_TRIANGLES,
				(GLuint)(renderer->mesh->subMeshes[j].inds.size()),
				GL_UNSIGNED_INT,
				(void*)(sizeof(GLuint) * lastCount)
			);

			lastCount += (GLuint)(renderer->mesh->subMeshes[j].inds.size());
		}
	}
}

void MeshRenderer::DrawShadow()
{
	MeshRenderer* renderer = this;

	//ʹ����ɫ��
	RenderSystem::Instance()->shaderShadow->passes[0]->Use();


	//��Mesh��VBO�����δ��
	if (renderer->mesh->binding_vbo == false)
	{
		renderer->mesh->SetupAPI();
	}

	// ���Ƶ�ǰģ��
	mMat = renderer->gameObject->transform->GetLocalToWorldMat();


	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	sLoc = glGetUniformLocation(RenderSystem::Instance()->shaderShadow->passes[0]->renderProgram, "shadowMVP");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindVertexArray(renderer->mesh->vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->mesh->ebo);

	glDrawElements(GL_TRIANGLES, renderer->mesh->inds.size(), GL_UNSIGNED_INT, 0);// ��Ⱦ��Ӱ�����ֲ��ʡ�����
}
