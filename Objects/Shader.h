#pragma once

enum class ShaderType
{
	VertShader = 0,
	FragShader = 1,
	Count = 2
};
class Shader
{
public:
	std::string content;
	ShaderType shaderType;
	bool isCompiled;
	GLuint handle;

public:
	Shader(ShaderType shadertype);
	void Compile();
};


class ShaderPass
{
public:
	Shader* shaders[static_cast<int>(ShaderType::Count)];
	GLuint renderProgram;
private:
	bool isCompiled;
public:
	ShaderPass();
	void Compile();
	void Use();
};


class MaterialShader : public Object
{
	OBJECT_TYPE_DECLARE(MaterialShader)
public:
	std::vector<ShaderPass*> passes;

	bool isCompiled;
public:
	MaterialShader();
	void Compile();
};

