<?php
$context = new ZMQContext();
$worker  = $context->getSocket(ZMQ::SOCKET_XREP);
$worker->setSockOpt(ZMQ::SOCKOPT_IDENTITY,"WORKER1");//自分の名前を指定
$worker->connect("tcp://127.0.0.1:4444");
sleep(1);

$recvMsg = array();
while ( true ) {
  $r = $worker->recv();
  var_dump($r);
  // 複数ホップしてくるとメッセージフォーマットがネストしてくるので真面目に書き直す。
  $recvMsg[]=$r;
  if ( $worker->getSockOpt(ZMQ::SOCKOPT_RCVMORE) ) {
    continue;
  }
  // 最終パート：メッセージパート
  //  メッセージパートを除いて、ヘッダのみを返却用に残しておく
  $msg = array_pop($recvMsg);
  var_dump($msg);
  // ネストしたメッセージフォーマットを返す。
  foreach($recvMsg as $head){
    $worker->send($head,ZMQ::MODE_SNDMORE); // 先頭パート：XREPでは送信先を指定
  }
  $worker->send("WORKER1 REPLY : $r");    // 最終パート：メッセージパート
  $recvMsg = array(); // リセット
}
