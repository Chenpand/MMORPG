<?php
error_reporting(0);
include "../config/config.php";
if($_POST){
	$usr = $_POST['usr'];
	$quid = intval($_POST['qu']);
	$cdk = $_POST['cdk'];
	$pas = trim($_POST['pas']);
	$usr = str_replace(array(' ','%'),'',$usr);
	$cdk = str_replace(array(' ','%'),'',$cdk);
	$pas = str_replace(array(' ','%'),'',$pas);
	$cdk =='' && (die('请输入CDKEY'));
	if(!$quid) die('请先选区');
	$usr =='' && (die('请输入'.$namecol));
	$pas =='' && (die('请输入密码'));
	$mysqli = new mysqli($PZ['DB_HOST'],$PZ['DB_USER'],$PZ['DB_PWD'],$PZ['DB_NAME'],$PZ['DB_PORT']);
	if ($mysqli->connect_errno) die('数据库连接失败');
	$mysqli->set_charset($PZ['DB_CHARSET']);
	$stmt = $mysqli->prepare("select * from cdks.cdk where cdk=? limit 1");
	$stmt->bind_param('s',$cdk);
	$stmt->execute();
	$result=$stmt->get_result();
	$row=$result->fetch_assoc();
	$result->close();
	$stmt->close();
	if(!$row['cdk']) die('CDK不存在');
	if($row['status']==1) die('CDK已经被使用了');
	$result = $mysqli->query("select * from xy_gameconfig where Id={$quid} limit 1");
	$row = $result->fetch_assoc();
	$result->close();
	if(!$row) die('区服配置不存在');
	$serverid=$row['server'];
	$serverdb=$row['surface'];
	$stmt = $mysqli->prepare("select guid from {$serverdb}.t_player_info where name=? limit 1");
	$stmt->bind_param('s',$usr);
	$stmt->execute();
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	$result->close();
	$stmt->close();
	if(!$row['guid']) die('角色不存在');
	$guid=$row['guid'];
	$result = $mysqli->query("select * from cdks.cdk where uid='{$guid}' limit 1");
	$row = $result->fetch_assoc();
	$result->close();
	if($row['cdk']) die('该角色已经授权过了');
	$stmt=$mysqli->prepare('update cdks.cdk set qid=?,uid=?,pass=?,status=1 where cdk=?');
	$stmt->bind_param('dsss',$serverid,$guid,$pas,$cdk);
	$stmt->execute();
	if($stmt->affected_rows){
		die($namecol.'授权成功');
	}else{
		die('授权失败');
	}
}else{
	die('不支持的操作');
}
?>