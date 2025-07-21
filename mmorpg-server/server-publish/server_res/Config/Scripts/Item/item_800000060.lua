function OnUseSelf(player, item)
	local giftpack = {item1 = {id=600000007,num=1000}, item2 = {id=300000106,num=50}}
	local reason="2012;800000060;0"
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