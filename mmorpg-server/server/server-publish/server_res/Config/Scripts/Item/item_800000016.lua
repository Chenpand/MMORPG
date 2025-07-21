function OnUseSelf(player, item)
		local reason="2012;800000016;0"
		player:ReduceItemNum(reason, item, 1)
		player:AddItem(reason, 600000001, 6000000, 0, 0, 1)	
end