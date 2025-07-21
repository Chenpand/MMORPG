function OnUseSelf(player, item)
	local giftpack = {item1 = {id=200000002,num=600}, item2 = {id=600000006,num=30},
	                  item3 = {id=200001001,num=60}, item4 = {id=360000002,num=25}}
	local reason="2012;800000099;0"
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