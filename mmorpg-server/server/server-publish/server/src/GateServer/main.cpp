#include "GSApplication.h"

int main(int argc,char* argv[])
{
	GSApplication::Instance()->Init("GateServer","GateServer.cfg"); //��ʼ��AppName�Լ���Ӧ�������ļ�
	GSApplication::Instance()->SetInterval(30);
	GSApplication::Instance()->Main(argc,argv); //���ȼ����˶�Ӧ�������ļ���Ϣ��Ȼ�����BeginProcessѭ��
	GSApplication::Destroy();
	return 0;
}
