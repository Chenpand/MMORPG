#include "VCApplication.h"

int main(int argc, char* argv[])
{
	VCApplication::Instance()->Init("VerifyCenterServer", "VerifyCenterServer.cfg");
	VCApplication::Instance()->SetInterval(35);
	VCApplication::Instance()->Main(argc, argv);
	VCApplication::Destroy();
	return 0;
}
