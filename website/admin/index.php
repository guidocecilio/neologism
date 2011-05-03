<?php
/*
**	@desc:	PHP ajax login form using jQuery
**	@author:	programmer@chazzuka.com
**	@url:		http://www.chazzuka.com/blog
**	@date:	15 August 2008
**	@license:	Free!, but i'll be glad if i my name listed in the credits'
*/

// @ error reporting setting  (  modify as needed )
ini_set("display_errors", 1);
error_reporting(E_ALL);

//@ explicity start session  ( remove if needless )
session_start();

require_once('includes/admin.config.php');
require_once('includes/admin.lang.php');
require_once('user.controller.php');

$controller = new UserController();

// this is the execution point for the controller, sometime the function is called invoke()
$controller->index();

////@ if logoff
//if(!empty($_GET['logoff'])) { $_SESSION = array(); }
//
////@ is authorized?
//if(empty($_SESSION['exp_user']) || @$_SESSION['exp_user']['expires'] < time()) {
//	header("location:login.html");	//@ redirect 
//} else {
//	$_SESSION['exp_user']['expires'] = time()+(45*60);	//@ renew 45 minutes
//}	
unset($controller);
?>

