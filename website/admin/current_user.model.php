<?php
class Current_User {
	
	private static $user;
	
	private function __construct() {}
	
	public static function user() {
		if (!isset(self::$user)) {
			
		  if (empty($_SESSION['user']) || @$_SESSION['user']['expires'] < time()) {
				return null;
			}
			
			
			else {
			    return $_SESSION['user']['user_id'];
					//return true;
			}
			
//			if (!$user_id = Current_User::getUser()) {
//				return FALSE;
//			}
//			
//			if (!$u = Doctrine::getTable('User')->find($user_id)) {
//				return FALSE;
//			}
			
			self::$user = $u;
		}
		
		return self::$user;
	}
	
	/**
	 * Authenticate and validate the user.
	 * @param $username
	 * @param $password
	 * @return unknown_type
	 */
	public static function authenticate($username, $password) {
		global $db_config;
		$result = FALSE;
		$db = NULL;
			
			if ($username && $password) {
					$db = @mysql_connect($db_config['server'], $db_config['user'], $db_config['pass']);
					if (!$db) return FALSE;
					
					$opendb = @mysql_select_db($db_config['name'], $db);
					if (!$opendb) return FALSE;
					
					$sql = "SELECT * FROM ".$db_config['users']." WHERE ";
					switch (LOGIN_METHOD) {
							case 'both':
								$sql .= "(username='".mysql_real_escape_string($username)."' OR useremail='".mysql_real_escape_string($username)."')";
								break;
							case 'email':
								$sql .= "useremail='".mysql_real_escape_string($username)."'";
								break;
							default:
								$sql .= "username='".mysql_real_escape_string($username)."'";
								break;
					}
					$sql .= " AND userpassword = '".md5($password)."'";
									
					$rs = @mysql_query($sql, $db);
					
					if (!$rs) return FALSE;
					
					if (mysql_num_rows($rs))	{
					    $u = new stdClass();
					    //$u->user_id 
  					  self::$user = $u;
							self::setSession(array_merge(mysql_fetch_assoc($rs), array('expires'=>time()+(45*60))));
							$result = TRUE;
					}
					mysql_free_result($rs);
					mysql_close($db);
					unset($rs, $sql);
			}
				
			return $result;
			
	}
		
  static private function setSession($data = NULL) {
			if (!empty($data)) {
					$_SESSION['user'] = $data;
			}
	}
	
}
