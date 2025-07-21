#include "DSApplication.h"

int main(int argc,char* argv[])
{
	DSApplication::Instance()->Init("DungeonServer", "DungeonServer.cfg");
	DSApplication::Instance()->SetInterval(10);
	DSApplication::Instance()->Main(argc, argv);
	DSApplication::Destroy();
	return 0;
}


