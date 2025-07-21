#include "LSApplication.h"

int main(int argc,char* argv[])
{
	LSApplication::Instance()->Init("DataLogServer","DataLogServer.cfg");
	LSApplication::Instance()->SetInterval(3);
	LSApplication::Instance()->Main(argc, argv);
	LSApplication::Destroy();
	return 0;
}
