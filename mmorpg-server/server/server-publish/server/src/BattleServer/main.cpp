#include "BSApplication.h"

int main(int argc,char* argv[])
{
	BSApplication::Instance()->Init("BattleServer","BattleServer.cfg");
	BSApplication::Instance()->SetInterval(10);
	BSApplication::Instance()->Main(argc,argv);
	BSApplication::Destroy();
	return 0;
}


