function OnUseSelf(player, item)
	local giftpack = {item1 = {id=600000008,num=5000},item2 = {id=200000001,num=400},
	item3 = {id=200000002,num=400},item4 = {id=600000007,num=5000000},
	item5 = {id=300000106,num=2000},item6 = {id=300000105,num=2000},
	item7 = {id=200000310,num=5},item8 = {id=300001101,num=2000},
	item9 = {id=200050023,num=50}
}

	local group = player:GetRewardGroup()
	for k,v in pairs(giftpack) do
		group:AddReward(v.id, v.num)
	end

	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end
	local reason="2012;800000079;0"
	player:ReduceItemNum(reason, item, 1)
	player:AddRewards(reason, group, true)
	return 0
end