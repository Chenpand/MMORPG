function OnUseSelf(player, item)
		local reason="2012;800000024;0"
		player:ReduceItemNum(reason, item, 1)
		player:AddItem(reason, 600000001, 50000000, 0, 0, 1)	
end