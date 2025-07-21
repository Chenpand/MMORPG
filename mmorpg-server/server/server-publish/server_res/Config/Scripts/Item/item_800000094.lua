function OnUseSelf(player, item)
	--local job = player::GetMainOccu()
	local job = math.ceil(player:GetOccu())
	local reason="2012;800000094;0"
	if job == 12 or job==10 then
		local giftpack = {item1 = {id=531003007,num=1}, item2 = {id=531003008,num=1},
	                      item3 = {id=531003009,num=1}, item4 = {id=531003010,num=1},
					      item5 = {id=531003011,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum(reason, item, 1)
		player:AddRewards(reason, group, true)
		return 0

	elseif job == 21 or job==20 or job==22 then
		local giftpack = {item1 = {id=532003007,num=1}, item2 = {id=532003008,num=1},
	                      item3 = {id=532003009,num=1}, item4 = {id=532003010,num=1},
					      item5 = {id=532003011,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum(reason, item, 1)
		player:AddRewards(reason, group, true)
		return 0

	elseif job == 33 or job==30 then
		local giftpack = {item1 = {id=533003007,num=1}, item2 = {id=533003008,num=1},
	                      item3 = {id=533003009,num=1}, item4 = {id=533003010,num=1},
					      item5 = {id=533003011,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum(reason, item, 1)
		player:AddRewards(reason, group, true)
		return 0
		
	elseif job == 51 or job==50 then
		local giftpack = {item1 = {id=535003007,num=1}, item2 = {id=535003008,num=1},
	                      item3 = {id=535003009,num=1}, item4 = {id=535003010,num=1},
					      item5 = {id=535003011,num=1}}
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
