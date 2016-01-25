# CoffeeをRPCしたい

## class実装はどうなっているのか？
```
_ = require 'underscore'

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
```

### classの名前がそのまま関数名になってる

`console.log(typeof Foo)`

```
function
```

`console.log Foo.name`

```
Foo
```

### 実装

`console.log Foo`

```
{
  [Function: Foo]
  member: { key: 'val' },
  __super__: { proc: [Function], run: [Function] }
}
```

### 関数ならばtoString()で実装が取れる

`console.log Foo.toString()`


```
function Foo(arg) {
  Foo.__super__.constructor.call(this, arg);
}
```

### `prototype.constructor`は自身つまり循環参照
`Foo.prototype.constructor.name`

```
Foo
```

### なのでprototypeがこんな感じ

`console.log Foo.prototype`

```
{
  constructor: {
    [Function: Foo]
    member: { key: 'val' },
    __super__: { proc: [Function], run: [Function] }
  },
  proc: [Function]
}
```

`__super__` は親関数そのものね。

親クラスのメンバーだけ継承してるのか。。

子クラスに`run`が無いので`__super__`経由で呼ぶようだが、、どうやって呼ぶ仕組みなのか解らんぞ？

### やっぱりね`run`あった。
```
for k, v of Foo.prototype
  console.log k
```

```
constructor
proc
run
```

コレが無いと呼べないよね。。


## やらなきゃいけない事
1. class実装
2. 循環参照問題
3. coffee特有の対応



```js
coffee2string = require './coffee2string'
jsString = coffee2string Foo
console.log jsString
```

```
var extend = function(child, parent) { for (var key in parent) { if (hasProp.call(parent, key)) child[key] = parent[key]; } function ctor() { this.constructor = child; } ctor.prototype = parent.prototype; child.prototype = new ctor(); child.__super__ = parent.prototype; return child; },
hasProp = {}.hasOwnProperty,
bind = function(fn, me){ return function(){ return fn.apply(me, arguments); }; },
indexOf = [].indexOf || function(item) { for (var i = 0, l = this.length; i < l; i++) { if (i in this && this[i] === item) return i; } return -1; };
var objects184061033651232700 = {}
objects184061033651232700[0] = {key: 'val',}

function Foo(arg) {
      Foo.__super__.constructor.call(this, arg);
    }
function Job(arg) {
      this.val = arg;
    }Job.prototype['proc'] = function () {
      return console.log("Job::" + this.val);
    }
Job.prototype['run'] = function () {
      return this.proc();
    }
Foo.__super__ = {proc: function () {
      return console.log("Job::" + this.val);
    },run: function () {
      return this.proc();
    },}
Foo.__super__.constructor = Job
Foo['member'] = objects184061033651232700[0]
Foo.prototype['constructor'] = Foo
Foo.prototype['proc'] = function () {
      return console.log("Foo::" + this.val);
    }
Foo.prototype['run'] = function () {
      return this.proc();
    }

Foo::bar
```