--开服礼盒
function OnUseSelf(player, item)
	--奖池道具
	local bonusPool = {item1 = {id=130193016,num=1,wt=50},item2 = {id=152443002,num=1,wt=5},item3 = {id=152453001,num=1,wt=5},item4 = {id=152473001,num=1,wt=5},
	item5 = {id=152473002,num=1,wt=5},item6 = {id=500003001,num=1,wt=50},item7 = {id=500003002,num=1,wt=0},item8 = {id=500003003,num=1,wt=0},item9 = {id=500003004,num=1,wt=0},
	item10 = {id=500003005,num=1,wt=0},item11 = {id=300000106,num=20,wt=200},item12 = {id=300000105,num=10,wt=200},item13 = {id=600000006,num=1,wt=200},item14 = {id=600000006,num=2,wt=200},
	item15 = {id=200002003,num=1,wt=50},item16 = {id=200005003,num=1,wt=30},item17 = {id=200006003,num=1,wt=30},item18 = {id=200004003,num=1,wt=30},item19 = {id=200003003,num=1,wt=30},
	item20 = {id=200005002,num=1,wt=200},item21 = {id=200006002,num=1,wt=200},item22 = {id=200004002,num=1,wt=200},item23 = {id=200003002,num=1,wt=200},item24 = {id=200001002,num=3,wt=200},
	item25 = {id=200001003,num=5,wt=200}}

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
	player:ReduceItemNum("2012;200000201;0", item, 1)

	return 0
end