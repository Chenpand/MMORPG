--传承罐子（布甲）
function OnUseSelf(player, item)
	--奖池道具
	local bonusPool = {item1 = {id=124040001,num=1,wt=3000},item2 = {id=124041001,num=1,wt=3000},item3 = {id=124042001,num=1,wt=3000},item4 = {id=124043001,num=1,wt=3000},
	item5 = {id=124044001,num=1,wt=3000},item6 = {id=124540001,num=1,wt=3000},item7 = {id=124541001,num=1,wt=3000},item8 = {id=124742001,num=1,wt=3000},item9 = {id=124743001,num=1,wt=3000},
	item10 = {id=124544001,num=1,wt=3000},item11 = {id=125240001,num=1,wt=3000},item12 = {id=125241001,num=1,wt=3000},item13 = {id=125242001,num=1,wt=3000},item14 = {id=125243001,num=1,wt=3000},
	item15 = {id=125244001,num=1,wt=3000},item16 = {id=134240001,num=1,wt=200},item17 = {id=134241002,num=1,wt=200},item18 = {id=134242002,num=1,wt=200},item19 = {id=134243001,num=1,wt=200},
	item20 = {id=134244001,num=1,wt=200},item21 = {id=134240002,num=1,wt=50},item22 = {id=134241001,num=1,wt=50},item23 = {id=134242001,num=1,wt=50},item24 = {id=134243002,num=1,wt=50},
	item25 = {id=134244002,num=1,wt=50},item26 = {id=134740004,num=1,wt=30},item27 = {id=134741001,num=1,wt=30},item28 = {id=134742003,num=1,wt=30},item29 = {id=134743001,num=1,wt=30},
	item30 = {id=134744001,num=1,wt=30},item31 = {id=134740001,num=1,wt=100},item32 = {id=134740002,num=1,wt=100},item33 = {id=134741002,num=1,wt=100},item34 = {id=134741003,num=1,wt=100},
	item35 = {id=134742001,num=1,wt=100},item36 = {id=134742002,num=1,wt=100},item37 = {id=134743002,num=1,wt=100},item38 = {id=134743003,num=1,wt=100},item39 = {id=134744002,num=1,wt=100},
	item40 = {id=134744003,num=1,wt=100},item41 = {id=135440003,num=1,wt=10},item42 = {id=135441004,num=1,wt=10},item43 = {id=135442004,num=1,wt=10},item44 = {id=135443004,num=1,wt=10},
	item45 = {id=135444004,num=1,wt=10},item46 = {id=135440001,num=1,wt=100},item47 = {id=135440002,num=1,wt=100},item48 = {id=135441002,num=1,wt=100},item49 = {id=135441003,num=1,wt=100},
	item50 = {id=135442002,num=1,wt=100},item51 = {id=135442003,num=1,wt=100},item52 = {id=135443002,num=1,wt=100},item53 = {id=135443003,num=1,wt=100},item54 = {id=135444002,num=1,wt=100},
	item55 = {id=135444003,num=1,wt=100},item56 = {id=145440001,num=1,wt=3},item57 = {id=145441001,num=1,wt=10},item58 = {id=145442001,num=1,wt=10},item59 = {id=144943001,num=1,wt=36},item60 = {id=144944001,num=1,wt=36}}

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
	player:ReduceItemNum("2012;200000104;0", item, 1)

	return 0
end