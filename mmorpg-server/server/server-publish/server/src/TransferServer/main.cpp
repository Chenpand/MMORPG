#include "TFSApplication.h"

int main(int argc, char*argv[])
{
	TFSApplication::Instance()->Init("TransferServer", "TransferServer.cfg");
	TFSApplication::Instance()->SetInterval(1);
	TFSApplication::Instance()->Main(argc, argv);
	TFSApplication::Instance()->Destroy();

	return 0;
}