<?php
// クライアント側ソケット
$side_client = new ZMQSocket(new ZMQContext(), ZMQ::SOCKET_XREP);
$side_client->setSockOpt(ZMQ::SOCKOPT_IDENTITY,"WORKER1"); // 僕がWORKER1です。
$side_client = $side_client->connect("tcp://127.0.0.1:4444"); 
// ワーカー側ソケット(RRバランシング)
$side_worker = new ZMQSocket(new ZMQContext(), ZMQ::SOCKET_XREQ);
$side_worker = $side_worker->bind("ipc:///tmp/worker1");

sleep(1);
$dev = new ZMQDevice(ZMQ::DEVICE_QUEUE,$side_client,$side_worker);
