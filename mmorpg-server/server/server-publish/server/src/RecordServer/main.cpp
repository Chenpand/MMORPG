#include "RSApplication.h"

int main(int argc,char* argv[])
{
	RSApplication::Instance()->Init("RecordServer","RecordServer.cfg");
	RSApplication::Instance()->SetInterval(10);
	RSApplication::Instance()->Main(argc,argv);
	RSApplication::Destroy();
	return 0;
}
