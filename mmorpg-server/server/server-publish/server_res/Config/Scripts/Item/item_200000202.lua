--魔盒
function OnUseSelf(player, item)
	--奖池道具
	local bonusPool = {item1 = {id=200000001,num=1,wt=1000},item2 = {id=200000002,num=1,wt=1000},item3 = {id=155610004,num=1,wt=1},item4 = {id=154471001,num=1,wt=1},
	item5 = {id=154451001,num=1,wt=1},item6 = {id=300000501,num=1,wt=100},item7 = {id=200000310,num=1,wt=100},item8 = {id=300000106,num=200,wt=300},item9 = {id=300000105,num=100,wt=300},
	item10 = {id=200110002,num=1,wt=100},item11 = {id=200050004,num=5,wt=100},item12 = {id=200050008,num=5,wt=100},item13 = {id=200050012,num=5,wt=100},item14 = {id=200050016,num=5,wt=100},
	item15 = {id=200050020,num=5,wt=100},item16 = {id=600000006,num=1,wt=300},item17 = {id=600000006,num=2,wt=300},item18 = {id=200002003,num=1,wt=60},item19 = {id=200005003,num=1,wt=60},
	item20 = {id=200006003,num=1,wt=60},item21 = {id=200004003,num=1,wt=60},item22 = {id=200003003,num=1,wt=60},item23 = {id=200005002,num=3,wt=500},item24 = {id=200006002,num=3,wt=500},
	item25 = {id=200004002,num=3,wt=500},item26 = {id=200003002,num=3,wt=500},item27 = {id=200001002,num=3,wt=500},item28 = {id=200001003,num=5,wt=500}}

	local totalWeight = 0
	for k, v in pairs(bonusPool) do
		totalWeight = totalWeight + v.wt
	end

	local randWt = Sys:RandBetween(1, totalWeight)

	--随机出抽到的奖励道具
	local sumWt = 0
	local hitBonus = {}
	for k, v in pairs(bonusPool) do
		sumWt = v.wt + sumWt
		if randWt <= sumWt then
			hitBonus = v
			break	
		end
	end

	local bonusIds = ""
	for k, v in pairs(bonusPool) do
		if bonusIds == "" then
			bonusIds = v.id
		else
			bonusIds = bonusIds.."|"..v.id
		end
	end

	player:SendMagicJarResult(bonusIds, hitBonus.id, hitBonus.num)
	player:ReduceItemNum("2012;200000202;0", item, 1)

	return 0
end