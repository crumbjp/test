#!/usr/bin/env ruby
puts '=== Extension mode ==='
require 'myext'
include MyExt
def test1(i)
  myext1 i
end

def test2
  myext2
end

require 'test'
