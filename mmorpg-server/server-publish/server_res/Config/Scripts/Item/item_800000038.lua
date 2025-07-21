--魔盒
function OnUseSelf(player, item)
	--奖池道具
	local reason="2012;800000038;0"
	local bonusPool = {item1 = {id=200050021,num=5,wt=5},
item2 = {id=200050021,num=8,wt=5},
item3 = {id=200050022,num=5,wt=10},
item4 = {id=200050022,num=6,wt=50},
item5 = {id=200050022,num=8,wt=50},
item6 = {id=200050023,num=3,wt=60},
item7 = {id=200050023,num=5,wt=16},
item8 = {id=200050023,num=6,wt=5}
}

	local totalWeight = 0
    for k, v in pairs(bonusPool) do
        totalWeight = totalWeight + v.wt
    end
	local randWt = Sys:RandBetween(1, totalWeight)

	
	local sumWt = 0
	local hitBonus = {}
	for k, v in pairs(bonusPool) do
		sumWt = v.wt + sumWt
		if randWt <= sumWt then
			hitBonus = v
			break	
		end
	end

	local group = player:GetRewardGroup()
	group:AddReward(hitBonus.id, hitBonus.num)
	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end
	player:ReduceItemNum(reason, item, 1)
	player:AddItem(reason, hitBonus.id, hitBonus.num, 0, 0, 1)
	
	return 0
end