--场景初始化调用
function OnSceneInit(scene)
end

--场景卸载时调用
function OnSceneQuit(scene)
end

--场景开始卸载时调用
function OnSceneUnload(scene)
end

--重置关卡
function OnResetCustomsPass(player)
end

--检查能否进入
function CheckPlayerEnter(scene, playerId, tribeId, level)
	return true;
end

--检查玩家能否进入副本场景
function CheckPlayerEnterCopy(level, player)
	return true;
end

--玩家进入场景之前，在此处理位置
function OnBeforePlayerEnter(player)
end

--npc进入场景之前，在此处理位置
function OnBeforeNpcEnter(npc)
end

--玩家进入场景
function OnPlayerEnter(player)
end

--玩家离开场景
function OnPlayerLeave(player)
end

--心跳
function OnTick(scene, now)
end

--怪物被杀
function OnNpcKilled(npc, attacker)
end

--怪物死亡
function OnNpcDied(npc)
end

--玩家被杀死
function OnPlayerKilledByPlayer(dead, attacker)
end

--玩家被npc杀死
function OnPlayerKilledByNpc(dead, npc)
end

--怪物被杀死
function OnNpcKilledByPlayer(npc, attacker)
end

--固定伤害的特殊npc血量变化
function OnFixedDmgHpChanged(creature)
end

--进入事件陷阱
function OnEnterEventTrap(player, trapid)
	return false;
end
function OnNpcEnterEventTrap(npc, trapid)
	return false;
end

--进入传送点
function OnEnterTransZone(player, index)
	return 0;
end

--请求召唤战场NPC
function ReqSummonBattleCreature(player, builder, dataid, param)
end

--请求升级战场NPC
function ReqUpgradeBattleCreature(player, npc)
end

--进行某种操作
function DoOperate(player, opname, param)
end

--剧情结束
function OnStoryEnd(player, storyid)
end
