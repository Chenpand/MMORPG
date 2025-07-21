function OnUseSelf(player, item)
		local reason="2012;800000015;0"
		player:ReduceItemNum(reason, item, 1)
		player:AddItem(reason, 600000001, 5000000, 0, 0, 1)	
end