function Init(task)				---��(ע��)��������Ҫ��ע���¼�	
	task:BindClearDungeonEventByRevive(0, 1); --��עͨ��ĳ�����³�(���³�ID, �������)

end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end


function OnClearDungeon(player, task, dungeonId, beated, areas)
	local num = task:GetVar("parameter");
	local dungeonType = dungeonId % 10;		---��ȡ���³��Ѷ�(0:��ͨ,1:ð��,2:��ʿ,3:����,4:����)
		num = num + 1
		task:SetVar("parameter", num);					---	���ñ�����ֵ
	if num >= 3 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end
