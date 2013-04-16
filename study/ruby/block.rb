class Block
  def initialize(num)
    @a = Array.new
    (0..num).each { |i|
      @a[i] = i*10
    }
  end
  def test (offset,&block)
    if block_given?
      @a[offset..-1].each_with_index { |v,i|
        block.call @a[i+offset]
      }
    else
      return @a[offset..-1]
    end  
  end
end

b = Block.new 10
b.test(5) {
  |i|
  p i
}
a = b.test(5)
p a

