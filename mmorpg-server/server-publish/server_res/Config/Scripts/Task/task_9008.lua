function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindClearDungeonEvent(0, 0, 0)				-- ��עͨ���κε��³�
	task:BindClearActivityDungeonEvent(0)			-- ��עͨ���κλ���³�
	task:BindClearHellEvent(0, 0)
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
	if dungeonType <= 1 then
		local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
		num = num + 1;
		task:SetVar("parameter",num);					---	���ñ�����ֵ
		if num >= 5 then
			task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
		end
	end	
end

function OnClearActivityDungeon(player, task, dungeonId, beated, areas)
	local dungeonType = dungeonId % 10;		---��ȡ���³��Ѷ�(0:��ͨ,1:ð��,2:��ʿ,3:����,4:����)
	if dungeonType <= 1 then
		local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
		num = num + 1;
		task:SetVar("parameter",num);					---	���ñ�����ֵ
		if num >= 5 then
			task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
		end
	end	
end


function OnClearHell(player, task, dungeonId, usedTime)
	local dungeonType = dungeonId % 10;		---��ȡ���³��Ѷ�(0:��ͨ,1:ð��,2:��ʿ,3:����,4:����)
	if dungeonType <= 1 then
		local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
		num = num + 1;
		task:SetVar("parameter",num);					---	���ñ�����ֵ
		if num >= 5 then
			task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
		end
	end	
end



function CheckFinish(player, task)		---��������������
end
