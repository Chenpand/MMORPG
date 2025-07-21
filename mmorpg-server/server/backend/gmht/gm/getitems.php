<?php
include '../config/config.php';
$quarr=array();
$mysqli=new mysqli($PZ['DB_HOST'],$PZ['DB_USER'],$PZ['DB_PWD'],$PZ['DB_NAME'],$PZ['DB_PORT']);
if(!$mysqli->errno){
	$mysqli->set_charset($PZ['DB_CHARSET']);
	$query=$mysqli->query('select Id as id,name from xy_gameconfig where state=1');
	while($row=$query->fetch_assoc()) array_push($quarr,$row);
}
$itemarr= json_decode(file_get_contents('ItemTable.json'),true);
exit(json_encode(array('items'=>$itemarr,'qus'=>$quarr,'gamename'=>$gamename,'namecol'=>$namecol,'copyright'=>$copyright),JSON_UNESCAPED_UNICODE));