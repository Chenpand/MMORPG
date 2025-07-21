function OnUseSelf(player, item)
	--local job = player::GetMainOccu()
	local job = math.ceil(player:GetOccu()/10)
	
	if job == 1 then
		local giftpack = {item1 = {id=120511001,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum("useItem", item, 1)
		player:AddRewards("useItem", group, true)
		return 0
		
	elseif job == 2 then
		local giftpack = {item1 = {id=120515001,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum("useItem", item, 1)
		player:AddRewards("useItem", group, true)
		return 0
		
	elseif job == 3 then
		local giftpack = {item1 = {id=120620001,num=1}}
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
	
end
