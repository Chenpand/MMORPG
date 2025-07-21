
function OnUseSelf(player, item)
	local reason="2012;800000001;0"
	local bonusPool = {item1 = {id=600000007,num=100,wt=50}, item2 = {id=600000007,num=200,wt=35},
					item3 = {id=600000007,num=500,wt=10}, item4 = {id=600000007,num=1000,wt=5}}
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