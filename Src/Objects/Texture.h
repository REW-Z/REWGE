#pragma once


class Texture2D : public Object
{
	OBJECT_TYPE_DECLARE(Texture2D)
public:
	static Texture2D * emptyTexture;
public :
	int width;
	int height;
	int channels;

	GLuint handle; // SOIL's DOCS: OpenGL texture (unsigned int) handle
	std::byte* rawData = nullptr;  //  CRE：raw data 卸载后依然可以显示纹理，OGL是归OpenGL管理的，SOIL不负责OGLhandle分配和释放。。  

public:
	~Texture2D();

	void Init();

	void SaveAs(std::string path, int image_type = SOIL_SAVE_TYPE_PNG);
	
};
