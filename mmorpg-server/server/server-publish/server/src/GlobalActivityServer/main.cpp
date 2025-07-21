#include "GASApplication.h"

int main(int argc,char* argv[])
{
	GASApplication::Instance()->Init("GlobalActivityServer","GlobalActivityServer.cfg");
	GASApplication::Instance()->SetInterval(10);
	GASApplication::Instance()->Main(argc,argv);
	GASApplication::Destroy();
	return 0;
}


