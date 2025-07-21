function OnUseSelf(player, item)
	local giftpack = {item1 = {id=200000312,num=1}, item2 = {id=600000007,num=1000000},
	                  item3 = {id=300000106,num=1000}, item4 = {id=300000105,num=500}}
	local reason="2012;800000100;0"
	local group = player:GetRewardGroup()
	for k,v in pairs(giftpack) do
		group:AddReward(v.id, v.num)
	end

	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end
	player:ReduceItemNum(reason, item, 1)
	player:AddRewards(reason, group, true)
	return 0
end