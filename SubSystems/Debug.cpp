#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include "Debug.h"

std::string Debug::logText;

void Debug::Log(std::string str)
{
	std::string strLine = str.append("\n");
	logText.append(strLine);
	std::printf(strLine.c_str());
}
void Debug::Log(const char* cstr)
{
	std::string strLine = std::string(cstr).append("\n");
	logText.append(strLine);
	std::printf(strLine.c_str());
}

void Debug::WriteLogFile()
{
	std::ofstream ofs = std::ofstream("./Log/log.txt");
	ofs.write(logText.c_str(), logText.length());
	ofs.close();
}
