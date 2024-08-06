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

//����������
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

// Note: ʹ�ô�Bin�����Ա��������Bin�в���





struct AutoProfile
{
	const char* name;

	Bin* bin;

	clock_t	 startTime;

	AutoProfile(const char* str);

	~AutoProfile();
};


//��Ҫ������������ǰ����ô˺�
///
#define PROFILE(name) AutoProfile p(name)