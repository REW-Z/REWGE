#pragma once

#include <io.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <regex>
#include <vector>
#include <thread>
#include "REWCPPL.h"

#include "Memory/Allocator.h"
#include "FileSystem.h"
#include "Debug.h"

using namespace std;





// ******************************* FileNa,e ********************************************************


Path::Path(std::string new_fullname)
{
	this->fullName = new_fullname;
}

Path::Path(const char* new_fullname)
{
	this->fullName = new_fullname;
}

std::string Path::GetExtension() { return Path::GetExtension(this->fullName); }

std::string Path::GetName() { return Path::GetName(this->fullName); }

std::string Path::GetNameWithOutExtension() { return Path::GetNameWithOutExtension(this->fullName); }

std::string Path::GetDirectory() { return Path::GetDirectory(this->fullName); }

bool Path::HasExtension() { return Path::HasExtension(this->fullName); }


std::string Path::GetExtension(std::string path)
{
	size_t offset = path.find_last_of('.');

	if (offset > 0 && offset < path.length())
	{
		return path.substr(offset);
	}
	else
	{
		return "";
	}
}

std::string Path::GetName(std::string path)
{
	size_t offset = path.find_last_of('\\');

	if (offset > 0 && offset < path.length() - 1)
		return path.substr(offset + 1);
	else
		return path;
}

std::string Path::GetFullNameWithOutExtension(std::string path)
{
	size_t offset = path.find_last_of('.');

	if (offset > 0 && offset < path.length() - 1)
		return path.substr(0, offset);
	else
		return path;
}
std::string Path::GetNameWithOutExtension(std::string path)
{
	size_t slashOffset = path.find_last_of('\\');
	size_t dotOffset = path.find_last_of('.');
	size_t non_ext_length = dotOffset - slashOffset - 1;

	if (dotOffset > 0 && dotOffset < path.length())
	{
		if (slashOffset > 0 && slashOffset < path.length() - 1)
		{
			if (non_ext_length > 0)
			{
				return path.substr(slashOffset + 1, non_ext_length);
			}
		}
	}
	return "";
}

std::string Path::GetDirectory(std::string path)
{
	size_t offset = path.find_last_of('\\');

	if (offset > 0 && offset < path.length() - 1)
		return path.substr(0, offset);
	else
		return path;
}

bool Path::HasExtension(std::string path)
{
	size_t offset = path.find_last_of('.');
	return (offset > 0 && offset < path.length());
}




FileBinary::FileBinary()
{
	isBinary = true;
}

FileText::FileText()
{
	isBinary = false;
}




// ******************************* FILE SYSTEN ********************************************************

void FileSystem::FindAllFilesWithRegex(std::string path, std::regex flag, std::vector<string>& files)
{
	std::intptr_t  hFile = 0;
	struct _finddata_t fileinfo;
	string p;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (regex_search(fileinfo.name, flag))
			{
				//files.push_back(p.assign(path).append("/").append(fileinfo.name));//文件名	
				files.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	sort(files.begin(), files.end());
}


bool FileSystem::Exist(std::string path)
{
	return std::filesystem::exists(path); 
}

void FileSystem::LoadFileBinary(std::string path, FileBinary* filePtr, AsyncFileResult* result, FileSystem::IOSLIB ioslib_flag)
{
	//文件名信息
	filePtr->filePath = Path(path);



	//异步初始化 (if is async)
	if (result != nullptr)
	{
		result->filePtr = filePtr;
		result->isCompelete = false;
		result->progress = 0.0f;
	}




	//C++模式 (fstream)
	if (ioslib_flag == IOSLIB::Fstream)
	{
		ifstream ifs = ifstream(path, (std::ios::binary | std::ios::in));

		ifs.seekg(0, ifs.end);
		auto len = ifs.tellg();

		filePtr->length = static_cast<int>(len);
		filePtr->buffer = (unsigned char*)  Allocator::GetAllocator()->Allocate(static_cast<int>(len) * sizeof(char), 0);

		//Allocator::GetAllocator()->WriteInfo("alloc to file system");

		ifs.seekg(0, ifs.beg);
		ifs.read(reinterpret_cast<char*>(filePtr->buffer), len);
		ifs.close();



		//Reset Async Result (if is async)
		if (result != nullptr)
		{
			result->progress = 1.0f;
			result->isCompelete = true;
		}
	}
	//标准C模式 (standard c)
	else if (ioslib_flag == IOSLIB::StandardC)
	{
		FILE* file;
		auto err = fopen_s(&file, path.c_str(), "rb+");
		if (err == 0)
		{
			fseek(file, 0l, SEEK_END);
			size_t len = ftell(file);

			filePtr->buffer = 0;
			int allocTimes = 0;
			while (filePtr->buffer == 0 && allocTimes < 5)
			{
				allocTimes++;
				filePtr->buffer = (unsigned char*)  Allocator::GetAllocator()->Allocate(len * sizeof(char), 0);
			}

			rewind(file);//fseek(START);
			auto beg = ftell(file); 
			fread((filePtr->buffer), sizeof(unsigned char), len, file);
			fclose(file);

			filePtr->length = static_cast<int>(len);



			//Reset Async Result (if is async)
			if (result != nullptr)
			{
				result->progress = 1.0f;
				result->isCompelete = true;
			}
		}
		else
		{
			//Debug::Log(std::to_string(err).c_str()); //输出开启文件的错误
		}
	}
	else
	{
		throw "读取方式未支持！";
	}
}

void FileSystem::LoadFileText(std::string path, FileText* filePtr, AsyncFileResult* result, IOSLIB ioslib_flag)
{
	//文件名信息
	filePtr->filePath = Path(path);



	//异步初始化 (if is async)
	if (result != nullptr)
	{
		result->filePtr = filePtr;
		result->isCompelete = false;
		result->progress = 0.0f;
	}


	//C++模式 (fstream)
	if (ioslib_flag == IOSLIB::Fstream)
	{
		ifstream ifs = ifstream(path, (std::ios::in));

		std::string line = "";
		while (!ifs.eof())
		{
			getline(ifs, line);
			filePtr->content.append(line + "\n");
		}
		ifs.close();

		//Reset Async Result (if is async)
		if (result != nullptr)
		{
			result->progress = 1.0f;
			result->isCompelete = true;
		}
	}
	else
	{
		throw "读取方式未支持！";
	}
}

File * FileSystem::LoadFile(std::string path, IOSLIB ioslib_flag, MODE mode)
{
	File* filePtr;

	if (mode == FileSystem::MODE::BINARY)
	{
		filePtr = NEW(FileBinary)FileBinary();
		LoadFileBinary(path, (FileBinary*)filePtr, nullptr, ioslib_flag);
	}
	else
	{
		filePtr = NEW(FileBinary)FileBinary();
		LoadFileText(path, (FileText*)filePtr, nullptr, ioslib_flag);
	}
	return filePtr;
}

AsyncFileResult* FileSystem::LoadFileAsync(std::string path, IOSLIB ioslib_flag, MODE mode)
{
	//异步结果
	AsyncFileResult * result = NEW(AsyncFileResult)AsyncFileResult();

	if (mode == FileSystem::MODE::BINARY)
	{
		FileBinary* filep = NEW(FileBinary)FileBinary();
		result->filePtr = filep;

		std::thread t = std::thread(FileSystem::LoadFileBinary, path, filep, result, ioslib_flag);
		t.detach();
	}
	else
	{
		FileText* filep = NEW(FileText)FileText();
		result->filePtr = filep;

		std::thread t = std::thread(FileSystem::LoadFileText, path, filep, result, ioslib_flag);
		t.detach();
	}
	//for (int i = 0; i < 10; ++i)
	//{
	//	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	//}
	return result;
}

std::string FileSystem::LoadText(std::string path)
{
	string result;

	ifstream ifs = ifstream(path, (std::ios::in));

	std::string line = "";
	while (!ifs.eof())
	{
		getline(ifs, line);
		result.append(line + "\n");
	}
	ifs.close();

	return result;
}

void FileSystem::WriteObject(std::string path, void* objPtr, size_t size)
{
	ofstream ofs;
	ofs.open("Resources\\test.model", ios::binary);
	ofs.write((char*)objPtr, size);
	ofs.close();
}

void FileSystem::ReadObject(std::string path, void* objPtr, size_t size)
{
	ifstream ifs;
	ifs.open("Resources\\test.model", ios::binary);
	ifs.read((char*)&objPtr, size);
	ifs.close();
}
