#include "MallHelper.h"

#include "CLSystemValueDataEntry.h"


namespace MallHelper
{
	UInt32 GetMallBuyGotItemDataId(MallBuyGotType type)
	{
		switch (type)
		{
		case MALL_BUY_GOT_TYPE_GNOME_COIN:
			return SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GNOME_COIN_ITEM_DATA_ID);

		default:
			break;
		}

		return 0;
	}
}