<!DOCTYPE HTML>
<html lang="ja">
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<meta http-equiv="content-style-type" content="text/css">
<meta http-equiv="content-script-type" content="text/javascript">
<meta name="description" content="">
<title>????</title>
<link rel="stylesheet" type="text/css" media="" href="http://">
</head>
<body>
<pre id="dump">
</pre>
</body>
</html>
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js"></script>
<script language=javascript>
<!--

// 経路探索コード
//  １．配達場所の追加、削除は cost_matrixをメンテすること。
//　２．search_cost()はスタート地点に戻ってくる事を想定している。
//　３．スタート地点を変更（'2'からスタート）する場合は、search_cost({r:['2'],c:0})の様に呼ぶ事。
//  ４．変数cutsは枝刈り用のログを入れている。メモリ使用量と探索数のトレードオフ。
//　　　　現状の効果は 326 => 130
/*
var cost_matrix = [
[ 0, 7, 12, 8, 11, 7 ],
[ 3, 0, 10, 7, 13, 2 ],
[ 4, 8, 0, 9, 12, 3 ],
[ 6, 6, 9, 0, 10, 7 ],
[ 7, 7, 11, 10, 0, 5 ],
[ 9, 7, 8, 9, 10, 0 ],
]
*/
// （問題で与えられたマトリクスが汚いので）扱い易い形に変形
var cost_matrix = {
  1:{      2:7, 3:12, 4:8, 5:11, 6:7 },
  2:{ 1:3,      3:10, 4:7, 5:13, 6:2 },
  3:{ 1:4, 2:8,       4:9, 5:12, 6:3 },
  4:{ 1:6, 2:6, 3:9,       5:10, 6:7 },
  5:{ 1:7, 2:7, 3:11, 4:10,      6:5 },
  6:{ 1:9, 2:7, 3:8,  4:9, 5:10      }
};

var n_matrix = 0;
for ( var i in cost_matrix ) {
  n_matrix++;
}

var n_try = 0; // チューニング用・目視データ
var cuts = {}; // 枝刈り用ログ
//----------------
// 再帰探索
//  log => { 
//       r : [], // ルート
//       c : 0  // コスト
//  }
function search_cost(log){
  n_try++;
  //--------------
  // Sub functions
  function copy ( data ) {
    return JSON.parse(JSON.stringify(data));
  }

  function is_end(log) {
    if ( log.r.length === n_matrix ) {
      return true;
    }
    return false;
  }
  
  function get_last(arr){
    var ret = arr.pop();
    arr.push(ret);
    return ret;
  }

  //--------------
  // logの最後から現在地点を得てmatrix上のコストマップ特定する
  var pos = get_last(log.r);
  var pos_matrix = copy(cost_matrix[pos]);

  // 再帰終了条件：
  // 　全ルート配り終わったら終了　＝＞　ゴール＝スタート地点("1")に戻る
  if ( is_end(log) ) {
    var goal = log.r[0];
    log.c += pos_matrix[goal];
    log.r.push(goal);
    return log;
  }

  // 既に通った地点は探索候補から外す
  for ( var i in log.r ) {
    delete(pos_matrix[log.r[i]]);
  }

  // 再帰探索し、最少コストを探す。
  var ret = { c: Number.MAX_VALUE }; 
  for ( var i in pos_matrix ) {
    var cur_log = copy(log);
    cur_log.c += pos_matrix[i];
    cur_log.r.push(i);
    // 『同一組み合わせの地点を通り現在地も同じ』の最少コストも同時に記録し無駄な探索を削る
    var cut_key = copy(cur_log.r).sort()+':'+i;
    if ( cuts[cut_key] && cuts[cut_key] <= cur_log.c ) {
      // console.log(cut_key + ' : ' + cuts[cut_key] + ' <= ' + cur_log.c);
      continue;
    }
    cuts[cut_key] = cur_log.c;

    var r = search_cost(cur_log);
    if (r.c <= ret.c ) {
      ret = r;
    }
  }
  return ret;
}

// 探索開始：("1"地点から始まる)
var ret = search_cost({r:['1'],c:0});
var ans = ret.c + '\n' + ret.r.toString().split(',').join(' -> ');
console.log(ans);
console.log(ret);
console.log(n_try);
$('#dump').text(ans);

-->
</script>
