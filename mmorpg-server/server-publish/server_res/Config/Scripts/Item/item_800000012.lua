function OnUseSelf(player, item)
		local reason="2012;800000012;0"
		player:ReduceItemNum(reason, item, 1)
		player:AddItem(reason, 600000001, 2000000, 0, 0, 1)	
end