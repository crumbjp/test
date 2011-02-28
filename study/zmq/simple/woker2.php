<?php
$context = new ZMQContext();
$worker  = $context->getSocket(ZMQ::SOCKET_XREP);
$worker->setSockOpt(ZMQ::SOCKOPT_IDENTITY,"WORKER2"); // 名前はWORKER2
$worker->connect("tcp://127.0.0.1:4444");
sleep(1);

$recvMsg = array();
while ( true ) {
  $r = $worker->recv();
  $recvMsg[]=$r;
  if ( $worker->getSockOpt(ZMQ::SOCKOPT_RCVMORE) ) {
    continue;
  }
  $msg = array_pop($recvMsg);
  var_dump($msg);
  foreach($recvMsg as $head){
    $worker->send($head,ZMQ::MODE_SNDMORE);
  }
  $worker->send("WORKER2 REPLY : $r");    // メッセージもWORKER2
  $recvMsg = array(); // リセット
}
