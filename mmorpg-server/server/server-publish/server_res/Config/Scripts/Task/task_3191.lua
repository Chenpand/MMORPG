function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindChangeOccuEvent()	--תְ
end

function CheckTask(player, task)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    local isInitOccu = player:IsInitOccu();     --��ȡ��ɫ�Ƿ��ǳ�ʼְҵ
    local num = task:GetVar("parameter");   --- ��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
    if not isInitOccu then                      --��ɫ������15���������תְ
        num = num + 1
        task:SetVar("parameter",num);
        task:SetStatus(2)                       ---��������޸�״̬
    end

end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnChangeOccu(player, task)		--תְ
    local num = task:GetVar("parameter");   --- ��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
    num = num + 1
    task:SetVar("parameter",num)
	task:SetStatus(2)
end




function CheckFinish(player, task)		---��������������
end
