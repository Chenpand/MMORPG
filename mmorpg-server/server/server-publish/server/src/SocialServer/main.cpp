#include "OSApplication.h"

int main(int argc, char* argv[])
{
	OSApplication::Instance()->Init("SocialServer","SocialServer.cfg");
	OSApplication::Instance()->SetInterval(15);
	OSApplication::Instance()->Main(argc,argv);
	OSApplication::Destroy();
	return 0;
}
