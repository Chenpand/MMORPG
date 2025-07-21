#include "USConsoleCmdHandler.h"

#include <json/writer.h>

#include "USConsoleAgent.h"
#include "UnionServiceMgr.h"
#include "GoldConsignmentService.h"

void SubmitOrderHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	UInt32 tradeType = 0;
	UInt32 orderType = 0;
	UInt32 unitPrice = 0;
	UInt32 tradeNum = 0;

	json::Object body;
	body["error"] = json::Boolean(true);

	try
	{
		tradeType = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["tradeType"]);
		orderType = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["orderType"]);
		unitPrice = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["unitPrice"]);
		tradeNum = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)jsonobj["tradeNum"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "SubmitOrderHandler failed, error msg:" << e.what() << "!" << LogStream::eos;

		body["msg"] = json::String("decode json failed!");
		USConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	auto service = UnionServiceMgr::Instance()->GetUnionService<GoldConsignmentService>();
	if (service == nullptr)
	{
		body["msg"] = json::String("not find gold consignment server! ");
		USConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	std::string retStr = service->GMSumbitOrder(tradeType, orderType, unitPrice, tradeNum);

	body["msg"] = json::String(retStr);
	body["error"] = json::Boolean(false);

	USConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}
