#!/usr/bin/env ruby
## -*- coding: utf-8 -*-
class Poll
  def initialize
    @arr = Array.new
  end
  def push(v)
    raise "Could not push : " + v.to_s if @arr.size > 0 and @arr.last <= v
    @arr.push(v)
  end
  def pop
    @arr.pop
  end
  attr_accessor :arr
end

class Hanoi
  def initialize(n)
    @polls = {
      'S' => Poll.new,
      'G' => Poll.new,
      'B' => Poll.new
    }
    n.times { |i|
      @polls['S'].push(n-i)
    }
    @n = 0
  end
  def dump 
    print '== ' + @n.to_s + '==' + "\n"
    print "S|"
    @polls['S'].arr.each { |e|
      print e.to_s + ' '
    }
    print "\n"
    print "G|"
    @polls['G'].arr.each { |e|
      print e.to_s + ' '
    }
    print "\n"
    print "B|"
    @polls['B'].arr.each { |e|
      print e.to_s + ' '
    }
    print "\n"
  end
  def move (from,to)
    @polls[to].push(@polls[from].pop)
    @n += 1
  end
  def hanoi (n,from,to,buffer)
    return move(from,to) if n === 1
    hanoi(n-1,from,buffer,to)
    dump
    move(from,to)
    dump
    hanoi(n-1,buffer,to,from)
  end
  attr_accessor :n
end

N=4
hanoi = Hanoi.new(N)
hanoi.dump
hanoi.hanoi(N,'S','G','B')
hanoi.dump

class Hanoi2
  def initialize(a)
    @ALL = a
  end
  def hanoi2(n,from,to,buffer)
    # print " #{@ALL} ( #{n} ) =>  from : #{from} to : #{to}  \n"
    (1..n).each { |i|
      if ( (2**i - 1 ) >= n )
        nx =  n - (2**(i-1) - 1 )
        if ( ((@ALL + i) % 2) === 1 ) 
          return [from,buffer,i] if ( nx === 1 )
          return Hanoi2.new(i-1).hanoi2(nx-1,to,buffer,from)
        else
          return [from,to,i] if ( nx === 1 )
          return Hanoi2.new(i-1).hanoi2(nx-1,buffer,to,from)
        end
      end
    }
  end
end

p hanoi2 = Hanoi2.new(5).hanoi2(1,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(2,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(3,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(4,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(5,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(6,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(7,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(8,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(9,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(10,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(11,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(12,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(13,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(14,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(15,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(16,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(28,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(29,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(30,'S','G','B')
p hanoi2 = Hanoi2.new(5).hanoi2(31,'S','G','B')
#p hanoi2 = Hanoi2.new(10).hanoi2(700,'S','G','B')
#p hanoi2 = Hanoi2.new(10000).hanoi2(100000000000000000000,'S','G','B')
 


