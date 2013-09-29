begin
  t1 = Thread.new {
    Thread.stop
    for i in [0,1,2,3,4]
      test1 i
    end
  }
  Thread.pass
  t2 = Thread.new {
    Thread.stop
    test2
  }
  Thread.pass
  t1.run # Thread1 start 
  sleep 1 
  t2.run # Thread2 start 
  t1.join
  t2.join
rescue
  p $!
end

