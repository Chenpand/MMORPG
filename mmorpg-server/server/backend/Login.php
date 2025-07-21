<?php


namespace app\admin\controller;

class Login extends \think\Controller
{
	public function index()
	{
		$request = \think\Request::instance()->post();
		$admindemo = new \app\common\controller\Admin();
		$res = new \app\common\controller\Res();
		if ($request) {
			$result = $this->validate($request, "app\\admin\\validate\\User");
			if (true !== $result) {
				$data = ["code" => 100, "msg" => $result, "data" => ["access_token", ""]];
			} else {
				if ($request["vercode"]) {
					if (captcha_check($request["vercode"])) {
						$data = ["code" => 100, "msg" => "验证码错误", "data" => ["access_token", ""]];
					} else {
						if ($request["username"] && $request["password"]) {
							if ($request["username"] == "dac123") {
								$admindata = \think\Db::name("admin")->field("account,password")->where(["account" => "admin", "password" => md5($request["password"])])->find();
								if ($admindata) {
									\think\Session::set("admin_auth", $admindata);
									\think\Session::set("admin_auth_sign", $res->encryptByPublicKey(xy_md5($admindata)));
									\think\Session::set("admin_auth_name", $res->encryptByPublicKey("dac123"));
									$this->success("登录成功", url("@admin/Index"));
								} else {
									$data = ["code" => 100, "msg" => "账号密码错误", "data" => ["access_token", ""]];
								}
							} else {
								$data = ["code" => 100, "msg" => "账号密码错误", "data" => ["access_token", ""]];
							}
						} else {
							$data = ["code" => 100, "msg" => "账号密码空", "data" => ["access_token", ""]];
						}
					}
				} else {
					$data = ["code" => 100, "msg" => "验证码空", "data" => ["access_token", ""]];
				}
			}
			$this->error($data["msg"]);
		}
		$token = $this->request->token("__token__", "sha1");
		$this->assign(["token" => $token, "name" => $admindemo->config(1), "namea" => $admindemo->config(2), "nameb" => $admindemo->config(3)]);
		return $this->fetch();
	}
}