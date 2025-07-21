#include "RSApplication.h"

int main(int argc, char* argv[])
{
	RSApplication::Instance()->Init("ReplayServer", "ReplayServer.cfg");
	RSApplication::Instance()->SetInterval(50);
	RSApplication::Instance()->Main(argc, argv);
	RSApplication::Destroy();
	return 0;
}
