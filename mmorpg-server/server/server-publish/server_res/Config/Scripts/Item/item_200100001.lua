function OnUseSelf(player, item)
	player:ReduceItemNum("2012;200100001;0", item, 1)
    player:AddRetinue(1000);
end
