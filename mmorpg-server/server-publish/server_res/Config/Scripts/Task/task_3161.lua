function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearActivityDungeonEvent(601000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(601001)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(601002)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(601003)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(602000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(602001)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(602002)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(602003)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(603000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(603001)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(603002)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(603003)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    local num = task:GetVar("parameter");           --- ��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
    if num >= 1 then
        task:SetStatus(2)                           ---��������޸�״̬
    end	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end


function OnClearActivityDungeon(player, task, dungeonId, beated, areas)
    local num = task:GetVar("parameter");           --- ��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
    num = num + 1;
    task:SetVar("parameter",num);                   --- ���ñ�����ֵ
    if num >= 1 then
        task:SetStatus(2)                           ---��������޸�״̬
    end
end


function CheckFinish(player, task)		---��������������
end
