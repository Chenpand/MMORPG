function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindClearDungeonEvent(0)					-- ��עͨ���κε��³�
	task:BindClearActivityDungeonEvent(0)			-- ��עͨ���κλ���³�
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
	local teamsize = player:GetTeamSize();	--��ȡ�������
	if teamsize >= 2 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end


function OnClearActivityDungeon(player, task, dungeonId, beated, areas)
	local teamsize = player:GetTeamSize();	--��ȡ�������
	if teamsize >= 2 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end



function CheckFinish(player, task)		---��������������
end
