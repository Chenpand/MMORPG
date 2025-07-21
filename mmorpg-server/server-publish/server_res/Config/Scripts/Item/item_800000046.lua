function OnUseSelf(player, item)
	local num = player:GetLevel()
	local reason="2012;800000046;0"
	player:ReduceItemNum(reason, item, 1)
	player:AddExpFromRewardAdapter(1002, num)
		
end
