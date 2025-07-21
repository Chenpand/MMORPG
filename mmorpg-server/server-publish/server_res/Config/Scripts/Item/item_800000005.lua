function OnUseSelf(player, item)
	local giftpack = {item1 = {id=111813001,num=1}, item2 = {id=121760001,num=1},
				   item3 = {id=121761001,num=1}, item4 = {id=121762001,num=1},
				   item5 = {id=121763001,num=1}, item6 = {id=121764001,num=1},
}

	local group = player:GetRewardGroup()
	for k,v in pairs(giftpack) do
		group:AddReward(v.id, v.num)
	end

	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end
	player:ReduceItemNum("useItem", item, 1)
	player:AddRewards("useItem", group, true)
	return 0
end