#include "CSApplication.h"

int main(int argc,char* argv[])
{
	CSApplication::Instance()->Init("CrossServer","CrossServer.cfg");
	CSApplication::Instance()->SetInterval(10);
	CSApplication::Instance()->Main(argc,argv);
	CSApplication::Destroy();
	return 0;
}


