function OnUseSelf(player, item)
	local num = player:GetLevel()
	local reason="2012;800000070;0"
	player:ReduceItemNum(reason, item, 1)
	player:AddExpFromRewardAdapter(1003, num)
		
end
