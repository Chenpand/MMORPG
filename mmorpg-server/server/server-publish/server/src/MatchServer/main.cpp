#include "MSApplication.h"

int main(int argc, char* argv[])
{
	MSApplication::Instance()->Init("MatchServer", "MatchServer.cfg");
	MSApplication::Instance()->SetInterval(15);
	MSApplication::Instance()->Main(argc, argv);
	MSApplication::Destroy();
	return 0;
}
