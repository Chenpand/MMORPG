#include "TCSApplication.h"

int main(int argc,char* argv[])
{
	TCSApplication::Instance()->Init("TeamCopyServer", "TeamCopyServer.cfg");
	TCSApplication::Instance()->SetInterval(10);
	TCSApplication::Instance()->Main(argc, argv);
	TCSApplication::Destroy();
	return 0;
}


