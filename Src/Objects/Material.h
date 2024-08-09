#pragma once

class Mesh;

class Material : public Object
{
	OBJECT_TYPE_DECLARE(Material)

public:
	std::string name;

	MaterialShader* shader;

	uint16 queueidx;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 emission;
	float Ns;

	bool hasDiffTex;
	bool hasBumpTex;
	
	Texture2D * textureDiffuse;
	Texture2D * textureBump;

public:
	Material();
};