#include "EqualPvPMatchSercive.h"


EqualPvPMatchSercive::EqualPvPMatchSercive(bool useRobot) : PvPMatchService(useRobot)
{
	m_type = MST_EQUAL_PK;
}


EqualPvPMatchSercive::~EqualPvPMatchSercive()
{
}

EqualPvPPracticeMatchService::EqualPvPPracticeMatchService() : PracticeMatchService()
{
	m_type = MST_EQUAL_PK_PRACTICE;
}

EqualPvPPracticeMatchService::~EqualPvPPracticeMatchService()
{

}
