function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearActivityDungeonEvent(3001000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
		task:BindClearActivityDungeonEvent(3001001)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
		task:BindClearActivityDungeonEvent(3001002)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
		task:BindClearActivityDungeonEvent(3001003)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(3002000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
		task:BindClearActivityDungeonEvent(3002001)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
		task:BindClearActivityDungeonEvent(3002002)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
		task:BindClearActivityDungeonEvent(3002003)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(3003000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
		task:BindClearActivityDungeonEvent(3003001)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
		task:BindClearActivityDungeonEvent(3003002)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
		task:BindClearActivityDungeonEvent(3003003)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
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
    		if num >= 2 then
					task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
				end			
end


function CheckFinish(player, task)		---��������������
end
