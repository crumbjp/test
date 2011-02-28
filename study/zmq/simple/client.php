<?php
$context = new ZMQContext();
$client  = $context->getSocket(ZMQ::SOCKET_XREP);
$client->bind("tcp://127.0.0.1:4444");
sleep(1);
// WORKER1と通信
for ( $i = 0 ; $i < 10 ; $i++ ) {
  $client->send("WORKER1",ZMQ::MODE_SNDMORE); // 先頭パート：送信先
  $client->send("SEND TO WORKER1 $i");        // 第２パート：メッセージパート
  $from = $client->recv();
  if ( $client->getSockOpt(ZMQ::SOCKOPT_RCVMORE) ) {
    $r = $client->recv(); // 最終パートがメッセージなので手抜き・・・
  }
  var_dump($r);
  usleep(300000);
}
// WORKER2と通信
for ( $i = 0 ; $i < 10 ; $i++ ) {
  $client->send("WORKER2",ZMQ::MODE_SNDMORE);
  $client->send("SEND TO WORKER2 $i");
  $from = $client->recv();
  if ( $client->getSockOpt(ZMQ::SOCKOPT_RCVMORE) ) {
    $r = $client->recv();
  }
  var_dump($r);
  usleep(300000);
}
