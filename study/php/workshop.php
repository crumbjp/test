<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html lang="ja">
<body>

<?php
 /*
  * Q: Implement signup and login logics.
  *    And call follow functions.
  *    - login_success_html
  *    - login_failed_html
  *    - signup_success_html
  *    - signup_failed_html
  *
  * ORDER
  *  - As beautiful and simple code as possible !!!
  *  - You must save the hash instead of the raw password.
  *  - You can use md5() but there is a possibility using other algorithm in the future.
  *  - You can save the account-data as a file but there is a possiblity ...
  *  - You can use the time less than 2 hours. ( less than 1 hour if you can do )
  *
  */

//----------------------------------------
// Dispatcher
$op =  $_GET['op'];
if ( ! $op ) {
  login_html();
}elseif ( $op === 'resist' ) {
  signup_html();
}elseif ( $op === 'login' ) {
  // TODO: Implement

}elseif ( $op === 'signup' ) {
  // TODO: Implement !!

}

//----------------------------------------
// Logic zone


//----------------------------------------
// HTML contents
function login_success_html($name){
   echo <<<_EOF_
<b>LOGIN success</b><br>
_EOF_;
   echo 'Welcome ' . $name;
}
function login_failed_html($msg){
   echo <<<_EOF_
<b>LOGIN failed</b><br>
_EOF_;
   echo $msg;
}
function signup_success_html($name){
   echo <<<_EOF_
<b>SIGNUP success</b><br>
_EOF_;
   echo $name . ' Joined';
}
function signup_failed_html($msg){
   echo <<<_EOF_
<b>SIGNUP failed</b><br>
_EOF_;
   echo $msg;
}
function login_html(){
   echo <<<_EOF_
<b>LOGIN</b>
<form method="GET" action="" >
<pre>
  <a href="?op=resist">signup</a><br>
  Name     <input type="text"     name="name"  ></input>
  Password <input type="password" name="passwd"></input>
           <input type="submit"   name="op" value="login"></input>
</pre>
</form>
_EOF_;

}
function signup_html(){
   echo <<<_EOF_
<b>SIGNUP</b>
<form method="GET" action="" >
<pre>
  <a href="?op=">login</a><br>
  Name     <input type="text"     name="name"  ></input>
  Password <input type="password" name="passwd"></input>
  Confirm  <input type="password"  name="confirm"></input>
           <input type="submit"   name="op" value="signup"></input>
</pre>
</form>
_EOF_;
}
?>

</body>
</html>
