#include "SSApplication.h"

int main(int argc,char* argv[])
{
	SSApplication::Instance()->Init("BattleScene","SceneServer.cfg");
	SSApplication::Instance()->SetInterval(35); //notedy by aprilliu, ，设置game loop运行间隔，也就是运行周期 单位：ms
	SSApplication::Instance()->Main(argc,argv);
	SSApplication::Destroy();
	return 0;
}
