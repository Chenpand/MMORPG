#include "VSApplication.h"

int main(int argc, char* argv[])
{
	VSApplication::Instance()->Init("VerifyServer", "VerifyServer.cfg");
	VSApplication::Instance()->SetInterval(35);
	VSApplication::Instance()->Main(argc, argv);
	VSApplication::Destroy();
	return 0;
}
