function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(208000, 0, 180000, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
    task:BindClearDungeonEvent(208001, 0, 180000, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
    task:BindClearDungeonEvent(208002, 0, 180000, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
    task:BindClearDungeonEvent(208003, 0, 180000, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
	
	task:BindClearDungeonEventByRevive(208003, 10); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEvent(208003, 0, 0 10); --��עͨ��ĳ�����³�,�޶��������
	
	task:BindClearDungeonEvent(0, 0, 0)				-- ��עͨ���κε��³�
	task:BindClearDungeonEvent(0)					-- ��עͨ���κε��³�
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
	local dungeonType = dungeonId % 10;		---��ȡ���³��Ѷ�(0:��ͨ,1:ð��,2:��ʿ,3:����,4:����)
	--if dungeonType >= 3 then
		--task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	--end
	
	local teamsize = player:GetTeamSize();	--��ȡ�������
	--if teamsize >= 2 then
		--task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	--end
	
	if(beated < 10 and areas >= 2) then   -- ���ͨ��ʱ���������Լ�ͨ���������Ƿ�����Ҫ��
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
