#pragma once


class Light : public Behaviour
{
	OBJECT_TYPE_DECLARE(Light)

public:
	Light();
public:
	glm::vec3 dir();
	glm::vec4 color;
public:
	////“ı”∞Ã˘Õº
	//GLuint shadowMap;
	//GLuint shadowBuffer;
	// 
	//void SetupShadowMap();
};
