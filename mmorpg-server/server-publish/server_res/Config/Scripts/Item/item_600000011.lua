function OnUseSelf(player, item)
	local reason="2012;600000011;0"
	player:ReduceItemNum(reason, item, 1)
	player:AddSP(reason ,10)
	return 0
end