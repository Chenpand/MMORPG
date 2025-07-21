<?php
session_start();
include '../config/config.php';
list($msec, $sec) = explode(' ', microtime());
$msectime = (float)sprintf('%.0f', (floatval($msec) + floatval($sec)) * 1000);
if ($msectime - (float)$_SESSION['lasttime'] < 2000) {
	exit_notice('刷太快了',1);
} else {
	$_SESSION['lasttime'] = $msectime;
}
switch($_POST['action']){
	case 'senditem':
		$qid=intval($_POST['qid']);
		$uid=trim($_POST['uid']);
		if(!$uid) exit_notice($namecol.'不能为空',1);
		$pswd=trim($_POST['pswd']);
		if(!$pswd) exit_notice('后台密码不能为空',1);
		$mysqli = new mysqli($PZ['DB_HOST'], $PZ['DB_USER'], $PZ['DB_PWD'], $PZ['DB_NAME'], $PZ['DB_PORT']);
		if ($mysqli->connect_errno) exit_notice('数据库连接失败',1);
		$mysqli->set_charset($PZ['DB_CHARSET']);
		$result = $mysqli->query("select * from xy_gameconfig where Id={$qid} limit 1");
		$row = $result->fetch_assoc();
		$result->close();
		if(!$row) exit_notice('区服配置不存在',1);
		$serverid=$row['server'];
		$serverdb=$row['surface'];
		$result = $mysqli->query("select * from xy_config where name='gmmami' limit 1");
		$row = $result->fetch_assoc();
		$gmcode=$row['title'];
		$result->close();
		$stmt = $mysqli->prepare("select guid from {$serverdb}.t_player_info where name=? limit 1");
		$stmt->bind_param('s',$uid);
		$stmt->execute();
		$result = $stmt->get_result();
		$row = $result->fetch_assoc();
		$result->close();
		$stmt->close();
		if(!$row['guid']) exit_notice('角色不存在',1);
		$guid=$row['guid'];
		$result = $mysqli->query("select * from cdks.cdk where qid={$serverid} and uid='{$guid}' and status=1 limit 1");
		$row = $result->fetch_assoc();
		$result->close();
		if(!$row['cdk']) exit_notice('角色未授权',1);
		if($row['pass'] !== $pswd) exit_notice('授权密码错误',1);
		$max=999;
		$itemid=trim($_POST['itemid']);	
		$itemnum=intval($_POST['itemnum']);
		if($itemid==200040518) exit_notice('物品错误',1);
		if($itemid==200040519) exit_notice('物品错误',1);
		if($itemid==200040520) exit_notice('物品错误',1);
		if($itemid==200021118) exit_notice('物品错误',1);
		if($itemid==200021119) exit_notice('物品错误',1);
		if($itemid==200021120) exit_notice('物品错误',1);		
		if($itemnum==0 || $itemnum>$max) exit_notice('物品数量错误',1);
		$cookie=login();
		if(!$cookie) exit_notice('后台登陆失败',1);
        $post=array(
			'GM'=>$gmcode,
			'data'=>array(
				'server'=>$serverid,
				'role'=>'',
				'roles'=>base64_encode($guid),
				'content'=>'尊贵的后台权限用户，这是您后台发送的物品！',
				'layTableCheckbox'=>'on',
				'__token__'=>'',
			),
			'attach_item'=>array(
				array(
					'title'=>'',
					'itemd'=>$itemid,
					'number'=>$itemnum
				)
			)
		);
		$headers=array(
			'Cookie:'. $cookie
		);
		$rtn=post($WEB['WEB_BASE'].'game/newmailformgmd',$post,$headers);
		$rtn=json_decode($rtn,true);
		if($rtn['code']===0 && $rtn['msg']===true){
			exit_notice('邮件发送成功',0);
		}else{
			exit_notice('邮件发送失败',1);
		}
		break;
	default:
		exit_notice('不支持的操作',1);
}