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
