#pragma once



//Profile Bin
struct Bin
{
public:
	const char* name;

	//const char* callerName;

	clock_t timeTotal;

	int invokeCount;
};

//性能剖析器
class Profiler
{
private:
	
	static Bin * currentBin; //context
	static std::vector<Bin*> bins;

public :

	static Bin* StartSample(const char* name);

	static void StoreSample(const char* name, clock_t elapsedTime, Bin* bin);


	static void WriteProfileLog();
};

// Note: 使用传Bin对象以避免从所有Bin中查找





struct AutoProfile
{
	const char* name;

	Bin* bin;

	clock_t	 startTime;

	AutoProfile(const char* str);

	~AutoProfile();
};


//在要分析的语句块最前面调用此宏
///
#define PROFILE(name) AutoProfile p(name)