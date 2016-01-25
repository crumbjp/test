_ = require 'underscore'
coffee2string = require './coffee2string'

class Job
  @member =
    key: 'val'

  constructor: (arg)->
    @val = arg

  proc: ()->
    console.log "Job::#{@val}"
  run: ()->
    @proc()

class Foo extends Job
  constructor: (arg)->
    super arg
  proc: ()->
    console.log "Foo::#{@val}"

job = new Foo 'foo'
job.run()
# Foo::foo

console.log Foo
# Foo

console.log Foo.prototype.constructor.name

console.log Foo.prototype
for k, v of Foo.prototype
  console.log k
# { constructor:
#   { [Function: Foo]
#     member: { key: 'val' },
#     __super__: { proc: [Function], run: [Function] } },
#  proc: [Function] }
#

console.log(typeof Foo)

console.log Foo.toString()
# function Foo(arg) {
#   Foo.__super__.constructor.call(this, arg);
# }

console.log _.keys(Foo)
# [ 'member', '__super__' ]

console.log Foo.__super__.constructor
# { [Function: Job] member: { key: 'val' } }

console.log _.keys(Foo.__super__)
# 'proc', 'run'

console.log Foo.__super__.proc.toString()
# function () {
#   return console.log("Job::" + this.val);
# }

console.log _.keys(Foo.__super__)

jsString = coffee2string Foo
console.log jsString

# eval(jsString)
# foo = new Foo 'foo'
# foo.run()
#  Foo::foo
