function OnDecompose(player, item)
	local getMats = {item1 = {id=300001101,num=200}}
	local group = player:GetRewardGroupNoClear()
	for k,v in pairs(getMats) do
		group:AddReward(v.id, v.num)
	end	
	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end
	return 0
end
