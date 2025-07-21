require "Debug"
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindGetItemEvent(400000015)		---��ע��õ����¼���
    task:BindGetItemEvent(400000016)		---��ע��õ����¼���
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local item1Num = player:GetItemNum(400000015);
	local item2Num = player:GetItemNum(400000016);
			task:SetVar("item1", item1Num);	
			task:SetVar("item2", item2Num);
			task:SetVar("submitCount", 5);

        task:SetStatus(1);
		if item1Num >= 5 and item2Num >= 5 then
			task:SetStatus(2);
		end	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
		local item1Num = player:GetItemNum(400000015);
        local item2Num = player:GetItemNum(400000016);
        
		if(item1Num < 5 or item2Num < 5) then
			return false;
		end

        local submitCount = task:GetVar("submitCount") - 1;

		task:SetVar("submitCount", submitCount);
       

		--�һ���������
		if submitCount < 0 then	
			task:SetStatus(4);	--�һ���������,����Ϊ�ύ��
		else
			task:SetStatus(1);			-- �һ�����û��,��������Ϊ�ѽ���
		end

        player:RemoveItem("16001;1000;0",400000015, 5);        ---  ɾ������ֵ�ĵ���
        player:RemoveItem("16001;1000;0",400000016, 5);        ---  ɾ������ֵ�ĵ���

        return true
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnGetItem(player, task, itemid, num)		---	��������л�õ���ʱ��Ҫִ�еĲ���
	local item1Num = player:GetItemNum(400000015);
	local item2Num = player:GetItemNum(400000016);

	if 400000015 == itemid then
		task:SetVar("item1",item1Num);
	elseif 400000016 == itemid then
		task:SetVar("item2",item2Num);
	end

	local submitCount = task:GetVar("submitCount");

	if item1Num >= 5 and item2Num >= 5 and submitCount > 0 then						---	�жϽ�������
		task:SetStatus(2)
        return
	end

    if item1Num < 5 and item2Num < 5 and submitCount > 0 then                     --- �жϽ�������
        task:SetStatus(2)
        return
    end

    if submitCount <= 0 then 
        task:SetStatus(5)
        return
    end
end


function OnRmItem(player, task, itemid, num)
	local item1Num = player:GetItemNum(400000015)
    local item2Num = player:GetItemNum(400000016)

    if item1Num < 0 then item1Num = 0 end
    if item2Num < 0 then item2Num = 0 end
    local submitCount = task:GetVar("submitCount");

	if 400000015 == itemid then
		task:SetVar("item1",item1Num);
	elseif 400000016 == itemid then
		task:SetVar("item2",item2Num);
	end

	if(task:GetStatus() == 2 and item1Num < 5 and item2Num < 5) then		-- ���ɾ��һ�������ĵ��ߺ������������������������ø�����Ϊδ���״̬
			task:SetStatus(1);
            print("task:SetStatus(1)")
            return
	end

    if item1Num >= 5 and item2Num >= 5 and submitCount > 0 then                     --- �жϽ�������
        task:SetStatus(2)
        print("task:SetStatus(2)")
        return
    end

    if submitCount <= 0 then 
        task:SetStatus(5)
        print("task:SetStatus(5)")
        return
    end
end	