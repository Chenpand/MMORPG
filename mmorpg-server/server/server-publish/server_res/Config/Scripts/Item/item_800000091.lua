--附魔卡礼�?
function OnUseSelf(player, item)
	--奖池道具
	local reason="2012;800000091;0"
	local bonusPool = {	item1= {id=220000901,num=1,wt=500},
	                    item2= {id=230000901,num=1,wt=136},
						item3= {id=230000902,num=1,wt=50},
						item4= {id=230000903,num=1,wt=42},
						item5= {id=220000902,num=1,wt=500},
						item6= {id=230000904,num=1,wt=137},
						item7= {id=220000903,num=1,wt=500},
						item8= {id=230000905,num=1,wt=136},
						item9= {id=230000906,num=1,wt=50},
						item10= {id=230000907,num=1,wt=43},
						item11= {id=220000904,num=1,wt=500},
						item12= {id=230000908,num=1,wt=136},
						item13= {id=230000909,num=1,wt=50},
						item14= {id=230000910,num=1,wt=43},
						item15= {id=220000905,num=1,wt=500},
						item16= {id=230000911,num=1,wt=136},
						item17= {id=230000912,num=1,wt=50},
						item18= {id=230000913,num=1,wt=43},
						item19= {id=220000906,num=1,wt=500},
						item20= {id=230000914,num=1,wt=136},
						item21= {id=230000915,num=1,wt=50},
						item22= {id=220000907,num=1,wt=500},
						item23= {id=230000916,num=1,wt=43},
						item24= {id=220000908,num=1,wt=500},
						item25= {id=220000909,num=1,wt=500},
						item26= {id=220000910,num=1,wt=500},
						item27= {id=230000917,num=1,wt=137},
						item28= {id=230000918,num=1,wt=50},
						item29= {id=230000919,num=1,wt=43},
						item30= {id=220000911,num=1,wt=500},
						item31= {id=230000920,num=1,wt=137},
						item32= {id=220000912,num=1,wt=500},
						item33= {id=230000921,num=1,wt=136},
						item34= {id=230000922,num=1,wt=50},
						item35= {id=230000923,num=1,wt=43},
						item36= {id=220000913,num=1,wt=500},
						item37= {id=230000924,num=1,wt=137},
						item38= {id=230000925,num=1,wt=50},
						item39= {id=230000926,num=1,wt=136},
						item40= {id=230000927,num=1,wt=50},
						item41= {id=220000914,num=1,wt=500},
						item42= {id=220000915,num=1,wt=500},
						item43= {id=230000928,num=1,wt=50},
						item44= {id=240000901,num=1,wt=14},
						item45= {id=240000902,num=1,wt=14},
						item46= {id=240000903,num=1,wt=15},
						item47= {id=240000904,num=1,wt=14},
						item48= {id=240000905,num=1,wt=14},
						item49= {id=240000906,num=1,wt=15},
						item50= {id=240000907,num=1,wt=14},
						item51= {id=230000929,num=1,wt=50},
						item52= {id=230000930,num=1,wt=50} }

	local totalWeight = 0
	for k, v in pairs(bonusPool) do
		totalWeight = totalWeight + v.wt
	end

	local randWt = Sys:RandBetween(1, totalWeight)

	--随机出抽到的奖励道具
	local sumWt = 0
	
	local group = player:GetRewardGroup()

	for k, v in pairs(bonusPool) do
		sumWt = v.wt + sumWt
		if randWt <= sumWt then
			group:AddReward(v.id, v.num)
			break	
		end
	end

	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end

	-- 这里写上理由
	player:AddRewards(reason, group, true)
    player:AddItem(reason, 600000007, 100, 0, 0, 1)  
	player:ReduceItemNum(reason, item, 1)
	return 0
end