----------------------------------装备强化配置start------------------------------------------------------------------
--武器20个强化系数
local WpStrenthModulus = {1,2,3,4,6,8,10,12,14,17,
						33,50,67,108,150,192,267,342,
						417,568}

--1.白 2.蓝 3.紫 4.绿 5.粉 6.黄
--武器品质系数A
local WpColorQaModulus = {8,8,10,11,12,14}
--武器品质系数B
local WpColorQbModulus = {40,70,100,110,125,145}

--1.巨剑 2.太刀 3.短剑 4.光剑 5.钝器
--6.左轮 7.弩 8.手炮 9.自动步枪 10.自动手枪
--11.法杖 12.魔杖 13.矛 14.棍棒
--各部位系数
local WpPartPhyModulus = {{st=1,val=120}, {st=2,val=95}, {st=3,val=95}, 
						  {st=4,val=90}, {st=5,val=110},{st=6,val=87},
						   {st=7,val=75}, {st=8,val=106}, {st=9,val=100}, {st=10,val=60},
						  {st=11,val=95}, {st=12,val=90}, {st=13,val=120}, {st=14,val=110}}

local WpPartMagModulus = {{st=1,val=95}, {st=2,val=105}, {st=3,val=115}, 
						  {st=4,val=100}, {st=5,val=95},{st=6,val=77},
						   {st=7,val=85}, {st=8,val=64}, {st=9,val=85}, {st=10,val=94},
						  {st=11,val=120}, {st=12,val=110}, {st=13,val=95}, {st=14,val=90}}

--防具20个强化系数
local ArmStrenthModulus = {20,40,60,80,120,160,200,240,280,340,
						660,1000,1340,2160,3000,3840,5340,6840,
						8340,11360}
--防具品质系数B
local ArmColorQbModulus = {40,70,100,110,125,145}

--首饰20个强化系数
local JewStrenthModulus = {30,70,100,130,200,270,330,400,470,570,
						1100,1670,2230,3600,5000,6400,8900,11400,
						13900,18930}
--首饰品质系数B
local JewColorQbModulus = {40,70,100,110,125,145}


--得到武器类别物攻系数
function GetWpPhyMod(subType)
	for k,v in pairs(WpPartPhyModulus) do
		if v.st == subType then
			return v.val
		end
	end

	return 0
end

--得到武器类别法攻系数
function GetWpMagMod(subType)
	for k,v in pairs(WpPartMagModulus) do
		if v.st == subType then
			return v.val
		end
	end

	return 0
end

--得到武器品质QA系数
function GetWpClQaMod(color)
	return WpColorQaModulus[color]
end

--得到武器品质QB系数
function GetWpClQbMod(color)
	return WpColorQbModulus[color]
end

--得到武器强化系数
function GetWpStrMod(strLv)
	return WpStrenthModulus[strLv]
end

--得到防具强化系数
function GetArmStrMod(strLv)
	return ArmStrenthModulus[strLv]
end

--得到防具品质QB系数
function GetArmClQbMod(color)
	return ArmColorQbModulus[color]
end

--得到首饰强化系数
function GetJewStrMod(strLv)
	return JewStrenthModulus[strLv]
end

--得到首饰品质QB系数
function GetJewClQbMod(color)
	return JewColorQbModulus[color]
end

----------------------------------装备强化配置end------------------------------------------------------------------

----------------------------------装备封装start------------------------------------------------------------------
--minLv:装备最小等级，maxLv:装备最大等级，purple:紫装需要蜜蜡数, pink:粉装需要蜜蜡数
local needMatNum = {{minLv=1, maxLv=9, purple=3, green=8, pink=8, yellow=8},
				 {minLv=10, maxLv=19, purple=6, green=8, pink=8, yellow=12},
				 {minLv=20, maxLv=29, purple=9, green=12, pink=12, yellow=18},
				 {minLv=30, maxLv=39, purple=12, green=16, pink=16, yellow=24},
				 {minLv=40, maxLv=60, purple=15, green=20, pink=20, yellow=24}}
				 
--count:再次封装次数，addNum:额外需要蜜蜡数
local count2MatNum = {{count=1, addNum=1},
					  {count=2, addNum=2},
					  {count=3, addNum=3},
					  {count=4, addNum=4},
				      {count=5, addNum=5}}

--获取封装需要的蜜蜡数
function GetNeedSealMat(lv, color, sealCount)
	if color < 3 or color > 5 then
		return 0
	end

	local needNum = 0
	for k,v in pairs(needMatNum) do
		if v.minLv <= lv and v.maxLv >= lv then
			-- 紫装和绿装
			if color == 3 then
				needNum = needNum + v.purple
			elseif color == 4 then
				needNum = needNum + v.green
			elseif color == 5 then
				needNum = needNum + v.pink
			end

			break
		end
	end

	for k,v in pairs(count2MatNum) do
		if v.count == sealCount then
			needNum = needNum + v.addNum
			break
		end
	end

	return needNum
end