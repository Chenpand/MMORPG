<?php
error_reporting(E_ERROR | E_PARSE);
$PZ = array(
	'DB_HOST'=>'127.0.0.1',// 服务器地址
	'DB_NAME'=>'demoald',// 游戏数据库
	'DB_USER'=>'root',// 用户名
	'DB_PWD'=>'912sy.com',// 密码
	'DB_PORT'=>'3306',// 端口
	'DB_CHARSET'=>'utf8',// 数据库字符集
);
$WEB=array(
	'WEB_BASE'=>'http://42.194.135.168/admin/',
	'WEB_ADMIN'=>'dac123',
	'WEB_PSWD'=>'dac123'
);
$gamename='阿拉德';
$namecol='角色名';
$copyright='版权信息';
$d_gmrz = '912sy.com';//gm认证码
/*以下函数用于替换exit*/
function exit_notice($message,$errno,$other=array()){
	if(is_array($message)) $message =json_encode($message,JSON_UNESCAPED_UNICODE);
	$rtn['message']=$message;
	$rtn['errno']=$errno;
	if(count($other)>0){
		foreach($other as $k=>$v){
			$rtn[$k]=$v;
		}
	}
	exit(json_encode($rtn,JSON_UNESCAPED_UNICODE));
}

	function get($url){
		$ch = curl_init(); 
		curl_setopt($ch, CURLOPT_URL, $url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 
		curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE); 
		curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE); 
		curl_setopt($ch, CURLOPT_HEADER, 1); 
		curl_setopt($ch, CURLOPT_TIMEOUT, 10);
		$output = curl_exec($ch);
		$errorCode = curl_errno($ch);
		curl_close($ch);
		if(0 !== $errorCode){
			return false;
		}
		return $output;
	}

	function post($url,$postdata,$headers=array()){
		$ch = curl_init(); 
		curl_setopt($ch, CURLOPT_URL, $url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 
		curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE); 
		curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE); 
		curl_setopt($ch, CURLOPT_HEADER, 0);
		curl_setopt($ch, CURLOPT_POST, 1);  
		if(sizeof($headers)>0){ 
			curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
		}
		curl_setopt($ch, CURLOPT_POSTFIELDS, http_build_query($postdata)); 
		curl_setopt($ch, CURLOPT_TIMEOUT, 10);
		$output = curl_exec($ch);
		$errorCode = curl_errno($ch);
		curl_close($ch);
		if(0 !== $errorCode){
			return false;
		}
		return $output;
	}

	function login(){
		global $WEB;
		$re=get($WEB['WEB_BASE'].'login.html');
		if($re){
			list($header, $body) = explode("\r\n\r\n", $re);
			preg_match("/name\=\"\_\_token\_\_\" value\=\"([^\"]*)/i", $re, $matches);
			$token=$matches[1];
			preg_match("/set\-cookie:([^\r\n]*)/i", $header, $matches);
			$cookie = trim(explode(';', $matches[1])[0]);
			$post=array(
				'__token__'=>$token,
				'username'=>$WEB['WEB_ADMIN'],
				'password'=>$WEB['WEB_PSWD'],
				'vercode'=>'abc'
			);
			$headers=array(
				'Cookie:'. $cookie,
				'Content-Type: application/x-www-form-urlencoded; charset=utf-8'
			);
			$re=post($WEB['WEB_BASE'].'Login',$post,$headers);
			if(strpos($re,'登录成功')){
				return $cookie;
			}else{
				return '';
			}
		}else{
			return '';
		}
	}