<?php
$context = new ZMQContext();
$worker  = $context->getSocket(ZMQ::SOCKET_XREP);
$worker->connect("ipc:///tmp/worker2");
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
    $worker->send($head,ZMQ::MODE_SNDMORE)
  }
  $worker->send("WORKER2 REPLY : $r");
  $recvMsg = array();
}
