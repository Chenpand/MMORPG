function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindClearDungeonEvent(0, 0, 0)				-- ��עͨ���κε��³�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	if num >= 3000 then
		task:SetStatus(2)							---��������޸�״̬
	else
		task:SetVar("parameter", num);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end


function OnClearDungeon(player, task, dungeonId, beated, areas)
	local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	num = num + 1;
	task:SetVar("parameter",num);					---	���ñ�����ֵ
	if num >= 3000 then
		task:SetStatus(2)							---��������޸�״̬
	end
end


function CheckFinish(player, task)		---��������������
end
