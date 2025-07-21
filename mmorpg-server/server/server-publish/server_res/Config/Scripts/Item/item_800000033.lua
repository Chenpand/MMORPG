
function OnUseSelf(player, item)
	
	local bonusPool = {item1 = {id=201200002,num=1,wt=450},
item2 = {id=201200003,num=1,wt=450},
item3 = {id=201200004,num=1,wt=90},
item4 = {id=201200005,num=1,wt=9},
item5 = {id=201200006,num=1,wt=1},
item6 = {id=201200008,num=1,wt=450},
item7 = {id=201200009,num=1,wt=450},
item8 = {id=201200010,num=1,wt=90},
item9 = {id=201200011,num=1,wt=9},
item10 = {id=201200012,num=1,wt=1},
item11 = {id=201200014,num=1,wt=450},
item12 = {id=201200015,num=1,wt=450},
item13 = {id=201200016,num=1,wt=90},
item14 = {id=201200017,num=1,wt=9},
item15 = {id=201200018,num=1,wt=1},
item16 = {id=201200020,num=1,wt=450},
item17 = {id=201200021,num=1,wt=450},
item18 = {id=201200022,num=1,wt=90},
item19 = {id=201200023,num=1,wt=9},
item20 = {id=201200024,num=1,wt=1},
item21 = {id=201200026,num=1,wt=450},
item22 = {id=201200027,num=1,wt=450},
item23 = {id=201200028,num=1,wt=90},
item24 = {id=201200029,num=1,wt=9},
item25 = {id=201200030,num=1,wt=1},
item26 = {id=201200032,num=1,wt=450},
item27 = {id=201200033,num=1,wt=450},
item28 = {id=201200034,num=1,wt=90},
item29 = {id=201200035,num=1,wt=9},
item30 = {id=201200036,num=1,wt=1},
item31 = {id=201200038,num=1,wt=450},
item32 = {id=201200039,num=1,wt=450},
item33 = {id=201200040,num=1,wt=90},
item34 = {id=201200041,num=1,wt=9},
item35 = {id=201200042,num=1,wt=1},
item36 = {id=201200044,num=1,wt=450},
item37 = {id=201200045,num=1,wt=450},
item38 = {id=201200046,num=1,wt=90},
item39 = {id=201200047,num=1,wt=9},
item40 = {id=201200048,num=1,wt=1},
item41 = {id=201200050,num=1,wt=450},
item42 = {id=201200051,num=1,wt=450},
item43 = {id=201200052,num=1,wt=90},
item44 = {id=201200053,num=1,wt=9},
item45 = {id=201200054,num=1,wt=1},
item46 = {id=201200056,num=1,wt=450},
item47 = {id=201200057,num=1,wt=450},
item48 = {id=201200058,num=1,wt=90},
item49 = {id=201200059,num=1,wt=9},
item50 = {id=201200060,num=1,wt=1},
item51 = {id=201200062,num=1,wt=450},
item52 = {id=201200063,num=1,wt=450},
item53 = {id=201200064,num=1,wt=90},
item54 = {id=201200065,num=1,wt=9},
item55 = {id=201200066,num=1,wt=1},
item56 = {id=201200068,num=1,wt=450},
item57 = {id=201200069,num=1,wt=450},
item58 = {id=201200070,num=1,wt=90},
item59 = {id=201200071,num=1,wt=9},
item60 = {id=201200072,num=1,wt=1},
item61 = {id=201200074,num=1,wt=450},
item62 = {id=201200075,num=1,wt=450},
item63 = {id=201200076,num=1,wt=90},
item64 = {id=201200077,num=1,wt=9},
item65 = {id=201200078,num=1,wt=1},
item66 = {id=201200080,num=1,wt=450},
item67 = {id=201200081,num=1,wt=450},
item68 = {id=201200082,num=1,wt=90},
item69 = {id=201200083,num=1,wt=9},
item70 = {id=201200084,num=1,wt=1},
item71 = {id=201200086,num=1,wt=450},
item72 = {id=201200087,num=1,wt=450},
item73 = {id=201200088,num=1,wt=90},
item74 = {id=201200089,num=1,wt=9},
item75 = {id=201200090,num=1,wt=1},
item76 = {id=201200092,num=1,wt=450},
item77 = {id=201200093,num=1,wt=450},
item78 = {id=201200094,num=1,wt=90},
item79 = {id=201200095,num=1,wt=9},
item80 = {id=201200096,num=1,wt=1},
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
	player:ReduceItemNum("useItem", item, 1)
	player:AddItem("useItem", hitBonus.id, hitBonus.num, 0, 0, 1)
	
	return 0
end