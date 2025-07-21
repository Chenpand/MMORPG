function OnUseSelf(player, item)
	local giftpack = {item1 = {id=600000008,num=1000},item2 = {id=600000007,num=1000000},
	item3 = {id=300000106,num=200},item4 = {id=300000105,num=200},
	item5 = {id=300001101,num=2000}
}

	local group = player:GetRewardGroup()
	for k,v in pairs(giftpack) do
		group:AddReward(v.id, v.num)
	end

	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end
	local reason="2012;800000074;0"
	player:ReduceItemNum(reason, item, 1)
	player:AddRewards(reason, group, true)
	return 0
end