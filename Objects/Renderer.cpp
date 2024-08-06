
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




//----------------------通用变量----------------------

GLuint mLoc, vLoc, mvLoc, projLoc, nLoc, sLoc;
GLuint envAmbLoc, dirLightDirLoc, dirLightColorLoc, mAmbLoc, mDiffLoc, mSpecLoc, mNsLoc;

GLuint widthLoc, heightLoc;
glm::mat4 mMat, vMat, mvMat, pMat, invTrMat;

//----------------------阴影相关通用变量----------------------

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

	isBoundsDirty = true;//默认没有计算过AABB

	//加入Renderers列表
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


	//使用着色器 
	//TODO:不同的材质使用不同着色器（而不是默认第0个）
	assert(renderer->materials.size() > 0);
	renderer->materials[0]->shader->passes[0]->Use();

	if (renderer != nullptr && renderer->mesh != nullptr)
	{
		//绑定Mesh的VBO，如果未绑定
		if (renderer->mesh->binding_vbo == false)
		{
			renderer->mesh->SetupAPI();
		}
		assert(renderer->mesh->binding_vbo);

		//世界环境设置
		World& currentWorld = World::GetCurrentWorld();
		WorldSettings * envSettings = &World::GetCurrentWorld().worldSettings;

		// 绘制当前模型
		mMat = renderer->gameObject->transform->GetLocalToWorldMat();
		mvMat = vMat * mMat;
		invTrMat = transpose(inverse(mvMat));//逆转置矩阵
		shadowMVP2 = RenderSystem::Instance()->b * lightPmatrix * lightVmatrix * mMat;//b矩阵用于将光源空间变换到纹理贴图空间

		assert(materials.size() > 0);//至少有一个材质  

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


		//事实上我们还是可以在glUseProgram之外绑定数据的——乃至直接在初始化时。这得益于glProgramUniform系列函数的引入，它比起往常的glUniform要多一个参数用来接收一个ShaderProgram的ID。
		/*glProgramUniform4fv(renderingProgram, ambLoc, 1, glm::value_ptr(ambientColor));
		glProgramUniform4fv(renderingProgram, dirLightColorLoc, 1, glm::value_ptr(lightColor));
		glProgramUniform3fv(renderingProgram, dirLightDirLoc, 1, glm::value_ptr(lightDir));
		glProgramUniform4fv(renderingProgram, mDiffLoc, 1, glm::value_ptr(materialDiff));
		glProgramUniform4fv(renderingProgram, mSpecLoc, 1, glm::value_ptr(materialSpec));
		glProgramUniform1fv(renderingProgram, mGlosLoc, 1, &materialGloss);*/


		//绑定VAO后就不再需要重复下列的属性指令
		glBindVertexArray(renderer->mesh->vao);

		////传递顶点属性-顶点位置
		//glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vbo[0]);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(0);

		////传递顶点属性-顶点纹理坐标
		//glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vbo[1]);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(1);

		////传递顶点属性-顶点法线
		//glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vbo[2]);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(2);

		////传递顶点属性-顶点切线
		//glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vbo[3]);
		//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(3);





		//激活环境纹理
		glActiveTexture(GL_TEXTURE1);			//纹理单元1-绑定环境纹理
		glBindTexture(GL_TEXTURE_CUBE_MAP, envSettings->texSkybox);

		//绘制前绑定索引缓冲区
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->mesh->ebo);
		size_t submeshCount = renderer->mesh->subMeshes.size();
		GLuint lastCount = 0;

		//cout << renderer->gameObject->name << "的子网格数量：" << submeshCount << endl;

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

			//std::cout << gameObject->name << "的第" << j << "个子网格的漫反射颜色：" << matOfSub->diffuse.x << "," << matOfSub->diffuse.y << "," << matOfSub->diffuse.z << std::endl;


			glActiveTexture(GL_TEXTURE2);			//纹理单元2-绑定物体漫反射贴图
			glBindTexture(GL_TEXTURE_2D, matOfSub->textureDiffuse->handle);
			glActiveTexture(GL_TEXTURE3);			//纹理单元3-绑定物体法线贴图
			glBindTexture(GL_TEXTURE_2D, matOfSub->textureBump->handle);

			//std::cout << this->gameObject->name << "正在绘制子网格：" << j  <<  "三角形顶点数：" << renderer->mesh->subMeshes[j].inds.size() << std::endl;

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

	//使用着色器
	RenderSystem::Instance()->shaderShadow->passes[0]->Use();


	//绑定Mesh的VBO，如果未绑定
	if (renderer->mesh->binding_vbo == false)
	{
		renderer->mesh->SetupAPI();
	}

	// 绘制当前模型
	mMat = renderer->gameObject->transform->GetLocalToWorldMat();


	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	sLoc = glGetUniformLocation(RenderSystem::Instance()->shaderShadow->passes[0]->renderProgram, "shadowMVP");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindVertexArray(renderer->mesh->vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->mesh->ebo);

	glDrawElements(GL_TRIANGLES, renderer->mesh->inds.size(), GL_UNSIGNED_INT, 0);// 渲染阴影不区分材质、部分
}
