function OnUseSelf(player, item)
	--local job = player::GetMainOccu()
	local job = math.ceil(player:GetOccu())
	local reason="2012;800000104;0"
	if job == 12 then
		local giftpack = {item1 = {id=132272001,num=1},item2 = {id=132273001,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum(reason, item, 1)
		player:AddRewards(reason, group, true)
		return 0

	elseif job == 22 then
		local giftpack = {item1 = {id=132273002,num=1},item2 = {id=132272002,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum(reason, item, 1)
		player:AddRewards(reason, group, true)
		return 0

	elseif job == 33 then
		local giftpack = {item1 = {id=132242002,num=1},item2 = {id=132243001,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum(reason, item, 1)
		player:AddRewards(reason, group, true)
		return 0
		
	elseif job == 51 or job==21 then
		local giftpack = {item1 = {id=132252001,num=1},item2 = {id=132253001,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum(reason, item, 1)
		player:AddRewards(reason, group, true)
		return 0
		
	else 
        player:SendNotifyById(1206)
        return 0
	end	
	
	
end
