function OnUseSelf(player, item)
	--local job =	player::GetMainOccu()
	local job = math.ceil(player:GetOccu()/10)	

	if job == 1 then
		local bonusPool = {item1 = {id=124070001,num=1,wt=3000},item2 = {id=124071001,num=1,wt=3000},item3 = {id=124072001,num=1,wt=3000},item4 = {id=124073001,num=1,wt=3000},
		item5 = {id=124074001,num=1,wt=3000},item6 = {id=124570001,num=1,wt=3000},item7 = {id=124571001,num=1,wt=3000},item8 = {id=124772001,num=1,wt=3000},item9 = {id=124773001,num=1,wt=3000},
		item10 = {id=124774001,num=1,wt=3000},item11 = {id=125270001,num=1,wt=3000},item12 = {id=125271001,num=1,wt=3000},item13 = {id=125272001,num=1,wt=3000},item14 = {id=125273001,num=1,wt=3000},
		item15 = {id=125274001,num=1,wt=3000},item16 = {id=134271003,num=1,wt=50},item17 = {id=134270004,num=1,wt=50},item18 = {id=134272004,num=1,wt=50},item19 = {id=134273004,num=1,wt=50},
		item20 = {id=134274004,num=1,wt=50},item21 = {id=134270002,num=1,wt=100},item22 = {id=134270003,num=1,wt=100},item23 = {id=134271002,num=1,wt=100},item24 = {id=134271004,num=1,wt=100},
		item25 = {id=134272002,num=1,wt=100},item26 = {id=134272003,num=1,wt=100},item27 = {id=134273002,num=1,wt=100},item28 = {id=134273003,num=1,wt=100},item29 = {id=134274002,num=1,wt=100},
		item30 = {id=134274003,num=1,wt=100},item31 = {id=134770001,num=1,wt=30},item32 = {id=134771001,num=1,wt=30},item33 = {id=134772001,num=1,wt=30},item34 = {id=134773001,num=1,wt=30},
		item35 = {id=134774001,num=1,wt=30},item36 = {id=134770002,num=1,wt=100},item37 = {id=134770003,num=1,wt=100},item38 = {id=134771002,num=1,wt=100},item39 = {id=134771003,num=1,wt=100},
		item40 = {id=134772002,num=1,wt=100},item41 = {id=134772003,num=1,wt=100},item42 = {id=134773002,num=1,wt=100},item43 = {id=134773003,num=1,wt=100},item44 = {id=134774002,num=1,wt=100},
		item45 = {id=134774003,num=1,wt=100},item46 = {id=135470003,num=1,wt=10},item47 = {id=135471003,num=1,wt=10},item48 = {id=135472003,num=1,wt=10},item49 = {id=135473004,num=1,wt=10},
		item50 = {id=135474003,num=1,wt=10},item51 = {id=135470002,num=1,wt=100},item52 = {id=135470004,num=1,wt=100},item53 = {id=135471002,num=1,wt=100},item54 = {id=135471004,num=1,wt=100},
		item55 = {id=135472002,num=1,wt=100},item56 = {id=135472004,num=1,wt=100},item57 = {id=135473002,num=1,wt=100},item58 = {id=135473003,num=1,wt=100},item59 = {id=135474002,num=1,wt=100},
		item60 = {id=135474004,num=1,wt=100},item61 = {id=145470001,num=1,wt=3},item62 = {id=145471001,num=1,wt=10},item63 = {id=145472001,num=1,wt=10},item64 = {id=144973001,num=1,wt=36},
		item65 = {id=144974001,num=1,wt=36}}

		local totalWeight = 0
			for k, v in pairs(bonusPool) do
			totalWeight = totalWeight + v.wt
		end

		local randWt = Sys:RandBetween(1, totalWeight)

		--随机出抽到的奖励道具
		local sumWt = 0
		local hitBonus = {}
		for k, v in pairs(bonusPool) do
			sumWt = v.wt + sumWt
			if randWt <= sumWt then
				hitBonus = v
				break	
			end
		end

		local bonusIds = ""
		for k, v in pairs(bonusPool) do
			if bonusIds == "" then
				bonusIds = v.id
			else
				bonusIds = bonusIds.."|"..v.id
			end
		end

		local group = player:GetRewardGroup()
    	group:AddReward(hitBonus.id, hitBonus.num)
    	local canAdd = player:CheckAddRewards(group, item, 1)
    	if canAdd ~= 0 then return canAdd end

		player:ReduceItemNum("2012;200000106;0", item, 1)
		player:SendMagicJarResult(bonusIds, hitBonus.id, hitBonus.num)


		return 0
		
	elseif job == 2 then
		local bonusPool = {item1 = {id=124050001,num=1,wt=3000},item2 = {id=124051001,num=1,wt=3000},item3 = {id=124052001,num=1,wt=3000},item4 = {id=124053001,num=1,wt=3000},
		item5 = {id=124054001,num=1,wt=3000},item6 = {id=124550001,num=1,wt=3000},item7 = {id=124551001,num=1,wt=3000},item8 = {id=124752001,num=1,wt=3000},item9 = {id=124753001,num=1,wt=3000},
		item10 = {id=124754001,num=1,wt=3000},item11 = {id=125250001,num=1,wt=3000},item12 = {id=125251001,num=1,wt=3000},item13 = {id=125252001,num=1,wt=3000},item14 = {id=125253001,num=1,wt=3000},
		item15 = {id=125254001,num=1,wt=3000},item16 = {id=134250001,num=1,wt=200},item17 = {id=134251002,num=1,wt=200},item18 = {id=134252001,num=1,wt=200},item19 = {id=134253002,num=1,wt=200},
		item20 = {id=134254002,num=1,wt=200},item21 = {id=134250002,num=1,wt=50},item22 = {id=134251001,num=1,wt=50},item23 = {id=134252002,num=1,wt=50},item24 = {id=134253001,num=1,wt=50},
		item25 = {id=134254001,num=1,wt=50},item26 = {id=134750001,num=1,wt=30},item27 = {id=134751001,num=1,wt=30},item28 = {id=134752001,num=1,wt=30},item29 = {id=134753002,num=1,wt=30},
		item30 = {id=134754002,num=1,wt=30},item31 = {id=134750002,num=1,wt=200},item32 = {id=134751002,num=1,wt=200},item33 = {id=134752002,num=1,wt=200},item34 = {id=134753001,num=1,wt=200},
		item35 = {id=134754001,num=1,wt=200},item36 = {id=135450002,num=1,wt=10},item37 = {id=135451002,num=1,wt=10},item38 = {id=135452002,num=1,wt=10},item39 = {id=135453002,num=1,wt=10},
		item40 = {id=135454002,num=1,wt=10},item41 = {id=135450001,num=1,wt=200},item42 = {id=135451001,num=1,wt=200},item43 = {id=135452001,num=1,wt=200},item44 = {id=135453001,num=1,wt=200},
		item45 = {id=135454001,num=1,wt=200},item46 = {id=145450001,num=1,wt=3},item47 = {id=145451001,num=1,wt=10},item48 = {id=145452001,num=1,wt=10},item49 = {id=144953001,num=1,wt=36},
		item50 = {id=144954001,num=1,wt=36}}

		local totalWeight = 0
			for k, v in pairs(bonusPool) do
			totalWeight = totalWeight + v.wt
		end

		local randWt = Sys:RandBetween(1, totalWeight)

		--随机出抽到的奖励道具
		local sumWt = 0
		local hitBonus = {}
		for k, v in pairs(bonusPool) do
			sumWt = v.wt + sumWt
			if randWt <= sumWt then
				hitBonus = v
				break	
			end
		end

		local bonusIds = ""
		for k, v in pairs(bonusPool) do
			if bonusIds == "" then
				bonusIds = v.id
			else
				bonusIds = bonusIds.."|"..v.id
			end
		end

		local group = player:GetRewardGroup()
    	group:AddReward(hitBonus.id, hitBonus.num)
    	local canAdd = player:CheckAddRewards(group, item, 1)
    	if canAdd ~= 0 then return canAdd end

		player:ReduceItemNum("2012;200000106;0", item, 1)
		player:SendMagicJarResult(bonusIds, hitBonus.id, hitBonus.num)
		return 0
		
	end		
		
end