function OnUseSelf(player, item)
	local reason="2012;800000035;0"
	local num = player:GetLostFatigue();
    if num >= 5 then
        player:ReduceItemNum(reason, item, 1)
        player:AddFatigue(reason, 5, true)
        return 0
    else 
        return 2007
    end
    
end