function OnUseSelf(player, item)
	local giftpack = {item1 = {id=200000002,num=400}, item2 = {id=600000006,num=20},
	                  item3 = {id=200001001,num=40}, item4 = {id=300000501,num=15}}
	local reason="2012;800000098;0"
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