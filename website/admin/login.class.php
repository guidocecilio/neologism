<?php
/*
**	@desc:		PHP ajax login form using jQuery
**	@author:	programmer@chazzuka.com
**	@url:		http://www.chazzuka.com/blog
**	@date:		15 August 2008
**	@license:	Free!, but i'll be glad if i my name listed in the credits'
*/
//@ validate inclusion
if(!defined('VALID_ACL_')) exit('direct access is not allowed.');

class Authorization {
  
	public function check_status() {
			if(empty($_SESSION['exp_user']) || @$_SESSION['exp_user']['expires'] < time()) {
					return false;
			}
			else {
					return true;
			}
	}
		
	public function	form() {
			global $ACL_LANG;
			$htmlForm =	'<form id="frmlogin">'.
						'<label>';
			switch(LOGIN_METHOD) {
				case 'both':
					$htmlForm .= $ACL_LANG['USERNAME'].'/'.$ACL_LANG['EMAIL'];
					break;
				case 'email':
					$htmlForm .= $ACL_LANG['EMAIL'];
					break;
				default:
					$htmlForm .= $ACL_LANG['USERNAME'];
					break;
			}						
			$htmlForm .= ':</label>'.
						 '<input type="text" name="u" id="u" class="textfield" />'.
						 '<label>'.$ACL_LANG['PASSWORD'].'</label>'.
						 '<input type="password" name="p" id="p" class="textfield" />'.
						 '<input type="submit" name="btn" id="btn" class="buttonfield" value="'.$ACL_LANG['LOGIN'].'" />'.
						 '</form>';
			return $htmlForm;
	}
		
	public function signin($u,$p)	{
			global $db_config,$user_config;
			$return = false;
			
			if ($u && $p) {
					$this->db = @mysql_connect($db_config['server'], $db_config['user'], $db_config['pass']);
					if (!$this->db) return false;
					
					$opendb = @mysql_select_db($db_config['name'], $this->db);
					if (!$opendb) return false;
					
					$sql = "SELECT * FROM ".$db_config['users']." WHERE ";
					switch (LOGIN_METHOD) {
							case 'both':
								$sql .= "(username='".mysql_real_escape_string($u)."' OR useremail='".mysql_real_escape_string($u)."')";
								break;
							case 'email':
								$sql .= "useremail='".mysql_real_escape_string($u)."'";
								break;
							default:
								$sql .= "username='".mysql_real_escape_string($u)."'";
								break;
					}
					$sql .= " AND userpassword = '".md5($p)."'";
									
					$rs = @mysql_query($sql,$this->db);
					
					if (!$rs) return false;
					
					if (mysql_num_rows($rs))	{
							$this->set_session(array_merge(mysql_fetch_assoc($rs),array('expires'=>time()+(45*60))));
							$return = true;
					}
					mysql_free_result($rs);
					mysql_close($this->db);
					unset($rs,$sql);
			}
				
			return $return;		
	}

	private function set_session($a = false) {
			if (!empty($a)) {
					$_SESSION['exp_user'] = $a;
			}
	}
}
?>