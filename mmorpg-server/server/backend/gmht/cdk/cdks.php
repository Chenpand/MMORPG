<?php
error_reporting(0);
$sqm = $_POST['sqm'];
$num = $_POST['num'];
include "../config/config.php";
$zame = 'PAY_';
$sqm != $d_gmrz && (die("<script>alert('授权码错误');window.history.back(-1); </script>")); 
$num > 100 && (die("<script>alert('单次最多生成100条');window.history.back(-1); </script>")); 
$num == '' && ($num = 1);
$mysql = mysqli_connect($PZ['DB_HOST'],$PZ['DB_USER'],$PZ['DB_PWD'],$PZ['DB_NAME'],$PZ['DB_PORT']) or die("<script>alert('数据库连接失败');window.history.back(-1); </script>");
for($i=1;$i<=$num;$i++){
	$cdk = cdkey('aldddd');
	$txt .= $cdk.PHP_EOL;
	if(!$mysql->query("INSERT INTO cdks.cdk (cdk) VALUES ('$cdk');")){die("<script>alert('生成失败,请查看数据库连接是否正常');window.history.back(-1); </script>");}
}
$ts = time().'.txt';
file_put_contents('cdks/'.$zame.$ts,$txt);
Header ( "Content-type: application/octet-stream" );
Header ( "Accept-Ranges: bytes" );
Header ( "Content-Disposition: attachment; filename=".$zame.$ts);
die($txt);

function cdkey($namespace = null) {  
    static $guid = '';  
    $uid = uniqid ( "", true );  
    $data = $namespace;  
    $data .= $_SERVER ['REQUEST_TIME']; 
    $data .= $_SERVER ['HTTP_USER_AGENT'];
    $data .= $_SERVER ['SERVER_ADDR'];
    $data .= $_SERVER ['SERVER_PORT'];
    $data .= $_SERVER ['REMOTE_ADDR'];
    $data .= $_SERVER ['REMOTE_PORT'];
    $hash = strtoupper (substr(md5($uid.$data), 8, 16));
    $guid = substr ( $hash, 0, 4 ) . '-' . substr ( $hash, 4, 4 ) . '-' . substr ( $hash, 8, 4 ) . '-' . substr ( $hash, 12, 4 );  
    return $guid;  
} 
?>