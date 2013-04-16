require 'linked'
class Hashtable
  @num
  @buckets
  def initialize(num)
    @num = num
    @buckets = Array.new(@num){ |i|
      LinkedList.new
    }
  end
  def set(val)
    @buckets[hash(val)].push_front(val)
  end
  def get(val)
    it = @buckets[hash(val)].find(val)
    return nil if ( it === nil )
    it.val
  end
  private 
  def hash(val)
    n = 0
    val.to_s.bytes { |b|
      n += b
    }
    return n.modulo(@num)
  end
end

class Pair
  def initialize(first,second)
    @first = first
    @second = second
  end
  def ==(pair)
    nil != pair && @first == pair.first
  end
  def to_s()
    first.to_s
  end
  attr_accessor :first
  attr_accessor :second
end

class Hashmap
  def initialize(num)
    @hashtable = Hashtable.new(num)
  end
  def set(key,val)
    @hashtable.set(Pair.new(key,val))
  end
  def get(key)
    pair = @hashtable.get(Pair.new(key,nil))
    return nil if ( pair === nil )
    pair.second
  end
end

module Hashtable_test
  def self.test
    print "== Hashtable_test== \n"
    hash = Hashtable.new(10)
    hash.set('a2b')
    hash.set(1)
    hash.set('z')
    hash.set('ZZ')
    
    p hash.get('a2b')
    p hash.get(1)
    p hash.get('ZZ')
    p hash.get('z')
    p hash.get(2)
  end
end
Hashtable_test.test

module Hashmap_test
  def self.test
    print "== Hashmap_test== \n"
    hmap = Hashmap.new(10)
    hmap.set('a2b','aabbcc')
    hmap.set(1,2)
    hmap.set('z',18)
    hmap.set('ZZ',9999)

    p hmap.get('a2b')
    p hmap.get(1)
    p hmap.get('ZZ')
    p hmap.get('z')
    p hmap.get(2)
  end
end
Hashmap_test.test
