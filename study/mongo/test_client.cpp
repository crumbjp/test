/*
 * test_client.cpp - ????
 *  
 *   Copyright (C) 2013 rakuten 
 *     by hiroaki.kubota <hiroaki.kubota@mail.rakuten.com> 
 *     Date : 2013/05/07
 */

#include "mongo/client/dbclient.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <typeinfo>
#include <cxxabi.h>
using namespace std;
using namespace mongo;

static const int DIV_BASE   = 2500;
static const int THREAD_MAX = 100;

struct pidst {
  int64_t timestamp;
  int     utime    ;
  int     stime    ;
  int     vsize    ;
  int     rss      ;
  pidst(int64_t timestamp = 0,
        int     utime     = 0,
        int     stime     = 0,
        int     vsize     = 0,
        int     rss       = 0)
    :timestamp(timestamp),
     utime    (utime    ),
     stime    (stime    ),
     vsize    (vsize    ),
     rss      (rss      ) {
  }
  pidst operator - ( const pidst & p) {
    return pidst(this->timestamp - p.timestamp,
                 this->utime     - p.utime    ,
                 this->stime     - p.stime    ,
                 this->vsize     - p.vsize    ,
                 this->rss       - p.rss      );
 }
};
pidst pidstat(pid_t pid ) {
  pidst ret;
  ostringstream statfile;
  statfile << "/proc/" << pid << "/stat";
  char buf[1024];
  FILE * fp = fopen(statfile.str().c_str(),"r");
  fread(buf,sizeof(buf),1,fp);
  fclose(fp);
  timeval tv;
  gettimeofday(&tv,0);
  ret.timestamp = tv.tv_sec * 1000000 + tv.tv_usec;
  char * sp = 0;
  char * p = buf;
  for ( int i = 0 ; ; i++ ) {
    char * token = strtok_r(p," ",&sp);
    p = 0;
    if ( i == 13 ) {
      ret.utime = strtoul(token,0,0);
    }
    if ( i == 14 ) {
      ret.stime = strtoul(token,0,0);
    }
    if ( i == 22 ) {
      ret.vsize = strtoul(token,0,0);
    }
    if ( i == 23 ) {
      ret.vsize = strtoul(token,0,0);
      break;
    }
  }
  return ret;
}

struct thread_test;

struct base_args {
  thread_test *THIS;
  DBClientConnection conn;
  int progres;
  int n;
  base_args( )
    :THIS(0),progres(0),n(n){
  }
};

struct thread_test {
  DBClientConnection conn;
  bool fin;
  pthread_cond_t  cond;
  pthread_mutex_t mutex;

  const char *connect_to;
  const char *ns;
  int  pid;
  int  ndocs;
  int  docsize;
  int  nthreads;

  vector<base_args*> args;
  thread_test( 
    const char *connect_to,const char *ns,int  pid,int  ndocs ,int  docsize,int  nthreads )
    : fin(false),
      connect_to(connect_to),
      ns        (ns        ),
      pid       (pid       ),
      ndocs     (ndocs     ),
      docsize   (docsize   ),
      nthreads  (nthreads  )
    {
      pthread_cond_init(&cond,0);
      pthread_mutex_init(&mutex,0);
      string errmsg;
      if ( ! conn.connect( connect_to , errmsg ) ) {
        throw runtime_error(errmsg);
      }
    }
  virtual ~thread_test(){
    for ( vector<base_args *>::iterator it(args.begin()),itend(args.end());
          it != itend;
          it++ ){
      delete *it;
    }
  }
  virtual base_args * getarg(int n) {
    return new base_args();
  }
  virtual void prepare( base_args * arg) {};
  virtual void test( base_args * arg ) = 0;
  virtual void finish(pidst diff) {};
  void start(){
    pthread_t th[nthreads];
    pthread_t dth;
    for ( int i = 0 ; i < nthreads ; i++ ) {
      base_args *a = this->getarg(i);
      a->n = i;
      a->THIS = this;
      args.push_back(a);
      pthread_create(&th[i],0,test_handler,(void*)a);
    }
    usleep(1000000);
    pthread_create(&dth,0,dump_handler,(void*)this);
    pthread_cond_broadcast(&this->cond);
    pidst begin = pidstat(pid);
    for ( int i = 0 ; i < nthreads ; i++ ) {
      pthread_join(th[i],0);
    }
    fin = true;
    pidst end = pidstat(pid);
    pidst diff = end - begin;
    double cpu = (diff.utime + diff.stime) / (diff.timestamp/1000000.0);
    pthread_join(dth,0);
    cout << "\rTIME: " << (diff.timestamp/1000000.0) << " , CPU : " << cpu << endl;
    finish(diff);
  }
  static void * dump_handler(void *a){
    thread_test *THIS = (thread_test*)a;
    int  linecount = 0;
    bool lineflg = false;
    for ( vector<base_args *>::iterator it(THIS->args.begin()),itend(THIS->args.end());
            it != itend;
            it++ ){
      if ( lineflg ) {
        linecount++;
        cout << endl;
      }
      lineflg ^= true;
    }
    linecount++;
    cout << endl;
    while (!THIS->fin){
      sleep(1);
      cout << "\033[" << linecount << "A\r";
      lineflg = false;
      for ( vector<base_args *>::iterator it(THIS->args.begin()),itend(THIS->args.end());
            it != itend;
            it++ ){
        //cout << "\033[2K\r" << setw(10) <<(*it)->n << " : " << setw(15) << (*it)->progres << fflush;
        cout << setw(10) <<(*it)->n << " : " << setw(15) << (*it)->progres << fflush;
        if ( lineflg ) {
          cout << endl << "\033[2K\r";
        }
        lineflg ^= true;
      }
      cout << endl;
    }
    cout << "\033[" << linecount << "A\r" << fflush;
    for ( int i = 0 ; i < linecount ; i++ ) {
      cout << "\033[2K\r" << endl;
    }
    cout << "\033[" << linecount << "A\r" << fflush;
    return 0;
  }
  static void * test_handler(void *a){
    base_args * arg = (base_args*)a;
    try { 
      arg->THIS->prepare(arg);
      pthread_cond_wait(&arg->THIS->cond,&arg->THIS->mutex);
      pthread_mutex_unlock(&arg->THIS->mutex);
      arg->THIS->test(arg);
    }catch(...){
      cout << arg->n << " : Fatal error !" << endl;
    }
    return 0;
  }
};

struct insert_args : base_args {
  int from;
  int to;
  int div1;
  int div2;
  long long totalsize;
  const char * value9;
  insert_args (  int from  ,
                 int to    ,
                 int div1  ,
                 int div2  ,
                 const char * value9)
    : base_args(),
      from  (from  ),
      to    (to    ),
      div1  (div1  ),
      div2  (div2  ),
      value9(value9),
      totalsize(0) {
  }
};

struct insert_test :  thread_test {
  string value9;
  insert_test( 
    const char *connect_to,const char *ns,int  pid,int  ndocs,int  docsize,int  nthreads 
    )
    : thread_test(connect_to,ns,pid,ndocs,docsize,nthreads) 
    {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " : " << nthreads << " ===" << endl;

    value9 = "";
    for ( int i = 365 ; i < docsize ; i++ ) {
      value9 += "H";
    }
    conn.dropCollection(ns);
  }
  virtual ~insert_test() {
  }
  virtual base_args * getarg(int n){
    return new insert_args(
      (ndocs/nthreads)*n,
      (ndocs/nthreads)*(n+1),
      DIV_BASE,
      ndocs/DIV_BASE,
      value9.c_str());
  }
  virtual void prepare(base_args * arg) {
    string errmsg;
    if ( ! arg->conn.connect( connect_to , errmsg ) ) {
      throw runtime_error(errmsg);
    }
  }
  virtual void test( base_args * a ) {
    insert_args * arg = (insert_args*)a;
    for ( int i = arg->from ; i < arg->to ; i++ ){
      arg->progres = i;
      BSONObj obj =  BSON( "_id" << i << 
                           "value0" << i << 
                           "value1" << i % arg->div1 << 
                           "value2" << i % arg->div2 << 
                           "value3" << "aaaaaaaaaa" << 
                           "value4" << "bbbbbbbbbbbbbbbbbbbb" << 
                           "value5" << "cccccccccccccccccccccccccccccc" << 
                           "value6" << "dddddddddddddddddddddddddddddddddddddddddddddddddd" << 
                           "value7" << "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee" << 
                           "value8" << i << 
                           "value9" << arg->value9);
      arg->conn.insert( ns ,obj);
      arg->totalsize += obj.objsize();
    }
    string errmsg = arg->conn.getLastError();
    if ( ! errmsg.empty() ) {
      throw runtime_error(errmsg);
    }
  }
  virtual void finish(pidst diff) {
    long long totalsize = 0L;
    for ( vector<base_args *>::iterator it(args.begin()),itend(args.end());
            it != itend;
            it++ ){
      totalsize += ((insert_args*)(*it))->totalsize;
    }
    cout << setw(5) << "ALL" << ": COUNT: " << ndocs << "( " << ndocs / (diff.timestamp/1000000.0) << " n/s) , TOTAL_SIZE : " << totalsize << " (" << totalsize  / (diff.timestamp/1000000.0) / 1024 / 1024 << " mb/s)" << endl;
  }
};


struct ensure_test :  thread_test {
  string fieldname;
  ensure_test( string fieldname,
    const char *connect_to,const char *ns,int  pid,int  ndocs,int  docsize
    )
    : thread_test(connect_to,ns,pid,ndocs,docsize,1),
      fieldname(fieldname) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " : " << nthreads << " ===" << endl;
  }
  virtual ~ensure_test() {
  }
  virtual void test( base_args * arg ) {
    conn.ensureIndex( ns , BSON( fieldname.c_str() << 1 ));
    string errmsg = conn.getLastError();
    if ( ! errmsg.empty() ) {
      throw runtime_error(errmsg);
    }
  }
  virtual void finish(pidst diff) {
    cout << setw(5) << "ALL:" << " COUNT: " << ndocs << "( " << ndocs / (diff.timestamp/1000000.0) << " n/s )" << endl;
  }
};

template <typename VALTYPE>
struct update_test :  thread_test {
  long long totalsize;
  string fieldname;
  VALTYPE value;
  update_test( string fieldname,VALTYPE value,
    const char *connect_to,const char *ns,int  pid,int  ndocs,int  docsize
    )
    : thread_test(connect_to,ns,pid,ndocs,docsize,1),
      totalsize(0),
      fieldname(fieldname),
      value(value) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " : " << nthreads << " ===" << endl;
  }
  virtual ~update_test() {
  }
  virtual void test( base_args * arg ) {
    BSONObj obj = BSON(fieldname << value);
    totalsize += obj.objsize();
    conn.update( ns , BSONObj() ,BSON( "$set" << obj ),false,true);
    string errmsg = conn.getLastError();
    if ( ! errmsg.empty() ) {
      throw runtime_error(errmsg);
    }
  }
  virtual void finish(pidst diff) {
    cout << setw(5) << "ALL" << ": COUNT: " << ndocs << "( " << ndocs / (diff.timestamp/1000000.0) << " n/s) , TOTAL_SIZE : " << totalsize << " (" << totalsize  / (diff.timestamp/1000000.0) / 1024 / 1024 << " mb/s)" << endl;
  }
};

struct query_base_args : base_args {
  long long totalsize;
  long long count;
  query_base_args() : totalsize(0),count(0){}
};
struct query_base_test : thread_test {
  string fieldname;
  query_base_test( string fieldname , 
    const char *connect_to,const char *ns,int  pid,int  ndocs,int  docsize,int  nthreads 
    )
    : thread_test(connect_to,ns,pid,ndocs,docsize,nthreads),
      fieldname(fieldname) {
  }
  virtual ~query_base_test() {
  }
  virtual void prepare(base_args * arg) {
    string errmsg;
    if ( ! arg->conn.connect( connect_to , errmsg ) ) {
      throw runtime_error(errmsg);
    }
  }
  virtual BSONObj gen_query(query_base_args * a) = 0;

  virtual void test( base_args * a ) {
    query_base_args * arg = (query_base_args*)a;
    auto_ptr<DBClientCursor> cursor = arg->conn.query( ns , gen_query(arg));
    while ( cursor->more() ) {
      arg->count++;
      BSONObj obj = cursor->next();
      arg->totalsize += obj.objsize();
      arg->progres = (int)arg->count;
    }
  }  

  virtual void finish(pidst diff) {
    long long count = 0L;
    long long totalsize = 0L;
    for ( vector<base_args *>::iterator it(args.begin()),itend(args.end());
            it != itend;
            it++ ){
      // cout << setw(10) <<(*it)->n << ": COUNT: " << (*it)->arg->count << "( " << (*it)->arg->count / (diff.timestamp/1000000.0) << ") , TOTAL_SIZE : " << (*it)->arg->totalsize  << " (" << (*it)->arg->totalsize / (diff.timestamp/1000000.0)<< ")" << endl;
      count += ((query_base_args*)(*it))->count;
      totalsize += ((query_base_args*)(*it))->totalsize;
    }
    cout << setw(5) << "ALL" << ": COUNT: " << count << "( " << count / (diff.timestamp/1000000.0) << " n/s) , TOTAL_SIZE : " << totalsize << " (" << totalsize / (diff.timestamp/1000000.0) / 1024 / 1024 << " mb/s)" << endl;
  }
};

struct query_test :  query_base_test {
  int value;
  query_test( string fieldname , int value , 
    const char *connect_to,const char *ns,int  pid,int  ndocs,int  docsize,int  nthreads 
    )
    : query_base_test(fieldname,connect_to,ns,pid,ndocs,docsize,nthreads) 
    {
      int status;
      cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " : " << nthreads << " ===" << endl;
    }
  virtual ~query_test() {
  }
  virtual base_args * getarg(int n){
    return new query_base_args();
  }
  virtual BSONObj gen_query(query_base_args * a) {
    return BSON( fieldname << value);
  }
};


struct query_in_args : query_base_args {
  vector<int> values;
  query_in_args() : query_base_args() {}
};


struct query_in_test :  query_base_test {
  query_in_test( string fieldname , 
    const char *connect_to,const char *ns,int  pid,int  ndocs,int  docsize,int  nthreads 
    )
    : query_base_test(fieldname,connect_to,ns,pid,ndocs,docsize,nthreads) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " : " << nthreads << " ===" << endl;
  }
  virtual ~query_in_test() {
  }
  virtual base_args * getarg(int n){
    query_in_args * a = new query_in_args();
    for ( int i = 0 ; i < (THREAD_MAX / nthreads); i++ ) {
      a->values.push_back(n * (THREAD_MAX / nthreads) + i);
    }
    return a;
  }
  virtual BSONObj gen_query(query_base_args * a) {
    query_in_args * arg = (query_in_args*) a;
    return BSON( fieldname << BSON("$in" << arg->values) );
  }
};

struct query_range_args : query_base_args {
  int from;
  int to;
  query_range_args(int from,int to) : from(from),to(to){}
};
struct query_range_test :  query_base_test {
  query_range_test( string fieldname , 
    const char *connect_to,const char *ns,int  pid,int  ndocs,int  docsize,int  nthreads 
    )
    : query_base_test(fieldname,connect_to,ns,pid,ndocs,docsize,nthreads) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " : " << nthreads << " ===" << endl;
  }
  virtual base_args * getarg(int n){
    return new query_range_args(
      (ndocs/nthreads)*n,
      (ndocs/nthreads)*n + (ndocs / DIV_BASE) * (THREAD_MAX/nthreads)
      );
  }
  virtual BSONObj gen_query(query_base_args * a) {
    query_range_args * arg = (query_range_args*) a;
    return BSON( fieldname << BSON("$lt" << arg->to << "$gte" << arg->from));
  }
};

struct conn_test :  thread_test {
  conn_test(
    const char *connect_to,const char *ns,int  pid,int  ndocs,int  docsize,int  nthreads 
    )
    : thread_test(connect_to,ns,pid,ndocs,docsize,nthreads) 
    {
      int status;
      cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " : " << nthreads << " ===" << endl;
    }
  virtual ~conn_test() {
  }
  virtual void test( base_args * arg ) {
    for ( int i = 0 ; i < ndocs ; i++ ){
      try { 
        arg->progres = i;
        DBClientConnection conn;
        conn.setSoTimeout(1);
        string errmsg;
        if ( ! conn.connect( connect_to , errmsg ) ) {
          throw runtime_error(errmsg);
        }
        BSONObj obj = conn.findOne( ns ,BSON( "_id" << i ));
      }catch(...){
      }
    }
  }
};
struct conn_pool_test :  thread_test {
  conn_pool_test(
    const char *connect_to,const char *ns,int  pid,int  ndocs,int  docsize,int  nthreads 
    )
    : thread_test(connect_to,ns,pid,ndocs,docsize,nthreads) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " : " << nthreads << " ===" << endl;
  }
  virtual ~conn_pool_test() {
  }
  virtual void test( base_args * arg ) {
    DBClientConnection conn;
    string errmsg;
    conn.setSoTimeout(1);
    for ( int i = 0 ; i < ndocs ; i++ ){
      try { 
        arg->progres = i;
        if ( ! conn.connect( connect_to , errmsg ) ) {
          throw runtime_error(errmsg);
        }
        BSONObj obj = conn.findOne( ns ,BSON( "_id" << i ));
      }catch(...){
      }
    }
  }
};


int main ( int argc , char * argv[]  ){
  const char *CONNECT_TO = "localhost:27017";
  const char *NS         = "test.TEST";
  int  PID         = 0;
  int  DOCSIZE     = 400;
  int  NDOCS       = 100000;
  int  NTHREADS    = 10;
  if ( argc >= 2 ) {
    CONNECT_TO = argv[1];
  }
  if ( argc >= 3 ) {
    NS = argv[2];
  }
  if ( argc >= 4 ) {
    PID = strtoul(argv[3],0,0);
  }
  if ( argc >= 5 ) {
    DOCSIZE = strtoul(argv[4],0,0);
  }
  if ( argc >= 6 ) {
    NDOCS = strtoul(argv[5],0,0);
  }
  if ( argc >= 7 ) {
    NTHREADS = strtoul(argv[6],0,0);
  }
  cout << setw(10) << "CONNECT TO : " << setw(15) << CONNECT_TO << endl;
  cout << setw(10) << "NS         : " << setw(15) << NS << endl;
  cout << setw(10) << "NTHREADS   : " << setw(15) << NTHREADS << endl;
  cout << setw(10) << "NDOCS      : " << setw(15) << NDOCS << endl;

  DBClientConnection conn;
  string errmsg;
  if ( ! conn.connect( CONNECT_TO , errmsg ) ) {
    cout << " : Couldn't connect : " << errmsg << endl;
  }

  
  {
    insert_test i(CONNECT_TO,NS,PID,NDOCS,DOCSIZE,NTHREADS);
    i.start();
  }
  // {
  //   conn_test c(CONNECT_TO,NS,PID,NDOCS,DOCSIZE,THREAD_MAX);
  //   c.start();
  // }
  // {
  //   conn_pool_test c(CONNECT_TO,NS,PID,NDOCS,DOCSIZE,THREAD_MAX);
  //   c.start();
  // }
  {
    insert_test i(CONNECT_TO,NS,PID,NDOCS,DOCSIZE,1);
    i.start();
  }
  {
    ensure_test e("value1",CONNECT_TO,NS,PID,NDOCS,DOCSIZE);
    e.start();
  }
  {
    ensure_test e("value2",CONNECT_TO,NS,PID,NDOCS,DOCSIZE);
    e.start();
  }
  { // It seems like that the number field of the new document is not normal number type...
    update_test<double> u("value8",1,CONNECT_TO,NS,PID,NDOCS,DOCSIZE);
    u.start();
  }
  {
    update_test<double> u("value8",3,CONNECT_TO,NS,PID,NDOCS,DOCSIZE);
    u.start();
  }
  {
    update_test<long long> u("value8",0x100000000LL,CONNECT_TO,NS,PID,NDOCS,DOCSIZE);
    u.start();
  }
  {
    update_test<const char *> u("value5","CCCCCCCCCCCCCCCCCCCCCCCCCCCCCC",CONNECT_TO,NS,PID,NDOCS,DOCSIZE);
    u.start();
  }
  {
    update_test<const char *> u("value7","EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE",CONNECT_TO,NS,PID,NDOCS,DOCSIZE);
    u.start();
  }
  {
    query_in_test q("value1",CONNECT_TO,NS,PID,NDOCS,DOCSIZE,1);
    q.start();
  }
  {
    query_in_test q("value1",CONNECT_TO,NS,PID,NDOCS,DOCSIZE,NTHREADS);
    q.start();
  }
  {
    query_in_test q("value1",CONNECT_TO,NS,PID,NDOCS,DOCSIZE,THREAD_MAX);
    q.start();
  }
  {
    query_test q("value2",0,CONNECT_TO,NS,PID,NDOCS,DOCSIZE,1);
    q.start();
  }
  {
    query_test q("value2",1,CONNECT_TO,NS,PID,NDOCS,DOCSIZE,1);
    q.start();
  }
  {
    query_range_test r("_id",CONNECT_TO,NS,PID,NDOCS,DOCSIZE,1);
    r.start();
  }
  {
    query_range_test r("_id",CONNECT_TO,NS,PID,NDOCS,DOCSIZE,NTHREADS);
    r.start();
  }
  {
    query_range_test r("_id",CONNECT_TO,NS,PID,NDOCS,DOCSIZE,THREAD_MAX);
    r.start();
  }
}
/*
  wget http://downloads.mongodb.org/cxx-driver/mongodb-linux-x86_64-2.4.3.tgz
  g++ test_client.cpp -o test_client -pthread  -L ./mongo-cxx-driver-v2.4/build/ -l mongoclient -lboost_thread-mt -lboost_filesystem -I ./mongo-cxx-driver-v2.4/src/
  nice ./test_client 127.0.0.1:27017 test.TEST 200 100000
*/


//Tue May  7 19:55:26 PDT 2013
//Tue May  7 19:55:35 PDT 2013

//65075 (mongod) S 1 65074 65074 0 -1 4202560 3376180 0 34999 0 207422 35237 0 0 20 0 14 0 44577557 21832527872 230540 18446744073709551615 4194304 21859778 140737049802384 140737049796848 254123316467 0 16387 4096 3821 18446744073709551615 0 0 17 1 0 0 1 0 0 
//65075 (mongod) S 1 65074 65074 0 -1 4202560 3387716 0 35142 0 208142 35349 0 0 20 0 14 0 44577557 21832527872 228781 18446744073709551615 4194304 21859778 140737049802384 140737049796848 254123316467 0 16387 4096 3821 18446744073709551615 0 0 17 0 0 0 1 0 0
