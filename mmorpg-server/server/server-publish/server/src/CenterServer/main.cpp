#include "CSApplication.h"

int main(int argc,char* argv[])
{
	CSApplication::Instance()->Init("CenterServer","CenterServer.cfg");
	CSApplication::Instance()->SetInterval(1);
	CSApplication::Instance()->Main(argc,argv);
	CSApplication::Destroy();
	return 0;
}
