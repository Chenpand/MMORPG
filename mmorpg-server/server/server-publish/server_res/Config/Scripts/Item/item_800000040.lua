--魔盒
function OnUseSelf(player, item)
	--奖池道具
	local reason="2012;800000040;0"
	local bonusPool = {item1 = {id=210000901,num=1,wt=100},
item2 = {id=220000901,num=1,wt=100},
item3 = {id=230000901,num=1,wt=50},
item4 = {id=230000902,num=1,wt=2},
item5 = {id=220000902,num=1,wt=100},
item6 = {id=230000904,num=1,wt=50},
item7 = {id=210000902,num=1,wt=100},
item8 = {id=220000903,num=1,wt=100},
item9 = {id=230000905,num=1,wt=50},
item10 = {id=230000906,num=1,wt=2},
item11 = {id=210000903,num=1,wt=100},
item12 = {id=220000904,num=1,wt=100},
item13 = {id=230000908,num=1,wt=50},
item14 = {id=230000909,num=1,wt=2},
item15 = {id=210000904,num=1,wt=100},
item16 = {id=220000905,num=1,wt=100},
item17 = {id=230000911,num=1,wt=50},
item18 = {id=230000912,num=1,wt=2},
item19 = {id=220000906,num=1,wt=100},
item20 = {id=230000914,num=1,wt=50},
item21 = {id=230000915,num=1,wt=2},
item22 = {id=220000907,num=1,wt=50},
item23 = {id=220000908,num=1,wt=50},
item24 = {id=220000909,num=1,wt=50}
}

	local totalWeight = 0
    for k, v in pairs(bonusPool) do
        totalWeight = totalWeight + v.wt
    end
	local randWt = Sys:RandBetween(1, totalWeight)

	
	local sumWt = 0
	local hitBonus = {}
	for k, v in pairs(bonusPool) do
		sumWt = v.wt + sumWt
		if randWt <= sumWt then
			hitBonus = v
			break	
		end
	end

	local group = player:GetRewardGroup()
	group:AddReward(hitBonus.id, hitBonus.num)
	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end
	player:ReduceItemNum(reason, item, 1)
	player:AddItem(reason, hitBonus.id, hitBonus.num, 0, 0, 1)
	
	return 0
end