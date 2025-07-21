--根据npc血量获取撕咬技能的cd
function AIGetSiYaoCdByHp(npc)
	local maxhp = npc:GetMaxHp();
	local hp = npc:GetHp();
	
	local percent = hp * 10 / maxhp;
	if percent > 9 then
		return 15;
	elseif percent > 8 then
		return 14;
	elseif percent > 7 then
		return 13;
	elseif percent > 6 then
		return 14;
	elseif percent > 5 then
		return 13;
	elseif percent > 4 then
		return 12;
	elseif percent > 3 then
		return 11;
	elseif percent > 2 then
		return 10;
	elseif percent > 1 then
		return 9;
	else
		return 8;
	end
end
