#include "DPApplication.h"

int main(int argc,char* argv[])
{
	Application::Instance()->Init("DBProxyServer","DBProxyServer.cfg");
	Application::Instance()->SetInterval(5);
	Application::Instance()->Main(argc,argv);
	Application::Destroy();
	return 0;
}
