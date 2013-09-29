#!/usr/bin/env ruby
puts '=== Interruptable mode ==='
require 'myext'
include MyExt
def test1(i)
  myext1 i
end

def test2
  myext3
end

require 'test'
