#include "Application.h"

int main(int argc,char* argv[])
{
	Application::Instance()->Init("SortlistServer","SortlistServer.cfg");
	Application::Instance()->SetInterval(10);
	Application::Instance()->Main(argc,argv);
	Application::Destroy();
	return 0;
}
