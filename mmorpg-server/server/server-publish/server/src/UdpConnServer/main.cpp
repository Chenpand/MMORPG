#include "UCApplication.h"

int main(int argc, char* argv[])
{
	UCApplication::Instance()->Init("UdpConnServer", "UdpConnServer.cfg");
	UCApplication::Instance()->SetInterval(1);
	UCApplication::Instance()->Main(argc, argv);
	UCApplication::Instance()->Destroy();
}