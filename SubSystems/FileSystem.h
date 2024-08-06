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

	//查找指定扩展名的所有文件(可用)
	static void FindAllFilesWithRegex(std::string path, std::regex flag, std::vector<std::string>& files);


	//文件存在
	static bool Exist(std::string path);

	//读取文件通用函数(可用)
	static void LoadFileBinary(std::string path, FileBinary* filePtr, AsyncFileResult* result = nullptr, IOSLIB ioslib_flag = IOSLIB::StandardC);

	static void LoadFileText(std::string path, FileText* filePtr, AsyncFileResult* result = nullptr, IOSLIB ioslib_flag = IOSLIB::Fstream);

	//同步读取文件(可用)
	static File* LoadFile(std::string path, IOSLIB ioslib_flag = IOSLIB::StandardC, MODE mode = MODE::BINARY);

	//异步读取文件(可用)
	static AsyncFileResult * LoadFileAsync(std::string path, IOSLIB flag = IOSLIB::StandardC, MODE mode = MODE::BINARY);


	//读取文本文件
	static std::string LoadText(std::string path);



	//保存对象到文件
	static void WriteObject(std::string path, void* objPtr, size_t size);
	//从文件读取对象
	static void ReadObject(std::string path, void* objPtr, size_t size);
};



/*
NOTE:关于ios::binary：

二者最大的区别在于对换行符的处理方式不同。由于历史原因，Windows操作系统是用两个字符(\r\n)来表示换行符的；
而Unix操作系统却是用单个字符(\n)来表示换行符的。
因此，在创建文件流时，如果指定了以ios::binary方式打开，那么换行符就是单字符的；否则，就采用Windows操作系统的双字符。

(CSDN)
*/