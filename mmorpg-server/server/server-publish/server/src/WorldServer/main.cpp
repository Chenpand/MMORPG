#include "WSApplication.h"

int main(int argc,char* argv[])
{
	WSApplication::Instance()->Init("WorldServer","WorldServer.cfg");
	WSApplication::Instance()->SetInterval(20);
	WSApplication::Instance()->Main(argc,argv);
	WSApplication::Destroy();
	return 0;
}
