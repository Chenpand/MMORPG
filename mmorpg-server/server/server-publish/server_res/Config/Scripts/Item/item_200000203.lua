--魔盒
function OnUseSelf(player, item)
	--奖池道具
	local bonusPool = {item1 = {id=200000001,num=1,wt=1400},item2 = {id=200000002,num=1,wt=800},item3 = {id=200000310,num=1,wt=60},item4 = {id=155290002,num=1,wt=1},
	item5 = {id=155291002,num=1,wt=1},item6 = {id=500001001,num=1,wt=15},item7 = {id=500001002,num=1,wt=15},item8 = {id=500001003,num=1,wt=15},item9 = {id=500001004,num=1,wt=15},
	item10 = {id=500001005,num=1,wt=15},item11 = {id=500001006,num=1,wt=15},item12 = {id=500002001,num=1,wt=30},item13 = {id=500002002,num=1,wt=30},item14 = {id=500002003,num=1,wt=30},
	item15 = {id=500002004,num=1,wt=30},item16 = {id=500002005,num=1,wt=30},item17 = {id=300000106,num=200,wt=300},item18 = {id=300000105,num=100,wt=300},item19 = {id=200110002,num=1,wt=100},
	item20 = {id=200110002,num=3,wt=100},item21 = {id=200110001,num=1,wt=100},item22 = {id=200110001,num=3,wt=100},item23 = {id=200050004,num=5,wt=40},item24 = {id=200050008,num=5,wt=40},
	item25 = {id=200050012,num=5,wt=40},item26 = {id=200050016,num=5,wt=40},item27 = {id=200050020,num=5,wt=40},item28 = {id=600000006,num=1,wt=300},item29 = {id=600000006,num=2,wt=300},
	item30 = {id=200002003,num=1,wt=60},item31 = {id=200005003,num=1,wt=60},item32 = {id=200006003,num=1,wt=60},item33 = {id=200004003,num=1,wt=60},item34 = {id=200003003,num=1,wt=60},
	item35 = {id=200005002,num=1,wt=500},item36 = {id=200006002,num=1,wt=500},item37 = {id=200004002,num=1,wt=500},item38 = {id=200003002,num=1,wt=500},item39 = {id=200001002,num=3,wt=500},
	item40 = {id=200001002,num=5,wt=500}}

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
	player:ReduceItemNum("2012;200000203;0", item, 1)

	return 0
end