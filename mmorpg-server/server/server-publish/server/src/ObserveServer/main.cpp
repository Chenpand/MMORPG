#include "OBSApplication.h"

int main(int argc, char* argv[])
{
	OBSApplication::Instance()->Init("ObserveServer", "ObserveServer.cfg");
	OBSApplication::Instance()->SetInterval(15);
	OBSApplication::Instance()->Main(argc, argv);
	OBSApplication::Destroy();
	return 0;
}
