#!/usr/bin/env ruby
puts '=== Pure ruby mode ==='
def test1(i)
  puts 'test1 ('+i.to_s+') start'
  sleep 1
  puts 'test1 end'
end

def test2
  puts 'TEST2 START'
  sleep 3
  puts 'TEST2 END       **** HERE ****'
end

require 'test'
