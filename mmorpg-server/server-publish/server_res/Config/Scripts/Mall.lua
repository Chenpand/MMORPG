--道具格式:商城ID,道具ID,一组数量,原价,折扣价,货币类型,是否限量,每人限量数量
--货币类型:点券 18, 绑定点券 28

--时装头部

------------------------热门推荐-----------------------------------------
local HOT_FAVORATE_GIFT = {
"10001,800000091,1,30,30,18,1,100,0",
"10002,800000092,1,30,60,18,1,200,0",
"10020,200050024,1,30,30,18,1,100,0",
"10017,130193017,1,600,300,18,1,1,0",
"10014,130392002,1,80,60,18,1,1,0",
"10015,130391001,1,80,60,18,1,1,0",
"10016,130390001,1,80,60,18,1,1,0"
}

------------------------道具-----------------------------------------
local ITEM_GIFT = {
}

local ITEM_REVIVAL = {
"10003,800000035,1,30,30,28,1,5,0",
"10018,300000106,1,30,1,28,1,100,0",
"10019,300000105,1,30,2,28,1,100,0",
"10001,800000091,1,30,30,18,1,100,0",
"10002,800000092,1,30,60,18,1,200,0",
"10020,200050024,1,30,30,18,1,100,0",
"10010,300000107,1,30,10,18,0,0,0",
"10011,200110002,1,20,10,18,0,0,0",
"10012,200110001,1,80,50,18,0,0,0",
"10013,200000005,1,200,50,18,0,0,0"
}

local ITEM_POTION = {
"10004,600000006,1,30,20,28,0,0,0",
"10005,200001001,1,15,10,28,0,0,0",
"10006,200003001,1,15,10,28,0,0,0",
"10007,200004001,1,15,10,28,0,0,0",
"10008,200005001,1,15,10,28,0,0,0",
"10009,200006001,1,15,10,28,0,0,0"
}

local ITEM_FUNCTION = {
"10014,130392002,1,80,60,18,1,1,0",
"10015,130391001,1,80,60,18,1,1,0",
"10016,130390001,1,80,60,18,1,1,0",
"10017,130193017,1,600,300,18,1,1,0"
}

------------------------时装-----------------------------------------
local FASHION_GIFT = {
}

local FASHION_HEAD50 = {
"10100,535003001,1,300,400,18,0,0,0",
"10120,535002001,1,500,400,18,0,0,0"
}

local FASHION_HEAD10 = {
"10105,531003001,1,300,400,18,0,0,0",
"10125,531002001,1,500,400,18,0,0,0"
}

local FASHION_HEAD30 = {
"10110,533003001,1,300,400,18,0,0,0",
"10130,533002001,1,500,400,18,0,0,0"
}

local FASHION_HEAD20 = {
"10115,532003001,1,300,400,18,0,0,0",
"10135,532002001,1,500,400,18,0,0,0"
}

--local FASHION_HAIR = {
--}

local FASHION_WAER50 = {
"10101,535003002,1,300,700,18,0,0,0",
"10121,535002002,1,500,700,18,0,0,0"
}

local FASHION_WAER10 = {
"10106,531003002,1,300,700,18,0,0,0",
"10126,531002002,1,500,700,18,0,0,0"
}

local FASHION_WAER30 = {
"10111,533003002,1,300,700,18,0,0,0",
"10131,533002002,1,500,700,18,0,0,0"
}

local FASHION_WAER20 = {
"10116,532003002,1,300,700,18,0,0,0",
"10136,532002002,1,500,700,18,0,0,0"
}

local FASHION_CHEST50 = {
"10102,535003003,1,300,240,18,0,0,0",
"10122,535002003,1,500,240,18,0,0,0"
}

local FASHION_CHEST10 = {
"10107,531003003,1,300,240,18,0,0,0",
"10127,531002003,1,500,240,18,0,0,0"

}

local FASHION_CHEST30 = {
"10112,533003003,1,300,240,18,0,0,0",
"10132,533002003,1,500,240,18,0,0,0"
}

local FASHION_CHEST20 = {
"10117,532003003,1,300,240,18,0,0,0",
"10137,532002003,1,500,240,18,0,0,0"
}

local FASHION_PANT50 = {
"10103,535003004,1,300,500,18,0,0,0",
"10123,535002004,1,500,500,18,0,0,0"
}

local FASHION_PANT10 = {
"10108,531003004,1,300,500,18,0,0,0",
"11118,531002004,1,500,500,18,0,0,0"
}

local FASHION_PANT30 = {
"10113,533003004,1,300,500,18,0,0,0",
"10133,533002004,1,500,500,18,0,0,0"
}

local FASHION_PANT20 = {
"10118,532003004,1,300,500,18,0,0,0",
"10138,532002004,1,500,500,18,0,0,0"
}

local FASHION_BELT50 = {
"10104,535003005,1,300,240,18,0,0,0",
"10124,535002005,1,500,240,18,0,0,0"
}

local FASHION_BELT10 = {
"10109,531003005,1,300,240,18,0,0,0",
"10129,531002005,1,500,240,18,0,0,0"
}

local FASHION_BELT30 = {
"10114,533003005,1,300,240,18,0,0,0",
"10134,533002005,1,500,240,18,0,0,0"
}

local FASHION_BELT20 = {
"10119,532003005,1,300,240,18,0,0,0",
"10139,532002005,1,500,240,18,0,0,0"
}

------------------------金币-----------------------------------------
local GOLD_ITEM = {
"10021,600000001,3000,20,10,18,1,50,0",
"10022,600000001,30000,200,100,18,1,50,0",
"10023,600000001,300000,2000,1000,18,1,50,0",
"10024,600000007,3000,20,10,28,1,50,0",
"10025,600000007,30000,200,100,28,1,50,0",
"10026,600000007,300000,2000,1000,28,1,50,0"
}


	
------------------------礼包详细-----------------------------------------
--格式:礼包id="道具id,数量|道具id,数量|...|道具id,数量"
local GIFT_DETAIL = {
}


--初始化商城限量道具
--AddMallItem(主页签编号，子页签编号，职业编号，商品表)
function Init(script)	
	for i = 1, table.getn(HOT_FAVORATE_GIFT) do
		script:AddMallItem(1, 11, 0, HOT_FAVORATE_GIFT[i]);
	end

	for i = 1, table.getn(ITEM_GIFT) do
		script:AddMallItem(3, 1, 0, ITEM_GIFT[i]);
	end

	for i = 1, table.getn(ITEM_REVIVAL) do
		script:AddMallItem(3, 8, 0, ITEM_REVIVAL[i]);
	end

	for i = 1, table.getn(ITEM_POTION) do
		script:AddMallItem(3, 9, 0, ITEM_POTION[i]);
	end

	for i = 1, table.getn(ITEM_FUNCTION) do
		script:AddMallItem(3, 10, 0, ITEM_FUNCTION[i]);
	end

	for i = 1, table.getn(FASHION_GIFT) do
		script:AddMallItem(2, 1, 0, FASHION_GIFT[i]);
	end

	for i = 1, table.getn(FASHION_HEAD50) do
		script:AddMallItem(2, 2, 50, FASHION_HEAD50[i]);
	end
	
	for i = 1, table.getn(FASHION_HEAD10) do
		script:AddMallItem(2, 2, 10, FASHION_HEAD10[i]);
	end
	
	for i = 1, table.getn(FASHION_HEAD30) do
		script:AddMallItem(2, 2, 30, FASHION_HEAD30[i]);
	end
	
	for i = 1, table.getn(FASHION_HEAD20) do
		script:AddMallItem(2, 2, 20, FASHION_HEAD20[i]);
	end

--	for i = 1, table.getn(FASHION_HAIR) do
--		script:AddMallItem(2, 3, 10, FASHION_HAIR[i]);
--	end

	for i = 1, table.getn(FASHION_WAER50) do
		script:AddMallItem(2, 3, 50, FASHION_WAER50[i]);
	end
	
	for i = 1, table.getn(FASHION_WAER10) do
		script:AddMallItem(2, 3, 10, FASHION_WAER10[i]);
	end
	
	for i = 1, table.getn(FASHION_WAER30) do
		script:AddMallItem(2, 3, 30, FASHION_WAER30[i]);
	end
	
	for i = 1, table.getn(FASHION_WAER20) do
		script:AddMallItem(2, 3, 20, FASHION_WAER20[i]);
	end

	for i = 1, table.getn(FASHION_CHEST50) do
		script:AddMallItem(2, 4, 50, FASHION_CHEST50[i]);
	end
	for i = 1, table.getn(FASHION_CHEST10) do
		script:AddMallItem(2, 4, 10, FASHION_CHEST10[i]);
	end
	for i = 1, table.getn(FASHION_CHEST30) do
		script:AddMallItem(2, 4, 30, FASHION_CHEST30[i]);
	end
	for i = 1, table.getn(FASHION_CHEST20) do
		script:AddMallItem(2, 4, 20, FASHION_CHEST20[i]);
	end

	for i = 1, table.getn(FASHION_PANT50) do
		script:AddMallItem(2, 5, 50, FASHION_PANT50[i]);
	end
	
	for i = 1, table.getn(FASHION_PANT10) do
		script:AddMallItem(2, 5, 10, FASHION_PANT10[i]);
	end
	
	for i = 1, table.getn(FASHION_PANT30) do
		script:AddMallItem(2, 5, 30, FASHION_PANT30[i]);
	end
	
	for i = 1, table.getn(FASHION_PANT20) do
		script:AddMallItem(2, 5, 20, FASHION_PANT20[i]);
	end

  for i = 1, table.getn(FASHION_BELT50) do
      script:AddMallItem(2, 6, 50, FASHION_BELT50[i]);
  end

  for i = 1, table.getn(FASHION_BELT10) do
      script:AddMallItem(2, 6, 10, FASHION_BELT10[i]);
  end
  for i = 1, table.getn(FASHION_BELT30) do
      script:AddMallItem(2, 6, 30, FASHION_BELT30[i]);
  end

  for i = 1, table.getn(FASHION_BELT20) do
      script:AddMallItem(2, 6, 20, FASHION_BELT20[i]);
  end
  
	for i = 1, table.getn(GOLD_ITEM) do
			script:AddMallItem(4, 1, 0, GOLD_ITEM[i]);
	end
end

function GetGiftDetail(giftId)
	local giftItems = GIFT_DETAIL[giftId]

	if giftItems == nil then
		return ""
	else
		return giftItems
	end
end
