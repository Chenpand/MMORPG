#ifndef __COMMON_NAMESPACE_DEFINITION_H__
#define __COMMON_NAMESPACE_DEFINITION_H__

//游戏平台名字空间结构
// namespace KingNet
// {
// 	namespace Server
// 	{
// 		//Logic Server
// 		namespace LogicServer
// 		{
// 
// 
// 		}
// 
// 		//Catalog Server
// 		namespace CatalogServer
// 		{
// 
// 		}
// 
// 		//Router Server
// 		namespace RouterServer
// 		{
// 
// 		}
// 
// 		//Game DB Server
// 		namespace GameDBServer
// 		{
// 
// 
// 		}
// 
// 	}
// 
// 
// }

#define KINGNET_BEGIN	namespace KingNet {
#define KINGNET_END		}

#define SERVER_BEGIN	KINGNET_BEGIN namespace Server{
#define SERVER_END		} KINGNET_END 

//Logic Server' namespace definition
#define LOGICSERVER_BEGIN	SERVER_BEGIN namespace LogicServer {
#define LOGICSERVER_END		} SERVER_END

//Catalog Server's namespace definition
#define CATALOGSERVER_BEGIN	SERVER_BEGIN namespace CatalogServer {
#define CATALOGSERVER_END	} SERVER_END

//Router Server's namespace definition
#define ROUTERSERVER_BEGIN	SERVER_BEGIN namespace RouterServer {
#define ROUTERSERVER_END	} SERVER_END

//Game DB Server's namespace definition
#define GAMEDBSERVER_BEGIN	SERVER_BEGIN namespace GameDBServer {
#define GAMEDBSERVER_END	} SERVER_END

//Connector Server's namespace definition
#define CONNECTOR_BEGIN		SERVER_BEGIN namespace Connector {
#define CONNECTOR_END		} SERVER_END


/*Using namespace directives*/
#define USING_NAMESPACE_LOGICSERVER	using namespace KingNet::LogicServer;
#define USING_NAMESPACE_CONNECTOR	using namespace KingNet::Connector;
#define USING_NAMESPACE_CATALOGSERVER	using namespace KingNet::CatalogServer;
#define USING_NAMESPACE_ROUTERSERVER	using namespace KingNet::RouterServer;
#define USING_NAMESPACE_GAMEDBSERVER	using namespace KingNet::GameDBServer;


#endif
