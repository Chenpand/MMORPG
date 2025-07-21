function OnUseSelf(player, item)
	--local job = player::GetMainOccu()
	local job = math.ceil(player:GetOccu())
	
	if job == 12 then
		local giftpack = {item1 = {id=145411001,num=1,str=13}, item2 = {id=144770001,num=1,str=13},
					item3 = {id=144771001,num=1,str=13}, item4 = {id=144772001,num=1,str=13},
					item5 = {id=144773001,num=1,str=13}, item6 = {id=144774001,num=1,str=13},
					item7 = {id=145492001,num=1,str=13}, item8 = {id=145491001,num=1,str=13},
					item9 = {id=145490001,num=1,str=13}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num, 1, v.str)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum("useItem", item, 1)
		player:AddRewards("useItem", group, true)
		return 0
		
	elseif job == 21 then
		local giftpack = {item1 = {id=145415001,num=1,str=13}, item2 = {id=144750001,num=1,str=13},
					item3 = {id=144751001,num=1,str=13}, item4 = {id=144752001,num=1,str=13},
					item5 = {id=144753001,num=1,str=13}, item6 = {id=144754001,num=1,str=13},
					item7 = {id=145492001,num=1,str=13}, item8 = {id=145491001,num=1,str=13},
					item9 = {id=145490001,num=1,str=13}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num, 1, v.str)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum("useItem", item, 1)
		player:AddRewards("useItem", group, true)
		return 0

	elseif job == 22 then
		local giftpack = {item1 = {id=145417001,num=1,str=13}, item2 = {id=144770001,num=1,str=13},
					item3 = {id=144771001,num=1,str=13}, item4 = {id=144772001,num=1,str=13},
					item5 = {id=144773001,num=1,str=13}, item6 = {id=144774001,num=1,str=13},
					item7 = {id=145492001,num=1,str=13}, item8 = {id=145491001,num=1,str=13},
					item9 = {id=145490001,num=1,str=13}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num, 1, v.str)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum("useItem", item, 1)
		player:AddRewards("useItem", group, true)
		return 0
		
	elseif job == 33 then
		local giftpack = {item1 = {id=145420001,num=1,str=13}, item2 = {id=144740001,num=1,str=13},
					item3 = {id=144741001,num=1,str=13}, item4 = {id=144242001,num=1,str=13},
					item5 = {id=144243001,num=1,str=13}, item6 = {id=144244001,num=1,str=13},
					item7 = {id=145492001,num=1,str=13}, item8 = {id=145491001,num=1,str=13},
					item9 = {id=145490001,num=1,str=13}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num, 1, v.str)
		end	
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum("useItem", item, 1)
		player:AddRewards("useItem", group, true)
		return 0
	end		
	
end
