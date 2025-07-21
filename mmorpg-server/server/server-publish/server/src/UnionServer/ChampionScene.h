#ifndef __CHAMPION_SCENE_H__
#define __CHAMPION_SCENE_H__

#include "UScene.h"
#include "CLChampionDefine.h"

class ChampionPlayer;

class ChampionScene : public UScene 
{
public:
	void StartBattle(ChampionStatus status);
	std::vector<ChampionPlayer*> GetTopScorePlayer(UInt32 top);
};

#endif


