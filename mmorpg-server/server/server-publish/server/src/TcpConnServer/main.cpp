#include "TCApplication.h"

int main(int argc, char* argv[])
{
	TCApplication::Instance()->Init("TcpConnServer", "TcpConnServer.cfg");
	TCApplication::Instance()->SetInterval(1);
	TCApplication::Instance()->Main(argc, argv);
	TCApplication::Instance()->Destroy();
}
