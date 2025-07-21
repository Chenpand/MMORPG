
function OnUseSelf(player, item)
	
	local bonusPool = {item1 = {id=201200002,num=1,wt=450},
item2 = {id=201200003,num=1,wt=450},
item3 = {id=201200004,num=1,wt=100},
item4 = {id=201200008,num=1,wt=450},
item5 = {id=201200009,num=1,wt=450},
item6 = {id=201200010,num=1,wt=100},
item7 = {id=201200014,num=1,wt=450},
item8 = {id=201200015,num=1,wt=450},
item9 = {id=201200016,num=1,wt=100},
item10 = {id=201200020,num=1,wt=450},
item11 = {id=201200021,num=1,wt=450},
item12 = {id=201200022,num=1,wt=100},
item13 = {id=201200026,num=1,wt=450},
item14 = {id=201200027,num=1,wt=450},
item15 = {id=201200028,num=1,wt=100},
item16 = {id=201200032,num=1,wt=450},
item17 = {id=201200033,num=1,wt=450},
item18 = {id=201200034,num=1,wt=100},
item19 = {id=201200038,num=1,wt=450},
item20 = {id=201200039,num=1,wt=450},
item21 = {id=201200040,num=1,wt=100},
item22 = {id=201200044,num=1,wt=450},
item23 = {id=201200045,num=1,wt=450},
item24 = {id=201200046,num=1,wt=100},
item25 = {id=201200050,num=1,wt=450},
item26 = {id=201200051,num=1,wt=450},
item27 = {id=201200052,num=1,wt=100},
item28 = {id=201200056,num=1,wt=450},
item29 = {id=201200057,num=1,wt=450},
item30 = {id=201200058,num=1,wt=100},
item31 = {id=201200062,num=1,wt=450},
item32 = {id=201200063,num=1,wt=450},
item33 = {id=201200064,num=1,wt=100},
item34 = {id=201200068,num=1,wt=450},
item35 = {id=201200069,num=1,wt=450},
item36 = {id=201200070,num=1,wt=100},
item37 = {id=201200074,num=1,wt=450},
item38 = {id=201200075,num=1,wt=450},
item39 = {id=201200076,num=1,wt=100},
item40 = {id=201200080,num=1,wt=450},
item41 = {id=201200081,num=1,wt=450},
item42 = {id=201200082,num=1,wt=100},
item43 = {id=201200086,num=1,wt=450},
item44 = {id=201200087,num=1,wt=450},
item45 = {id=201200088,num=1,wt=100},
item46 = {id=201200092,num=1,wt=450},
item47 = {id=201200093,num=1,wt=450},
item48 = {id=201200094,num=1,wt=100}}
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
	player:ReduceItemNum("useItem", item, 1)
	player:AddItem("useItem", hitBonus.id, hitBonus.num, 0, 0, 1)
	
	return 0
end