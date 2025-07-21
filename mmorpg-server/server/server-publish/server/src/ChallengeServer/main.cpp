#include "CSApplication.h"

int main(int argc,char* argv[])
{
	CSApplication::Instance()->Init("ChallengeServer","ChallengeServer.cfg");
	CSApplication::Instance()->SetInterval(10);
	CSApplication::Instance()->Main(argc,argv);
	CSApplication::Destroy();
	return 0;
}


