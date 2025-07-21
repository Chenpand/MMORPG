function OnUseSelf(player, item)
	local num = player:GetLostFatigue();
    if num >= 20 then
        local reason="2012;800000085;0"
        player:ReduceItemNum(reason, item, 1)
        player:AddFatigue(reason, 20, true)
        return 0
    else
        return 2007
    end
    
end