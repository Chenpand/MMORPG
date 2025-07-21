--设置沙箱环境, added by aprilliu, t 表示的是新环境表，可以把执行过SetSandBox的t 设置给其他函数 当作其新的环境表
function SetSandBox(t)
	local base = { __index = _G;}
	setmetatable(t, base);
end

--继承环境的require
function require_env(module, env)
	package.loaded[module] = nil;
	_G.PARENT_ENV = env;
	require(module);
end
 