function OnUseSelf(player, item)
	local num = player:GetLostFatigue();
    if num >= 5 then
        player:ReduceItemNum("useItem", item, 1)
        player:AddFatigue("reason", 5, true)
        return 0
    else
        return 2007
    end
    
end