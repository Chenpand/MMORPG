--传承罐子（皮甲）
function OnUseSelf(player, item)
	--奖池道具
	local bonusPool = {item1 = {id=124050001,num=1,wt=3000},item2 = {id=124051001,num=1,wt=3000},item3 = {id=124052001,num=1,wt=3000},item4 = {id=124053001,num=1,wt=3000},
	item5 = {id=124054001,num=1,wt=3000},item6 = {id=124550001,num=1,wt=3000},item7 = {id=124551001,num=1,wt=3000},item8 = {id=124752001,num=1,wt=3000},item9 = {id=124753001,num=1,wt=3000},
	item10 = {id=124754001,num=1,wt=3000},item11 = {id=125250001,num=1,wt=3000},item12 = {id=125251001,num=1,wt=3000},item13 = {id=125252001,num=1,wt=3000},item14 = {id=125253001,num=1,wt=3000},
	item15 = {id=125254001,num=1,wt=3000},item16 = {id=134250001,num=1,wt=200},item17 = {id=134251002,num=1,wt=200},item18 = {id=134252001,num=1,wt=200},item19 = {id=134253002,num=1,wt=200},
	item20 = {id=134254002,num=1,wt=200},item21 = {id=134250002,num=1,wt=50},item22 = {id=134251001,num=1,wt=50},item23 = {id=134252002,num=1,wt=50},item24 = {id=134253001,num=1,wt=50},
	item25 = {id=134254001,num=1,wt=50},item26 = {id=134750001,num=1,wt=30},item27 = {id=134751001,num=1,wt=30},item28 = {id=134752001,num=1,wt=30},item29 = {id=134753002,num=1,wt=30},
	item30 = {id=134754002,num=1,wt=30},item31 = {id=134750002,num=1,wt=200},item32 = {id=134751002,num=1,wt=200},item33 = {id=134752002,num=1,wt=200},item34 = {id=134753001,num=1,wt=200},
	item35 = {id=134754001,num=1,wt=200},item36 = {id=135450002,num=1,wt=10},item37 = {id=135451002,num=1,wt=10},item38 = {id=135452002,num=1,wt=10},item39 = {id=135453002,num=1,wt=10},
	item40 = {id=135454002,num=1,wt=10},item41 = {id=135450001,num=1,wt=200},item42 = {id=135451001,num=1,wt=200},item43 = {id=135452001,num=1,wt=200},item44 = {id=135453001,num=1,wt=200},
	item45 = {id=135454001,num=1,wt=200},item46 = {id=145450001,num=1,wt=3},item47 = {id=145451001,num=1,wt=10},item48 = {id=145452001,num=1,wt=10},item49 = {id=144953001,num=1,wt=36},
	item50 = {id=144954001,num=1,wt=36}}

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

    local group = player:GetRewardGroup()
    group:AddReward(hitBonus.id, hitBonus.num)
    local canAdd = player:CheckAddRewards(group, item, 1)
    if canAdd ~= 0 then return canAdd end

    player:ReduceItemNum("2012;200000107;0", item, 1)    --删除道具
	player:SendMagicJarResult(bonusIds, hitBonus.id, hitBonus.num)

	return 0
end