#include "ASApplication.h"

int main(int argc,char* argv[])
{
	ASApplication::Instance()->Init("AdminServer","AdminServer.cfg");
	ASApplication::Instance()->SetInterval(5);
	ASApplication::Instance()->Main(argc,argv);
	ASApplication::Destroy();
	return 0;
}

