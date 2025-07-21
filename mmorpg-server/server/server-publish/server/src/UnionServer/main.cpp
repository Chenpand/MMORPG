#include "USApplication.h"

int main(int argc,char* argv[])
{
	USApplication::Instance()->Init("UnionServer", "UnionServer.cfg");
	USApplication::Instance()->SetInterval(10);
	USApplication::Instance()->Main(argc, argv);
	USApplication::Destroy();
	return 0;
}


