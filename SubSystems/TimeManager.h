#pragma once

#define GAME_TIME_SCALE 0.01f


class TimeManager
{
public:
	static clock_t fixedDeltaTime;

	//--------------------Time----------------------------
	static clock_t gameTime;
	static clock_t simulationTime;
	static clock_t deltaTime;


	static void UpdateTime(clock_t newTime);

	static void FixedUpateSimTime();

	static clock_t GetDeltaTime();
};

