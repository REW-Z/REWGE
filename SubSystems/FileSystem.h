#pragma once

class Path
{
public:
	//Instance
	std::string fullName;

public:
	Path() {};
	Path(std::string new_fullname);
	Path(const char* new_fullname);

	std::string GetExtension();
	std::string GetName();
	std::string GetNameWithOutExtension();
	std::string GetDirectory();
	bool HasExtension();
public:
	//Static
	static std::string GetExtension(std::string fullName);
	static std::string GetName(std::string fullName);
	static std::string GetFullNameWithOutExtension(std::string fullName);
	static std::string GetNameWithOutExtension(std::string fullName);
	static std::string GetDirectory(std::string fullName);
	static bool HasExtension(std::string fullName);
};



class File
{
public:
	Path filePath;
protected:
	bool isBinary;
public:
	bool IsBinary() { return isBinary; }
};
class FileBinary : public File
{
public:
	void * buffer;
	int length;
public:
	FileBinary();
};
class FileText : public File
{
public:
	std::string content;
public:
	FileText();
};



class AsyncFileResult
{
public:
	bool isCompelete;
	float progress;

	File* filePtr;
};



class FileSystem
{
public:
	enum class IOSLIB
	{
		StandardC = 0,
		Fstream = 1,
		OS = 2,
	} DSAD;
	enum class MODE
	{
		NORMAL = 1,
		BINARY = 2,
	};

	//����ָ����չ���������ļ�(����)
	static void FindAllFilesWithRegex(std::string path, std::regex flag, std::vector<std::string>& files);


	//�ļ�����
	static bool Exist(std::string path);

	//��ȡ�ļ�ͨ�ú���(����)
	static void LoadFileBinary(std::string path, FileBinary* filePtr, AsyncFileResult* result = nullptr, IOSLIB ioslib_flag = IOSLIB::StandardC);

	static void LoadFileText(std::string path, FileText* filePtr, AsyncFileResult* result = nullptr, IOSLIB ioslib_flag = IOSLIB::Fstream);

	//ͬ����ȡ�ļ�(����)
	static File* LoadFile(std::string path, IOSLIB ioslib_flag = IOSLIB::StandardC, MODE mode = MODE::BINARY);

	//�첽��ȡ�ļ�(����)
	static AsyncFileResult * LoadFileAsync(std::string path, IOSLIB flag = IOSLIB::StandardC, MODE mode = MODE::BINARY);


	//��ȡ�ı��ļ�
	static std::string LoadText(std::string path);



	//��������ļ�
	static void WriteObject(std::string path, void* objPtr, size_t size);
	//���ļ���ȡ����
	static void ReadObject(std::string path, void* objPtr, size_t size);
};



/*
NOTE:����ios::binary��

���������������ڶԻ��з��Ĵ���ʽ��ͬ��������ʷԭ��Windows����ϵͳ���������ַ�(\r\n)����ʾ���з��ģ�
��Unix����ϵͳȴ���õ����ַ�(\n)����ʾ���з��ġ�
��ˣ��ڴ����ļ���ʱ�����ָ������ios::binary��ʽ�򿪣���ô���з����ǵ��ַ��ģ����򣬾Ͳ���Windows����ϵͳ��˫�ַ���

(CSDN)
*/