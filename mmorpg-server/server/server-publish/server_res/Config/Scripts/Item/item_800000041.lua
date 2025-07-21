function OnUseSelf(player, item)
	--local job = player::GetMainOccu()
	local reason="2012;800000041;0"
	local job = math.ceil(player:GetOccu()/10)
	
	if job == 1 then
		local giftpack = {item1 = {id=300001001,num=10}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum(reason, item, 1)
		player:AddRewards(reason, group, true)
		return 0
		
	elseif job == 5 then
		local giftpack = {item1 = {id=300001002,num=10}}
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
	
end
