function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearActivityDungeonEvent(2002000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
	task:BindClearActivityDungeonEvent(2002001)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
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


function OnClearActivityDungeon(player, task, dungeonId, beated, areas)
        local num = task:GetVar("parameter");   --- ��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
        num = num + 1;
        task:SetVar("parameter",num);               --- ���ñ�����ֵ
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
end


function CheckFinish(player, task)		---��������������
end
