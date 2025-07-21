function OnUseSelf(player, item)
	player:ReduceItemNum("2021;0;0", item, 1)
    player:OnBuyMonthCard("2021;0;0");
end
