#include "GSApplication.h"

int main(int argc,char* argv[])
{
	GSApplication::Instance()->Init("GateServer","GateServer.cfg"); //初始化AppName以及对应的配置文件
	GSApplication::Instance()->SetInterval(30);
	GSApplication::Instance()->Main(argc,argv); //首先加载了对应的配置文件信息，然后进入BeginProcess循环
	GSApplication::Destroy();
	return 0;
}
