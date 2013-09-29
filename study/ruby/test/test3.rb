#!/usr/bin/env ruby
puts '=== Complex mode ==='
def test1(i)
  puts 'test1 ('+i.to_s+') start'
  sleep 1
  puts 'test1 end'
end

require 'myext'
include MyExt

def test2
  myext2
end

require 'test'
