#pragma once


class Debug
{
private:
	static std::string logText;
public:

	static void Log(std::string str);

	static void Log(const char* str);

	static void WriteLogFile();
};

