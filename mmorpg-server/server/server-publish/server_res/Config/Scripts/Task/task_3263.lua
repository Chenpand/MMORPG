function Init(task)				---°ó¶¨(×¢²á)¸ÃÈÎÎñÐèÒª¹Ø×¢µÄÊÂ¼þ
    task:BindClearDungeonEvent(207002, 3)	-- ¹Ø×¢Í¨¹ØÄ³¸öµØÏÂ³ÇÊÂ¼þ£¨µØÏÂ³Ç£¬ÆÀ¼Û£¬Ê±¼ä/ºÁÃë£©
    task:BindClearDungeonEvent(207003, 3)	-- ¹Ø×¢Í¨¹ØÄ³¸öµØÏÂ³ÇÊÂ¼þ£¨µØÏÂ³Ç£¬ÆÀ¼Û£¬Ê±¼ä/ºÁÃë£©
end

function CheckAccept(player)		---	¼ì²é½ÓÊÜÌõ¼þ£¬ÎÞÌõ¼þÖ±½Ó·µ»Øreturn true
	return true;
end

function OnAccepted(player, task)			---	½ÓÊÜÈÎÎñÊ±ÐèÒªÖ´ÐÐµÄ²Ù×÷	
    task:SetVar("parameter", 0);
end

function OnSubmitted(player, task)		---	Ìá½»ÈÎÎñÊ±ÐèÒªÖ´ÐÐµÄ²Ù×÷
end

function OnAbandomed(player, task)		---	·ÅÆúÈÎÎñÊ±ÐèÒªÖ´ÐÐµÄ²Ù×÷
end


function OnClearDungeon(player, task, dungeonId, beated, areas)
    task:SetVar("parameter", 1);
	task:SetStatus(2);
end


function CheckFinish(player, task)		---��������������
end
