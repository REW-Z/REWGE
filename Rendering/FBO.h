#pragma once


/// FrameBuffer����һ�����ܵ���������������������Ⱦ�����������Щ�������ɫ����ȵȲ�ͬͨ�����������ӵ��������Ⱦ�������
///
/// ��Ļ֡�����Ϊ0,ֱ��ʹ��glBindFramebuffer(GL_FRAMEBUFFER, 0)���ɣ�����Ҫ�ֶ�����glGenFramebuffers�ֶ����ɡ�    
/// 


class FBO
{
public:
	GLuint fbo;
private:
	FBO();
	~FBO();

public:
	void Bind();
public:
	//static
	static FBO* Default();
	static FBO* Create();
	static void CurrentBindTextrue(GLuint texture, int level);
	static bool CurrentIsComplate();
};

