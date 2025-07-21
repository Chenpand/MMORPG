#include "RLApplication.h"

int main(int argc, char*argv[])
{

	RLApplication::Instance()->Init("RelayServer", "RelayServer.cfg");
	RLApplication::Instance()->SetInterval(1);
	RLApplication::Instance()->Main(argc, argv);
	RLApplication::Instance()->Destroy();

	return 0;
}