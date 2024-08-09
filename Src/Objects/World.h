#pragma once


struct WorldSettings
{
	glm::vec4 envionmentAmbient;

	GLuint texSkybox;
};

class World: public Object
{
	OBJECT_TYPE_DECLARE(World)
public:
	std::list<GameObject *> gameObjects;

	WorldSettings worldSettings;

	bool AddRootGameObject(GameObject& rootGO);
	bool AddGameObject(GameObject& go);

public:
	static World& GetCurrentWorld();
};

