function OnUseSelf(player, item)
	local giftpack = {item1 = {id=600000008,num=200}, item2 = {id=200000001,num=20}, item3 = {id=330000001,num=3},
					 item4 = {id=300000501,num=1}, item5 = {id=600000006,num=5}, item6 = {id=200000002,num=20}
}

	local group = player:GetRewardGroup()
	for k,v in pairs(giftpack) do
		group:AddReward(v.id, v.num)
	end

	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end
	local reason="2012;800000072;0"
	player:ReduceItemNum(reason, item, 1)
	player:AddRewards(reason, group, true)
	return 0
end