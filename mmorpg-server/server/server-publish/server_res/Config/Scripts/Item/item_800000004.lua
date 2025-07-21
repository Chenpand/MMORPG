function OnUseSelf(player, item)
	--local job =	player::GetMainOccu()
	local job = math.ceil(player:GetOccu()/10)	

	if job == 1 then
		local giftpack = {item1 = {id=133911001,num=1,str=10}, item2 = {id=133270002,num=1,str=10},
					item3 = {id=133271002,num=1,str=10}, item4 = {id=133272001,num=1,str=10},
					item5 = {id=133273001,num=1,str=10}, item6 = {id=133274002,num=1,str=10},
					item7 = {id=133291001,num=1,str=10}, item8 = {id=133290001,num=1,str=10},
					item9 = {id=133292001,num=1,str=10}, item10 = {id=600000003,num=7644110},
					item11 = {id=130193032,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num, 1, v.str)
		end		
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum("useItem", item, 1)
		player:AddRewards("useItem", group, true)
		return 0
		
	elseif job == 2 then
		local giftpack = {item1 = {id=133915001,num=1,str=10}, item2 = {id=133250002,num=1,str=10},
					item3 = {id=133251002,num=1,str=10}, item4 = {id=133252002,num=1,str=10},
					item5 = {id=133253002,num=1,str=10}, item6 = {id=133254002,num=1,str=10},
					item7 = {id=133291001,num=1,str=10}, item8 = {id=133290001,num=1,str=10},
					item9 = {id=133292001,num=1,str=10}, item10 = {id=600000003,num=7644110},
					item11 = {id=130193032,num=1}}
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num, 1, v.str)
		end
		local canAdd = player:CheckAddRewards(group, item, 1)
		if canAdd ~= 0 then return canAdd end
		player:ReduceItemNum("useItem", item, 1)
		player:AddRewards("useItem", group, true)
		return 0
		
	elseif job == 3 then
		local giftpack = {item1 = {id=133920001,num=1,str=10}, item2 = {id=133241004,num=1,str=10},
					item3 = {id=133242002,num=1,str=10}, item4 = {id=133243001,num=1,str=10},
					item5 = {id=133244001,num=1,str=10}, item6 = {id=133240003,num=1,str=10},
					item7 = {id=133291001,num=1,str=10}, item8 = {id=133290001,num=1,str=10},
					item9 = {id=133292001,num=1,str=10}, item10 = {id=600000003,num=7644110},									item11 = {id=130193032,num=1}}
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