
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm>

#include "Memory/Allocator.h"
#include "Profiler.h"


Bin* Profiler::currentBin;
std::vector<Bin*> Profiler::bins;

 Bin* Profiler::StartSample(const char* name)
{

	//Find Exist  (if not founded then construct one.) 
	Bin* binPtr = nullptr;
	for (std::vector<Bin*>::iterator iter = bins.begin(); iter != bins.end(); iter++)
	{
		if ((**iter).name == name)
		{
			binPtr = *iter;
			break;
		}
	}
	if (binPtr == nullptr)
	{
		Bin* newbin = NEW(Bin)Bin();
		newbin->name = name;
		newbin->invokeCount = 0;
		newbin->timeTotal = 0l;

		bins.push_back(newbin);
		binPtr = newbin;
	}

	//Enter Context( set current bin)
	currentBin = binPtr;



	//Set Value & Return
	return binPtr;
}

 void Profiler::StoreSample(const char* name, clock_t elapsedTime, Bin* bin)
{
	//restore
	bin->timeTotal += elapsedTime;
	bin->invokeCount += 1;
}


 void Profiler::WriteProfileLog()
{
	std::string output;

	for (std::vector<Bin*>::iterator iter = bins.begin(); iter != bins.end(); iter++)
	{
		Bin* bin = *iter;

		output.append(bin->name);
		output.append("\n---------");
		output.append("\ntimeTotal:");
		output.append(std::to_string(bin->timeTotal).c_str());
		output.append("\ninvokeCounts:");
		output.append(std::to_string(bin->invokeCount).c_str());
		output.append("\n\n");
	}


	std::ofstream ofs = std::ofstream("./Log/profilerLog.txt");
	ofs.write(output.c_str(), output.length());
	ofs.close();
}



 AutoProfile::AutoProfile(const char* str)
 {
	 name = str;

	 bin = Profiler::StartSample(name);

	 startTime = clock();
 }

 AutoProfile::~AutoProfile()
 {
	 clock_t endTime = clock();
	 clock_t elapsedTime = endTime - startTime;

	 Profiler::StoreSample(name, elapsedTime, bin);
 }