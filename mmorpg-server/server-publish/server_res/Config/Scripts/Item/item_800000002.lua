function OnUseSelf(player, item)
	local reason="2012;800000002;0"
	local giftpack = {item1 = {id=500001001,num=1}, item2 = {id=500001002,num=1},
				   item3 = {id=500001003,num=1}, item4 = {id=500001004,num=1},
				   item5 = {id=500001005,num=1}, item6 = {id=500001006,num=1}}

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