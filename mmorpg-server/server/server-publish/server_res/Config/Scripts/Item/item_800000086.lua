function OnUseSelf(player, item)
	local giftpack = {item1= {id=330000025,num=100}, item2= {id=330000026,num=100},
	                  item3= {id=330000027,num=100}, item4= {id=330000028,num=100},
					  item5= {id=330000029,num=100}, item6= {id=330000030,num=100},
					  item7= {id=330000031,num=100}, item8= {id=330000032,num=100},
					  item9= {id=330000033,num=100}, item10= {id=330000034,num=100},
					  item11= {id=330000035,num=100}, item12= {id=330000036,num=100},
					  item13= {id=330000037,num=100}, item14= {id=330000038,num=100},
					  item15= {id=330000039,num=100}, item16= {id=330000040,num=100},
					  item17= {id=330000041,num=100}, item18= {id=330000042,num=100},
					  item19= {id=330000043,num=100}, item20= {id=330000044,num=100},
					  item21= {id=330000045,num=100}, item22= {id=330000046,num=100},
					  item23= {id=330000047,num=100}, item24= {id=330000048,num=100},
					  item25= {id=330000049,num=100}, item26= {id=330000050,num=100},
					  item27= {id=330000051,num=100}, item28= {id=330000052,num=100},
					  item29= {id=330000053,num=100}, item30= {id=330000054,num=100},
					  item31= {id=330000055,num=100}, item32= {id=330000056,num=100},
					  item33= {id=330000057,num=100}, item34= {id=330000058,num=100},
					  item35= {id=330000059,num=100}, item36= {id=330000060,num=100},
					  item37= {id=330000061,num=100}, item38= {id=330000062,num=100},
					  item39= {id=330000063,num=100}, item40= {id=330000064,num=100},
					  item41= {id=330000065,num=100}, item42= {id=330000066,num=100},
					  item43= {id=330000067,num=100}, item44= {id=330000068,num=100},
					  item45= {id=330000069,num=100}, item46= {id=330000070,num=100},
					  item47= {id=330000071,num=100}, item48= {id=330000072,num=100},
					  item49= {id=330000073,num=100}, item50= {id=330000074,num=100},
					  item51= {id=330000075,num=100}, item52= {id=330000076,num=100},
					  item53= {id=330000077,num=100}, item54= {id=330000078,num=100},
					  item55= {id=330000079,num=100}, item56= {id=330000080,num=100},
					  item57= {id=330000081,num=100}, item58= {id=330000082,num=100},
					  item59= {id=330000083,num=100}, item60= {id=330000084,num=100},
					  item61= {id=330000085,num=100}, item62= {id=330000086,num=100},
					  item63= {id=330000087,num=100}, item64= {id=330000088,num=100},
					  item65= {id=330000089,num=100}, item66= {id=330000090,num=100},
					  item67= {id=330000091,num=100}, item68= {id=330000092,num=100},
					  item69= {id=330000093,num=100}, item70= {id=330000094,num=100},
					  item71= {id=330000095,num=100}, item72= {id=330000096,num=100},
					  item73= {id=300000105,num=10000}, item74= {id=600000001,num=100000}} 

	local group = player:GetRewardGroup()
	for k,v in pairs(giftpack) do
		group:AddReward(v.id, v.num)
	end

	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end
	local reason="2012;800000086;0"
	player:ReduceItemNum(reason, item, 1)
	player:AddRewards(reason, group, true)
	return 0
end