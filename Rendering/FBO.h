#pragma once


/// FrameBuffer类似一个流管道，负责接收来自相机的渲染输出，并将这些输出按颜色、深度等不同通道分流到附加的纹理或渲染缓冲对象。
///
/// 屏幕帧缓冲的为0,直接使用glBindFramebuffer(GL_FRAMEBUFFER, 0)即可，不需要手动调用glGenFramebuffers手动生成。    
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

