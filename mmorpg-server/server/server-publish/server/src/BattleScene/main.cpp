#include "SSApplication.h"

int main(int argc,char* argv[])
{
	SSApplication::Instance()->Init("BattleScene","SceneServer.cfg");
	SSApplication::Instance()->SetInterval(35); //notedy by aprilliu, ������game loop���м����Ҳ������������ ��λ��ms
	SSApplication::Instance()->Main(argc,argv);
	SSApplication::Destroy();
	return 0;
}
