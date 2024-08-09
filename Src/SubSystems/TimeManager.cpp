


#include <ctime>

#include "TimeManager.h"



clock_t TimeManager::fixedDeltaTime = 20L;


clock_t TimeManager::gameTime = 0L;
clock_t TimeManager::simulationTime = 0L;
clock_t TimeManager::deltaTime = 0L;

void TimeManager::UpdateTime(clock_t newTime)
{
	deltaTime = newTime - gameTime;
	gameTime = newTime;
}

void TimeManager::FixedUpateSimTime()
{
	simulationTime += fixedDeltaTime;
}

clock_t TimeManager::GetDeltaTime()
{
	return deltaTime;
}
