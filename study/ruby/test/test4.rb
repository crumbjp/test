#!/usr/bin/env ruby
puts '=== EventMachine mode ==='
def test1(i)
  puts 'test1 ('+i.to_s+') start'
  sleep 1
  puts 'test1 end'
end

require 'eventmachine'
class Echo < EM::Connection
  def receive_data(data)
    send_data(data)
  end
end
def test2
  puts 'TEST2 START'
  EM.run {
    EM.add_timer(3) {
      EM.stop
    }
    EM.start_server("0.0.0.0", 9999, Echo)
  }
  puts 'TEST2 END       **** HERE ****'
end

require 'test'
