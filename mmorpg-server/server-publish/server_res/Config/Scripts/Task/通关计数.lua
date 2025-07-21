require "Debug"

function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(101000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearDungeonEvent(102000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
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
	local strIds = task:GetNameVar("ids");
	local ids = string.split(strIds, ",");
	local num = #ids;
	
	for i = 1, num do
		if ids[i] == dungeonId then
			return;
		end
	end
	
	if strIds == "" then
		strIds = tostring(dungeonId);
	else
		strIds = strIds..","..tostring(dungeonId);
	end
	
	task:SetNameVar("ids", strIds);
	
	num = num + 1;
    task:SetVar("parameter", num);

    if num >= 1 then
        task:SetStatus(2);        -- ��������ʱ��������Ϊ���״̬
    end     
end


function CheckFinish(player, task)		---��������������
end
