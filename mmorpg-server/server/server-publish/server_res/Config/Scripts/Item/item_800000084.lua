function OnUseSelf(player, item)
	local num = player:GetLostFatigue();
    if num >= 10 then
        local reason="2012;800000084;0"
        player:ReduceItemNum(reason, item, 1)
        player:AddFatigue(reason, 10, true)
        return 0
    else 
        return 2007
    end
    
end