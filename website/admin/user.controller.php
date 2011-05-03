<?php
require_once('current_user.model.php');

class UserController {
  
  public function __construct() {  
    //$this->model = new Model();  
  }   
  
  public function index() {
    if ( Current_User::user() ) {
      if (!empty($_GET['logoff'])) { 
        $this->logout();
        header("location:login.html");
      }
      else {
			  include('admin_registration.view.php');
			  // @ session already active
//		    $arr = array('status' => true, 'message' => $ACL_LANG['SESSION_ACTIVE'], 'url' => SUCCESS_URL);
//        $json_arr = json_encode($arr);
//        echo $json_arr;
      }
  	}  
  	// no user is currently logged in, so the controller use the reader section as restricted as possible
  	else {
  	  // load data to the view if there are some data needed
    	//include('login.view.php');
  	
    	//@ session not active
		  if ($_SERVER['REQUEST_METHOD']=='GET')	{
				//@ first load
				$arr = array('status' => false, 'message' => $acl->form());
        $json_arr = json_encode($arr);
        echo $json_arr;
		  }
		  else {
				//@ form submission
				$u = (!empty($_POST['u'])) ? trim($_POST['u']) : false;	// retrive user var
				$p = (!empty($_POST['p'])) ? trim($_POST['p']) : false;	// retrive password var
				
				// @ try to signin
				$is_auth = Current_User::authenticate($u, $p);
				
				if ($is_auth) {
						//@ success
						$arr = array('status' => true, 'message' => $ACL_LANG['LOGIN_SUCCESS'], 'url' => SUCCESS_URL);
            $json_arr = json_encode($arr);
            echo $json_arr;
			  }
				else {
						//@ failed
						$arr = array('status' => false, 'message' => $ACL_LANG['LOGIN_FAILED']);
            $json_arr = json_encode($arr);
            echo $json_arr;
				}
		  }
    }
  }
  
  public function logout() {
    session_destroy();
  	//$this->session->sess_destroy();
    //echo "{success:true}";
  }
}