function OnUseSelf(player, item)
	--local job =	player::GetMainOccu()
	local job = math.ceil(player:GetOccu()/10)	

	if job == 1 then
		local giftpack = {item1 = {id=121010001,num=1}, item2 = {id=121170001,num=1},
					item3 = {id=121171001,num=1}, item4 = {id=121172001,num=1},
					item5 = {id=121173001,num=1}, item6 = {id=121174001,num=1},
					item7 = {id=121292001,num=1}, item8 = {id=121191001,num=1},
					item9 = {id=121290001,num=1}, item10 = {id=600000003,num=74137}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end		
		local canAdd = player:CheckAddRewards(group)
		if canAdd == false then return 1000010 end
		player:AddRewards("2012;200000102;0", group, true)
		return 0
		
	elseif job == 2 then
		local giftpack = {item1 = {id=121015001,num=1}, item2 = {id=121150001,num=1},
					item3 = {id=121151001,num=1}, item4 = {id=121152001,num=1},
					item5 = {id=121153001,num=1}, item6 = {id=121154001,num=1},
					item7 = {id=121292001,num=1}, item8 = {id=121191001,num=1},
					item9 = {id=121290001,num=1}, item10 = {id=600000003,num=74137}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end
		local canAdd = player:CheckAddRewards(group)
		if canAdd == false then return 1000010 end
		player:AddRewards("2012;200000102;0", group, true)
		return 0
		
	elseif job == 3 then
		local giftpack = {item1 = {id=121021001,num=1}, item2 = {id=121040001,num=1},
					item3 = {id=121041001,num=1}, item4 = {id=121042001,num=1},
					item5 = {id=121043001,num=1}, item6 = {id=121044001,num=1},
					item7 = {id=121292001,num=1}, item8 = {id=121191001,num=1},
					item9 = {id=121290001,num=1}, item10 = {id=600000003,num=74137}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end

		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum("2012;200000102;0", item, 1)
		player:AddRewards("2012;200000102;0", group, true)

		return 0
		
	end		
		
end