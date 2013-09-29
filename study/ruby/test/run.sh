#!/usr/bin/env bash
ruby make.rb
make

ruby test1.rb
ruby test2.rb
ruby test3.rb
ruby test4.rb
ruby test5.rb

exit 0

Results 


=== Pure ruby mode ===
test1 (0) start
test1 end
test1 (1) start
TEST2 START
test1 end
test1 (2) start
test1 end
test1 (3) start
TEST2 END       **** HERE ****
test1 end
test1 (4) start
test1 end
=== Extension mode ===
test1 (0) start
test1 end
test1 (1) start
test1 end
test1 (2) start
test1 end
TEST2 START
TEST2 END       **** HERE ****
test1 (3) start
test1 end
test1 (4) start
test1 end
=== Complex mode ===
test1 (0) start
test1 end
test1 (1) start
TEST2 START
TEST2 END       **** HERE ****
test1 end
test1 (2) start
test1 end
test1 (3) start
test1 end
test1 (4) start
test1 end
=== EventMachine mode ===
test1 (0) start
test1 end
test1 (1) start
TEST2 START
test1 end
test1 (2) start
test1 end
test1 (3) start
test1 end
test1 (4) start
TEST2 END       **** HERE ****
test1 end

