<?php
// クライアント側ソケット
$context = new ZMQContext();
$side_client  = $context->getSocket(ZMQ::SOCKET_XREQ);
$side_client->bind("ipc:///tmp/client");
sleep(1);
// ワーカー側ソケット(RRバランシング)
$side_worker = new ZMQSocket(new ZMQContext(), ZMQ::SOCKET_XREP);
$side_worker = $side_worker->bind("tcp://127.0.0.1:4444");

sleep(1);
$dev = new ZMQDevice(ZMQ::DEVICE_QUEUE,$side_client,$side_worker);
