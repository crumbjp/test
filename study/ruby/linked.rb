class Value 
  def initialize(val,nx=nil)
    @val = val
    @next = nx
  end
  def set_next(nx)
    @next = nx
  end
  attr_accessor :val
  attr_accessor :next
end
class Iterator
  def initialize(value)
    @value = value
  end
  def next()
    return nil if @value.next === nil
    @value = @value.next
    self
  end
  def val()
    return nil if @value === nil
    @value.val
  end
  attr_accessor :value
end
class LinkedList
  def initialize
    @root = Value.new(nil)
  end
  def first()
    Iterator.new(@root.next)
  end
  def tail()
    Iterator.new(_tail)
  end
  def find(val)
    cur = @root
    while cur.next != nil
      cur = cur.next
      return Iterator.new(cur) if ( cur.val == val )
    end
    Iterator.new(nil)
  end
  def push_front(val)
    _insert(@root,val)
  end
  def push_end(val)
    _tail.set_next(Value.new(val))
  end
  def insert(it,val)
    return nil if it === nil
    _insert(it.value,val)
  end
  def delete(it)
    return nil if it === nil
    _find_next(it.value).set_next(it.value.next)
  end
  private
  def _tail()
    cur = @root
    while cur.next != nil
      cur = cur.next
    end
    cur
  end
  def _insert(t,val)
    t.set_next(Value.new(val,t.next))
  end
  def _find_next(value)
    cur = @root
    while cur.next != nil
      return cur if ( cur.next == value )
      cur = cur.next
    end
    nil
  end
end

module LinkedList_test
  def self.test
    print "== LinkList_test== \n"
    list = LinkedList.new()
    list.push_end(6)             # 6
    list.push_end(4)             # 6 4        
    list.push_end(2)             # 6 4 2      
    list.push_end(8)             # 6 4 2 8    
    list.push_front(9)           # 9 6 4 2 8  
    list.insert(list.find(6),7)  # 9 6 7 4 2 8
    list.delete(list.find(6))    # 9 7 4 2 8  
    f = list.first
    p f.val
    while f.next
      p f.val
    end
  end
end
LinkedList_test.test
