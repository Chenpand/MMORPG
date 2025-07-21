--魔盒
function OnUseSelf(player, item)
	--奖池道具
	local reason="2012;800000039;0"
	local bonusPool = {item1 = {id=200050023,num=6,wt=100},
item2 = {id=200050023,num=8,wt=100},
item3 = {id=200050023,num=10,wt=100}
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