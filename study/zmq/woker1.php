<?php
$context = new ZMQContext();
$worker  = $context->getSocket(ZMQ::SOCKET_XREP);//自分の名前を指定
$worker->setSockOpt(ZMQ::SOCKOPT_IDENTITY,"WORKER1");
$worker->connect("tcp://127.0.0.1:4444");
sleep(1);

$from = null;
$to = null;
while ( true ) {
  $r = $worker->recv();
  if ( $worker->getSockOpt(ZMQ::SOCKOPT_RCVMORE) ) {
    if ( ! $from )  // 先頭パート：XREPでは送信元のIDが追加される。
      $from=$r;
    if ( ! $to )    // 第２パート：クライアント側で指定したIDENTITY
      $to=$r;
    continue;
  }
  var_dump($r);     // 第３パート：メッセージパート
  $worker->send($from,ZMQ::MODE_SNDMORE); // 先頭パート：XREPでは送信先を指定
  $worker->send("WORKER1 REPLY : $r");    // 第２パート：メッセージパート
}
