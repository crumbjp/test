#!/usr/bin/env node
// Install these packages.
//   npm install getopt
//   npm install jsdom
var sys = require('sys');
var opt = require('getopt');
var crypto  = require('crypto');
var fs = require('fs');
var path = require('path');
var url = require('url');
//var querystring = require('querystring');
var http    = require('http');
var https   = require('https');
var jsdom   = require('jsdom').jsdom;


var LOGLV   = 10;
var CONF    = null;
var JQUERY  = 'jquery-1.4.4.js'

var URL_LEN = 65;
var SEL_LEN = 20;
var MSG_LEN = 15;

var MEM_THRESHOLD = 100*1024*1024;

var ROOT = path.dirname(process.argv[1]);
var DATA_DIR = ROOT + '/data';
process.chdir(ROOT);
try {
  fs.mkdirSync(DATA_DIR,'755');
}catch(e){
}
var md5sum = crypto.createHash('md5');
var md5 = md5sum.update(JSON.stringify(process.argv)).digest('hex');
var QFILE = DATA_DIR + '/' + md5 + '.q';
var CFILE = DATA_DIR + '/' + md5 + '.c';
var LOG   = DATA_DIR + '/' + md5 + '.log';

function dump(m,d){
  if ( typeof(d)=='object' || d instanceof Array ) {
    for ( i in d ) {
      sys.puts('***' + m + ' ' + i + '=> ' + d[i]);
    }
  }else {
    sys.puts('***' + m + ' ' + d);
  }
}

function QUEUE(qfile) {
  this.q = [];
  this.qfile = qfile;
}
//QUEUE.prototype.q = [];
QUEUE.prototype.push = function ( url , test , remark ) {
  this.q.push({URL:url,TEST:test,REMARK:remark});

  this.save();
};
QUEUE.prototype.pop =  function () {
  ret = this.q.pop();
  this.save();
  return ret;
};
QUEUE.prototype.length = function () {
  return this.q.length;
};
QUEUE.prototype.load = function () {
  this.q = JSON.parse(fs.readFileSync(this.qfile));
}
QUEUE.prototype.save = function () {
  if ( this.q.length ) {
    fs.writeFileSync(this.qfile,JSON.stringify(this.q));
  }else{
    fs.unlinkSync(this.qfile);
  }
}

function FETCH_CHECKER(cfile) {
  this.c = {};
  this.cfile = cfile;
}
FETCH_CHECKER.prototype.check = function (url,st) {
  if ( url in this.c ) {
    return this.c[url];
  }
  this.change(url,st);
  return null;
}
FETCH_CHECKER.prototype.change = function (url,st) {
  this.c[url] = st;
  fs.writeFileSync(this.cfile,JSON.stringify(this.c,null,1));
}
FETCH_CHECKER.prototype.timeout = function (url,st) {
  if ( typeof(this.c[url]) != 'number' ){
    this.change(url,'TIMEOUT');
    return true;
  }
  return false;
}
FETCH_CHECKER.prototype.load = function () {
  this.c = JSON.parse(fs.readFileSync(this.cfile));
}


global.DEFAULT_CHECK_STATUS =  {
//    500:true,501:true,502:true,503:true,504:true,505:true,506:true,507:true,508:true,510:true,
//    400:true,401:true,402:true,403:true,404:true,405:true,406:true,407:true,408:true,409:true,410:true,
//    411:true,412:true,413:true,414:true,415:true,416:true,417:true,418:true,420:true,421:true,422:true,423:true,424:true,425:true,426:true,
    300:true,301:true,302:true,303:true,304:true,305:true,306:true,307:true,350:true,
    200:true,201:true,202:true,203:true,204:true,205:true,206:true,207:true,208:true,226:true
};
global.DEFAULT_CHECK_FILTER = {
  ERROR  : [],
  IGNORE : [],
  FOLLOW : [],
  DOMAIN : true
}
global.STATUS_TEST = {
  STATUS   : global.DEFAULT_CHECK_STATUS,
  REDIRECT : {
    ERROR  : [],
    SKIP   : [],
    ALLOW  : [],
    DOMAIN : true
  },
  CHECKS   : []
};
global.CRAWL_TEST = {
  STATUS   : global.DEFAULT_CHECK_STATUS,
  REDIRECT : {
    ERROR  : [],
    SKIP   : [],
    ALLOW  : [],
    SAME   : true
  },
  CHECKS   :[{
    METHOD   : 'CRAWL',
    SELECTORS: ['script','link','img','a'],
    FILTER   : global.DEFAULT_CHECK_FILTER
  }]
}
var DEFAULT_TEST = {
  STATUS   : global.DEFAULT_CHECK_STATUS,
  REDIRECT : {
    ERROR : [],
    SKIP  : [],
    ALLOW : [],
    SAME  : true
  },
  CHECKS   :[{
    METHOD   : 'EXIST',
    SELECTOR : 'html',
  },{
    METHOD   : 'EXIST',
    SELECTOR : 'pre',
  },{
    METHOD   : 'NOT_EXIST',
    SELECTOR : 'h1',
  },{
    METHOD   : 'NOT_EXIST',
    SELECTOR : 'pre',
  },{
    METHOD   : 'TEXT',
    SELECTOR : '#topicsDetailCmp',
    EXPECTS  : /.+/
  },{
    METHOD   : 'TEXT',
    SELECTOR : '#topicsDetailCmp',
    EXPECTS  : /Some expects string/
  },{
    METHOD   : 'LINK',
    SELECTOR : 'a',
    FILTER   : global.DEFAULT_CHECK_FILTER,
    TEST     : global.STATUS_TEST
  },{
    METHOD   : 'LINK',
    SELECTOR : 'link',
    FILTER   : global.DEFAULT_CHECK_FILTER,
    TEST     : global.STATUS_TEST
  },{
    METHOD   : 'LINK',
    SELECTOR : 'img',
    FILTER   : global.DEFAULT_CHECK_FILTER,
    TEST     : global.STATUS_TEST
  },{
    METHOD   : 'LINK',
    SELECTOR : 'script',
    FILTER   : global.DEFAULT_CHECK_FILTER,
    TEST     : global.STATUS_TEST
  }]
};
var SETTING = {
  URL      : null,
  PROXY    : null,
  SSLPROXY : null,
  TIMEOUT  : 5000,
  WAIT     : 1000,
  TEST : DEFAULT_TEST
};

var URL     = null;
var PROXY   = null;
var SSLPROXY= null;
var TIMEOUT = null;
var WAIT    = null;
var TEST    = null;

var WORKER  = null;
function help(a){
  sys.puts('Usage:');
  sys.puts('   node htmlcheck.js -u <url>       [-p <proxy>] [-l loglv] [-j <jquery>]');
  sys.puts('   node htmlcheck.js -c <conf-file> [-p <proxy>] [-l loglv] [-j <jquery>]');
  sys.puts('Options:');
  sys.puts('   -l <loglv>     : Specify the loglv from 0 to 20 ( 10 is defalut )');
  sys.puts('   -p <proxy>     : Specify the http-proxy-url ( for example -p http://proxy.example.com:8080 )');
  sys.puts('   -s <ssl-proxy> : Specify the https-proxy-url ( for example -p http://proxy.example.com:8080 )');
  sys.puts('   -t <timeout>   : Specify the http timeout (msec)');
  sys.puts('   -w <wait>      : Specify the request wait (msec)');
  sys.puts('   -C             : Crawl mode');
  sys.puts('   -u <url>       : Specify the target-url ');
  sys.puts('   -c <conf-file> : Specify the config file path');
  sys.puts('   -j <jquery>    : Specify the jquery file ( jquery-1.4.4.js is default )');
  process.exit(a);
}



try {
  opt.setopt('hl:p:s:u:w:t:c:j:CW',process.argv);
  opt.getopt(function ( o , p ){
    switch (o) {
      case 'h':
      help(0);
      break;
      case 'l':
      LOGLV = p;
      break;
      case 'p':
      PROXY = ''+p;
      break;
      case 's':
      SSLPROXY = ''+p;
      break;
      case 't':
      TIMEOUT = p;
      break;
      case 'w':
      WAIT = p;
      break;
      case 'C':
      TEST = global.CRAWL_TEST;
      break;
      case 'u':
      URL = ''+p;
      break;
      case 'c':
      CONF = ''+p;
      break;
      case 'j':
      JQUERY = ''+p;
      break;
      case 'W':
      WORKER = 1;
      break;
    }
  });
}catch ( e ) {
  error_dump('Invalid option !',e.opt,e.type);
  help(1);
}

if ( CONF ) {
  conf = require(CONF);
  SETTING = conf.get();
}
SETTING.URL     =(URL===null)?     SETTING.URL     :URL;
SETTING.PROXY   =(PROXY===null)?   SETTING.PROXY   :PROXY;
SETTING.SSLPROXY=(SSLPROXY===null)?SETTING.SSLPROXY:SSLPROXY;
SETTING.TIMEOUT =(TIMEOUT===null)? SETTING.TIMEOUT :TIMEOUT;
SETTING.WAIT    =(WAIT===null)?    SETTING.WAIT    :WAIT;
SETTING.TEST    =(TEST===null)?    SETTING.TEST    :TEST;

var TERM = false;
if ( ! WORKER ) {
  var child_process  = require('child_process');
  var cmd = process.argv.shift();
  process.argv.push('-W');

  process.on('SIGINT', function () {
    sys.puts('SIGINT Received !');
    TERM = true;
  });

  function fork_worker(){
    worker = child_process.spawn(cmd,process.argv);
    worker.stdout.on('data',function(data){
      process.stdout.write(data);
    });
    worker.stderr.on('data',function(data){
      process.stderr.write(data);
    });
    worker.on('exit',function (code ) {
      echo_dump(SETTING.URL,'Wait child ',code);
      if ( code === 2 && !TERM ) {
	fork_worker();
      }
    });
  }
  fork_worker();
  return;
}
process.on('SIGINT', function () {
  TERM = true;
});


if ( ! SETTING.URL ) {
  info_dump('Bad arguments !',SETTING );
  help(1);
}

var Q = new QUEUE(QFILE);
var C = new FETCH_CHECKER(CFILE);

try {
  st = fs.statSync(QFILE);
  st = fs.statSync(CFILE);
  Q.load();
  C.load();
  echo_dump(SETTING.URL,'=== CONTINUE ===',md5);
}catch ( e ) {
  echo_dump(SETTING.URL,'=== START ===',md5);
  Q = new QUEUE(QFILE);  
  Q.push(SETTING.URL,SETTING.TEST,'-ROOT-');
//  C = new FETCH_CHECKER(CFILE);  
}


try {
  echo_dump('<URL>','<VAL>','<MSG>','<REMARK>');
  function add_queue ( url , test , remark ) {
    c = C.check(url,'Queuing');
    if ( c ) {
      msg_dump(url,c,'skip queuing');
      return;
    }
    Q.push(url,test,remark);
  }
  function loop (){
    if ( Q.length() ) {
      q = Q.pop();
      fetch_content(q.URL,q.TEST,q.REMARK,add_queue);
      var m = process.memoryUsage();
      echo_dump(q.URL,Math.floor(m.heapUsed/(1024*1024)*100)/100 + ' / ' + Math.floor(m.heapTotal/(1024*1024)*100)/100 + ' (MB)','Q:' + Q.length());
      if (  TERM  || m.heapUsed >  MEM_THRESHOLD ) {
	setTimeout(function(){
	  echo_dump('=== Interrupt ===',m.heapUsed + ' > ' + MEM_THRESHOLD,md5);
	  process.exit(2);
	},SETTING.TIMEOUT);
      } else {
	if ( Q.length() ) {
	  setTimeout(loop,SETTING.WAIT);
	}else {
	  setTimeout(loop,SETTING.TIMEOUT);
	}
      }
    }
  }
  loop();
}catch ( e ) {
  error_dump(SETTING.URL,'*****','*******',e);
}

function debug_dump(msg,data){
  if ( LOGLV >= 20 ) {
    if ( typeof(data)=='object' || data instanceof Array ) {
      sys.puts('D    ' + msg + ' : ');
      for ( i in data ) {
	sys.puts('      ' + i + ' : ' + data[i]);
      }
    }else{
      sys.puts('D    ' + msg + ' : ' + data);
    }
  }
}

function trace_dump(msg,data){
  if ( LOGLV >= 15 ) {
    if ( typeof(data)=='object' || data instanceof Array ) {
      sys.puts('T   ' + msg + ' : ');
      for ( i in data ) {
	sys.puts('   ' + i + ' : ' + data[i]);
      }
    }else{
      sys.puts('T   ' + msg + ' : ' + data);
    }
  }
}

function info_dump(msg,data){
  if ( LOGLV >= 10 ) {
    if ( typeof(data)=='object' || data instanceof Array ) {
      sys.puts('I ' + msg + ' : ');
      for ( i in data ) {
	sys.puts(' ' + i + ' : ' + data[i]);
      }
    }else{
      sys.puts('I ' + msg + ' : ' + data);
    }
  }
}

function padding(str,n){
  if ( str.length < n ){
    var p = n-str.length;
    for ( var i = 0 ; i < p ; i++ ) {
      str += ' ';
    }
  }
  return str;
}

function status_dump(url,selector,msg,body){
  if ( LOGLV >= 8 ) {
    sys.puts('STATUS: ' + padding(url,URL_LEN) + ' ; ' + padding(msg,MSG_LEN) + '  ; ' + padding(selector,SEL_LEN) + (body?'  ; '+body:''));
  }
}

function msg_dump(url,selector,msg,body){
  if ( LOGLV >= 6 ) {
    sys.puts('MSG   : ' + padding(url,URL_LEN) + ' ; ' + padding(msg,MSG_LEN) + '  ; ' + padding(selector,SEL_LEN) + (body?'  ; '+body:''));
  }
}

function ok_dump(url,selector,msg,body){
  if ( LOGLV >= 4 ) {
    msg = 'OK    : ' + padding(url,URL_LEN) + ' ; ' + padding(msg,MSG_LEN) + '  ; ' + padding(selector,SEL_LEN) + (body?'  ; '+body:'');
    sys.puts(msg);
    var log = fs.openSync(LOG,'a+');
    fs.writeSync(log,msg+'\n',null);
    fs.closeSync(log);
  }
}

function echo_dump(url,selector,msg){
  if ( LOGLV >= 2 ) {
    sys.puts('ECHO  : ' + padding(url,URL_LEN) + ' ; ' + padding(msg,MSG_LEN) + '  ; ' + (selector?padding(selector,SEL_LEN):''));
  }
}

function error_dump(url,selector,msg,body){
  if ( LOGLV >= 0 ) {
    msg = 'ERROR    : ' + padding(url,URL_LEN) + ' ; ' + padding(msg,MSG_LEN) + '  ; ' + padding(selector,SEL_LEN) + (body?'  ; '+body:'');
    sys.puts(msg);
    var log = fs.openSync(LOG,'a+');
    fs.writeSync(log,msg+'\n',null);
    fs.closeSync(log);
  }
}

function uniq( arr ) {
  var buf = {};
  return arr.filter(function (e){
    if ( buf[e] ) {
      return false;
    }
    buf[e] = true;
    return true;
  });
}


function fetch_content(strurl,TEST,remark,callback){
  C.change(strurl,'Fetching');
  trace_dump('TRY' ,strurl);
  var parsed = url.parse(strurl);
  var request = null;
  if ( parsed.protocol === 'https:' ) {
    if ( SETTING.SSLPROXY ) {
      var px = /([^:]+):(\d+)/. exec(SETTING.SSLPROXY);
//      proxy = http.request({'host':px[1],'port':px[2],'path':parsed.hostname+':'+((parsed.port)?parsed.port:443),'method':'CONNECT'});
//      request = https.request({'host':parsed.host,'path':path,'method':'GET'});
//      request.connection = proxy.connection;
    }else{
      request = https.request({'host':parsed.host,'path':path,'method':'GET'});
    }
  }else {
    if ( SETTING.PROXY ) {
      var px = /([^:]+):(\d+)/. exec(SETTING.PROXY);
      debug_dump('PROXY','GET: (' + px[1]+ ':'+px[2]+')  : ' + strurl);
      request = http.request({'host':px[1],'port':px[2],'path':strurl,'method':'GET'});
    } else {
      var path = parsed.pathname+((parsed.search)?parsed.search:'')
      debug_dump('REQUEST','GET: (' + parsed.host+ ')  : ' + path);
      request = http.request({'host':parsed.host,'path':path,'method':'GET'});
    }
  }

  if ( /house\.www\.infoseek\.co\.jp/.test(strurl) ) {
    debug_dump('***',request);
  }

  request.on('error',function(err){
    error_dump(strurl,'HTTP ERROR',err);
  });
  
  request.on('response',function(res){
    C.change(strurl,res.statusCode);

    status_dump(strurl,res.statusCode,'STATUS');
    if ( ! (res.statusCode in  TEST.STATUS) ){
      error_dump(strurl,res.statusCode,'BAD STATUS 1');
    }else{

    }
    if ( res.statusCode === 302 || res.statusCode === 301 ) {
      if ( TEST.REDIRECT ) {
	var location = res.headers['location'];
	trace_dump('LOCATION',location);
	if ( strurl === location ) {
	  error_dump(strurl,location,'CIRCULAR LOCATION');
	}
	for ( var i in TEST.REDIRECT.ERROR ) {
	  if ( TEST.REDIRECT.ERROR[i].test(location) ) {
	    error_dump(strurl,location,'ERROR LOCATION ');
	    return;
	  }
	}
	for ( var i in TEST.REDIRECT.SKIP ) {
	  if ( TEST.REDIRECT.SKIP[i].test(location) ) {
	    msg_dump(strurl,location,'skip redirection');
	    return;
	  }
	}
	for ( var i in TEST.REDIRECT.ALLOW ) {
	  if ( TEST.REDIRECT.ALLOW[i].test(location) ) {
	    msg_dump(strurl,location,'redirect');
	    callback(location,TEST,strurl);
	    return;
	  }
	}
	var parsedl = url.parse(location);
	if ( TEST.REDIRECT.DOMAIN && parsed.host === parsedl.host ) {
	  msg_dump(strurl,location,'redirect');
	  callback(location,TEST,strurl);
	  return;
	}
      }
      msg_dump(strurl,location,'ignore redirection');
      return;
    }
    if ( TEST.CHECKS.length === 0 ){
      ok_dump(strurl,res.statusCode,'status ok');
      return;
    }
    res.setEncoding('utf8');
    var body = '';
    res.on('data',function(chunk){
      body = body + chunk;
    });
    res.on('end',function(){
      // res.destroy(); // @@@ 
      var content_type = res.headers['content-type'];
      debug_dump('TYPE' ,content_type);
      if ( ! /html/.test(content_type)  ) {
	ok_dump(strurl,content_type,'fetch ok');
	return;
      }
      if ( ! body ) {
	error_dump(strurl,content_type,'NO BODY',res.statusCode);
	return;
      }
      debug_dump('BODY' ,body);
      // The error that "TypeError: Cannot call method 'call' of undefined" often occurs. It's jsdom has some bug ? (conflecting body's script ?)
      try {
	var document = jsdom(body,null,{
	  features:{
	    FetchExternalResources : false,
	    ProcessExternalResources : false,
	      "MutationEvents"           : '2.0',
	      "QuerySelector"            : false
	  }
	});
	var window = document.createWindow();
//	document.cookie=res.headers['set-cookie'].toString();
	jsdom.jQueryify(window, JQUERY, function (window, $) {
	// jsdom.jQueryify(window, 'jquery.cookie.js', function (window, $) {
	  function check_filter ( link , CHECK ) {
	    for ( var i in CHECK.FILTER.ERROR ) {
	      if ( CHECK.FILTER.ERROR[i].test(link) ) {
		error_dump(strurl,CHECK.FILTER.ERROR[i],'BAD LINK  ',link);
		return false;
	      }
	    }

	    for ( var i in CHECK.FILTER.IGNORE ) {
	      if ( CHECK.FILTER.IGNORE[i].test(link) ) {
		msg_dump(strurl,CHECK.FILTER.IGNORE[i],'skip link (ignore)',link);
		return false;
	      }
	    }

	    for ( var i in CHECK.FILTER.FOLLOW ) {
	      if ( CHECK.FILTER.FOLLOW[i].test(link) ) {
		return true;
	      }
	    }
	    if ( CHECK.FILTER.DOMAIN ) {
	      var preq = url.parse(link);
	      if ( preq.host === parsed.host ) {
		return true;
	      }
	    }
	    msg_dump(strurl,link,'skip link (unmatch)');
	    return false;
	  }

	  function uniq_links( elements, links ) {
	    if ( ! links ){
	      links = [];
	    }
	    re = /(^[^#]+)#.*$/;
	    elements.each( function(){
	      href = $(this).attr('href');
	      if ( href ) {
		links.push(url.resolve(strurl,href).replace(re,"$1"));
	      }
	      src = $(this).attr('src');
	      if ( src ) {
		links.push(url.resolve(strurl,src).replace(re,"$1"));
	      }
	    });
	    return uniq(links).sort().reverse();
	  }
	  for( var i in TEST.CHECKS ) {
	    CHECK = TEST.CHECKS[i];
	    if ( CHECK.METHOD == 'CRAWL' ) {
	      ok_dump(strurl,remark,'crawl ok');
	      links = [];
	      for ( var s in CHECK.SELECTORS ) {
		links = uniq_links($(CHECK.SELECTORS[s]),links);
	      }
	      for ( var l in links ) {
		if ( check_filter(links[l],CHECK )){
		  callback(links[l],TEST,strurl);
		}
	      }
	    } else {
	      elements = $(CHECK.SELECTOR);
 	      debug_dump('CHECK('+CHECK.METHOD+')',CHECK.SELECTOR); 
	      if ( CHECK.METHOD == 'EXIST' ) {
		size = elements.size();
		if ( size === 0 ) {
		  error_dump(strurl,CHECK.SELECTOR,'ELEMENT NOT FOUND','');
		}else {
		  ok_dump(strurl,CHECK.SELECTOR,'exist ok','');
		}
	      }else if ( CHECK.METHOD == 'NOT_EXIST' ) {
		size = elements.size();
		if ( size !== 0 ) {
		  error_dump(strurl,CHECK.SELECTOR,'UNEXPECTED ELEMENT FOUND',elements.html());
		}else {
		  ok_dump(strurl,CHECK.SELECTOR,'not exist ok','');
		}
	      }else if ( CHECK.METHOD == 'TEXT' ) {
		text = elements.text();
		if ( ! CHECK.EXPECTS.test(text) ) {
		  error_dump(strurl,CHECK.SELECTOR,'TEXT UNMATCH',text);
		}else {
		  ok_dump(strurl,CHECK.SELECTOR,'text ok');
		}
	      }else if ( CHECK.METHOD == 'LINK' ) {
		links = uniq_links(elements);
		for ( var l in links ) {
		  if ( check_filter(links[l],CHECK )){
		    callback(links[l],CHECK.TEST,strurl);
		  }
		}
	      }else{
		error_dump(strurl,CHECK.METHOD,'Unknown check');
		return;
	      }
	    }
	  }
	}); // jquery-1.4.4.js
      }catch(e){
	error_dump(strurl,content_type,'INVALID HTML',e);
	return;
      }
    });
  });

  setTimeout(function () {
    if ( request ) {
      request.abort();
    }
    if ( C.timeout(strurl) ) {
      error_dump(strurl,'' + SETTING.TIMEOUT + ' (ms)','TIMEOUT');
    }
  },SETTING.TIMEOUT);
  request.end();
}

// NODE_PATH=/usr/local/nodejs/lib/node_modules node htmlwatch.js -l 10 -c `pwd`/conf.js 


/*
var CHECK_FILTER = {
  ERROR  : [],
  IGNORE : [],
  FOLLOW : [/www.google.co.jp/],
  DOMAIN   : true
};

exports.get = function() { 
  return {
    URL      : 'http://google.co.jp/',
    PROXY    : null, // <host>:<port>
    SSLPROXY : null, // Not supported
    TIMEOUT  : 5000, // msec
    WAIT     : 100,  // msec
    TEST : { 
      STATUS   : global.DEFAULT_CHECK_STATUS,
      REDIRECT : {
	ERROR : [],
	SKIP  : [],
	ALLOW : [/www.google.co.jp/],
	SAME  : true
      },
      CHECKS   :
	[{
	  METHOD   : 'EXIST',
	  SELECTOR : '#fll',
	},{
	  METHOD   : 'NOT_EXIST',
	  SELECTOR : 'pre',
	},{
	  METHOD   : 'TEXT',
	  SELECTOR : '#fll a.gl',
	  EXPECTS  : /Google.com in English/
	},{
	  METHOD   : 'LINK',
	  SELECTOR : 'a',
	  FILTER   : CHECK_FILTER,
	  TEST     : global.STATUS_TEST
	},{
	  METHOD   : 'LINK',
	  SELECTOR : 'link',
	  FILTER   : CHECK_FILTER,
	  TEST     : global.STATUS_TEST
	},{
	  METHOD   : 'LINK',
	  SELECTOR : 'img',
	  FILTER   : CHECK_FILTER,
	  TEST     : global.STATUS_TEST
	},{
	  METHOD   : 'LINK',
	  SELECTOR : 'script',
	  FILTER   : CHECK_FILTER,
	  TEST     : global.STATUS_TEST
	}]
    }
  };
}
*/
