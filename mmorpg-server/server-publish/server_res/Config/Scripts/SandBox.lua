--����ɳ�价��, added by aprilliu, t ��ʾ�����»��������԰�ִ�й�SetSandBox��t ���ø��������� �������µĻ�����
function SetSandBox(t)
	local base = { __index = _G;}
	setmetatable(t, base);
end

--�̳л�����require
function require_env(module, env)
	package.loaded[module] = nil;
	_G.PARENT_ENV = env;
	require(module);
end
 