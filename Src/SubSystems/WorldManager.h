#pragma once
class WorldManager
{
public:
	static WorldManager& Instance();

	//------------------Scene----------------------------
public:
	World currentWorld;
	//---------------------------------------------------
};

