<?php
$context = new ZMQContext();
$worker  = $context->getSocket(ZMQ::SOCKET_XREP);
$worker->setSockOpt(ZMQ::SOCKOPT_IDENTITY,"WORKER2");
$worker->connect("tcp://127.0.0.1:4444");
sleep(1);

$from = null;
$to = null;
while ( true ) {
  $r = $worker->recv();
  if ( $worker->getSockOpt(ZMQ::SOCKOPT_RCVMORE) ) {
    if ( ! $from )
      $from=$r;
    if ( ! $to ) 
      $to=$r;
    continue;
  }
  var_dump($r);
  $worker->send($from,ZMQ::MODE_SNDMORE);
  $worker->send("WORKER2 REPLY : $r");
}
