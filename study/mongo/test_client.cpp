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

const char *CONNECT_TO = "localhost:27017";
const char *NS         = "test.test";
int  NTHREADS    = 10;
int  DOCSIZE     = 400;
int  NDOCS       = 100000;
      

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

template <typename ARGTYPE>
struct thread_test {
  bool fin;
  pthread_cond_t  cond;
  pthread_mutex_t mutex;
  int nthreads;
  struct arg_t {
    thread_test<ARGTYPE> *THIS;
    int i;
    int n;
    ARGTYPE *arg;
    arg_t(thread_test<ARGTYPE> *THIS,
          int n ,
          ARGTYPE *arg )
      :THIS(THIS),i(0),n(n),arg(arg){
    }
  };
  vector<arg_t*> args;
  thread_test( int nthreads = 1 )
    : fin(false),
      nthreads(nthreads) {
      pthread_cond_init(&cond,0);
      pthread_mutex_init(&mutex,0);
    }
  virtual ~thread_test(){
    for ( typename vector<arg_t *>::iterator it(args.begin()),itend(args.end());
          it != itend;
          it++ ){
      delete (*it)->arg;
      delete *it;
    }
  }
  virtual ARGTYPE * getarg(int n) = 0;
  virtual void prepare( int n , ARGTYPE * arg) {};
  virtual void test( int n , ARGTYPE * arg , int &ai ) = 0;
  virtual void finish(pidst diff) {};
  void start(){
    pthread_t th[nthreads];
    pthread_t dth;
    for ( int i = 0 ; i < nthreads ; i++ ) {
      arg_t *a = new arg_t(
        this,
        i,
        this->getarg(i));
      args.push_back(a);
      pthread_create(&th[i],0,test_handler,(void*)a);
    }
    usleep(1000000);
    pthread_create(&dth,0,dump_handler,(void*)this);
    pthread_cond_broadcast(&this->cond);
    pidst begin = pidstat(65075);
    for ( int i = 0 ; i < nthreads ; i++ ) {
      pthread_join(th[i],0);
    }
    fin = true;
    pidst end = pidstat(65075);
    pidst diff = end - begin;
    double cpu = (diff.utime + diff.stime) / (diff.timestamp/1000000.0);
    pthread_join(dth,0);
    cout << "\rTIME: " << (diff.timestamp/1000000.0) << " , CPU : " << cpu << endl;
    finish(diff);
  }
  static void * dump_handler(void *a){
    thread_test<ARGTYPE> *THIS = (thread_test<ARGTYPE>*)a;
    for ( typename vector<arg_t *>::iterator it(THIS->args.begin()),itend(THIS->args.end());
            it != itend;
            it++ ){
      cout << endl;
    }
    while (!THIS->fin){
      sleep(1);
      cout << "\033[" << THIS->args.size() << "A\r" << fflush;
      for ( typename vector<arg_t *>::iterator it(THIS->args.begin()),itend(THIS->args.end());
            it != itend;
            it++ ){
        cout << "\033[2K\r" << setw(10) <<(*it)->n << " : " << setw(15) << (*it)->i << endl;
      }
    }
    cout << "\033[" << THIS->args.size() << "A\r" << fflush;
    for ( typename vector<arg_t *>::iterator it(THIS->args.begin()),itend(THIS->args.end());
            it != itend;
            it++ ){
      cout << "\033[2K\r" << endl;
    }
    cout << "\033[" << THIS->args.size() << "A\r" << fflush;
    return 0;
  }
  static void * test_handler(void *a){
    arg_t * arg = (arg_t*)a;
    try { 
      arg->THIS->prepare(arg->n,arg->arg);
      pthread_cond_wait(&arg->THIS->cond,&arg->THIS->mutex);
      pthread_mutex_unlock(&arg->THIS->mutex);
      arg->THIS->test(arg->n,arg->arg,arg->i);
    }catch(...){
      cout << arg->n << " : Fatal error !" << endl;
    }
    return 0;
  }
};

struct base_args {
  DBClientConnection conn;
};

struct insert_args : base_args {
  int from;
  int to;
  int div1;
  int div2;
  const char * value9;
  insert_args (  int from  ,
                 int to    ,
                 int div1  ,
                 int div2  ,
                 const char * value9)
    :from  (from  ),
     to    (to    ),
     div1  (div1  ),
     div2  (div2  ),
     value9(value9){
  }
};

struct insert_test :  thread_test<insert_args> {
  string value9;
  DBClientConnection conn;
  int num;                 
  insert_test( int num, int size , int nthreads = 1 )
    : thread_test<insert_args>(nthreads),
      num(num) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " ===" << endl;

    value9 = "";
    for ( int i = 380 ; i < size ; i++ ) {
      value9 += "H";
    }
    string errmsg;
    if ( ! conn.connect( CONNECT_TO , errmsg ) ) {
      cout << " : Couldn't connect : " << errmsg << endl;
    }
    conn.dropCollection(NS);
  }
  virtual ~insert_test() {
  }
  virtual insert_args * getarg(int n){
    return new insert_args(
      (num/nthreads)*n,
      (num/nthreads)*(n+1),
      2500,
      num/2500,
      value9.c_str());
  }
  virtual void prepare(int n,insert_args * arg) {
    string errmsg;
    if ( ! arg->conn.connect( CONNECT_TO , errmsg ) ) {
      cout << " : Couldn't connect : " << errmsg << endl;
    }
  }
  virtual void test( int n , insert_args * arg , int &ai ) {
    for ( int i = arg->from ; i < arg->to ; i++ ){
      ai = i;
      arg->conn.insert( NS ,
                        BSON( "_id" << i << 
                              "value0" << i << 
                              "value1" << i % arg->div1 << 
                              "value2" << i % arg->div2 << 
                              "value3" << "aaaaaaaaaa" << 
                              "value4" << "bbbbbbbbbbbbbbbbbbbb" << 
                              "value5" << "cccccccccccccccccccccccccccccc" << 
                              "value6" << "dddddddddddddddddddddddddddddddddddddddddddddddddd" << 
                              "value7" << "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee" << 
                              "value8" << i << 
                              "value9" << arg->value9));
    }
  }
  virtual void finish(pidst diff) {
    long long count = 0L;
    long long totalsize = 0L;
    cout << setw(10) << "ALL" << ": COUNT: " << num << "( " << num / (diff.timestamp/1000000.0) << ")" << endl;
  }
};


struct ensure_test :  thread_test<base_args> {
  DBClientConnection conn;
  string fieldname;
  ensure_test( string fieldname )
    : thread_test<base_args>(1),
      fieldname(fieldname) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " ===" << endl;

    string errmsg;
    if ( ! conn.connect( CONNECT_TO , errmsg ) ) {
      cout << " : Couldn't connect : " << errmsg << endl;
    }
  }
  virtual ~ensure_test() {
  }
  virtual base_args * getarg(int n){
    return 0;
  }
  virtual void test( int n , base_args * arg , int &ai ) {
    conn.ensureIndex( NS , BSON( fieldname.c_str() << 1 ));
  }
};

template <typename VALTYPE>
struct update_test :  thread_test<base_args> {
  DBClientConnection conn;
  string fieldname;
  VALTYPE value;
  update_test( string fieldname,VALTYPE value)
    : thread_test<base_args>(1),
      fieldname(fieldname),
      value(value) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " ===" << endl;

    string errmsg;
    if ( ! conn.connect( CONNECT_TO , errmsg ) ) {
      cout << " : Couldn't connect : " << errmsg << endl;
    }
  }
  virtual ~update_test() {
  }
  virtual base_args * getarg(int n){
    return 0;
  }
  virtual void test( int n , base_args * arg , int &ai ) {
    conn.update( NS , BSONObj() ,BSON( "$set" << BSON(fieldname << value) ),false,true);
  }
};

struct query_args : base_args {
  long long totalsize;
  long long count;
  int value;
  query_args(int value) : totalsize(0),count(0),value(value){}
};

struct query_test :  thread_test<query_args> {
  string fieldname;
  query_test( string fieldname , int nthreads = 1 )
    : thread_test<query_args>(nthreads),
      fieldname(fieldname) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " ===" << endl;
  }
  virtual ~query_test() {
  }
  virtual query_args * getarg(int n){
    return new query_args(n);
  }
  virtual void prepare(int n,query_args * arg) {
    string errmsg;
    if ( ! arg->conn.connect( CONNECT_TO , errmsg ) ) {
      cout << " : Couldn't connect : " << errmsg << endl;
    }
  }
  virtual void test( int n , query_args * arg , int &ai ) {
    auto_ptr<DBClientCursor> cursor = arg->conn.query( NS ,BSON( fieldname << arg->value ));
    while ( cursor->more() ) {
      arg->count++;
      BSONObj obj = cursor->next();
      arg->totalsize += obj.objsize();
      ai = (int)arg->count;
    }
  }
  virtual void finish(pidst diff) {
    long long count = 0L;
    long long totalsize = 0L;
    for ( vector<arg_t *>::iterator it(args.begin()),itend(args.end());
            it != itend;
            it++ ){
      // cout << setw(10) <<(*it)->n << ": COUNT: " << (*it)->arg->count << "( " << (*it)->arg->count / (diff.timestamp/1000000.0) << ") , TOTAL_SIZE : " << (*it)->arg->totalsize  << " (" << (*it)->arg->totalsize / (diff.timestamp/1000000.0)<< ")" << endl;
      count += (*it)->arg->count;
      totalsize += (*it)->arg->totalsize;
    }
    cout << setw(10) << "ALL" << ": COUNT: " << count << "( " << count / (diff.timestamp/1000000.0) << ") , TOTAL_SIZE : " << totalsize << " (" << totalsize / (diff.timestamp/1000000.0) << ")" << endl;
  }
};

struct random_test :  query_test {
  int num;
  random_test( string fieldname , int num, int nthreads = 1 )
    : query_test(fieldname,nthreads),
      num(num) {
    int status;
    cout << "=== " << abi::__cxa_demangle(typeid(this).name(), 0, 0, &status) << " ===" << endl;
  }
  virtual void test( int n , query_args * arg , int &ai ) {
    int c = num / 2500;
    int start = arg->value * c;
    int end   = start + c;
    auto_ptr<DBClientCursor> cursor = arg->conn.query( NS ,BSON( fieldname << BSON("$lt" << end << "$gte" << start) ));
    while ( cursor->more() ) {
      arg->count++;
      BSONObj obj = cursor->next();
      arg->totalsize += obj.objsize();
      ai = (int)arg->count;
    }
  }
};


int main ( int argc , char * argv[]  ){
  if ( argc >= 2 ) {
    CONNECT_TO = argv[1];
  }
  if ( argc >= 3 ) {
    NS = argv[2];
  }
  if ( argc >= 4 ) {
    NTHREADS = strtoul(argv[3],0,0);
  }
  if ( argc >= 5 ) {
    DOCSIZE = strtoul(argv[4],0,0);
  }
  if ( argc >= 6 ) {
    NDOCS = strtoul(argv[5],0,0);
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
    insert_test i(NDOCS,DOCSIZE,NTHREADS);
    i.start();
  }
  {
    insert_test i(NDOCS,DOCSIZE);
    i.start();
  }
  {
    ensure_test e("value1");
    e.start();
  }
  {
    ensure_test e("value2");
    e.start();
  }
  {
    update_test<double> u("value8",1);
    u.start();
  }
  {
    update_test<double> u("value8",3);
    u.start();
  }
  {
    update_test<long long> u("value8",0x100000000LL);
    u.start();
  }
  {
    update_test<const char *> u("value5","CCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
    u.start();
  }
  {
    update_test<const char *> u("value7","EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
    u.start();
  }
  {
    query_test q("value1");
    q.start();
  }
  {
    query_test q("value1",10);
    q.start();
  }
  {
    query_test q("value1",50);
    q.start();
  }
  {
    query_test q("value2");
    q.start();
  }
  {
    random_test r("_id",NDOCS);
    r.start();
  }
  {
    random_test r("_id",NDOCS,10);
    r.start();
  }
  {
    random_test r("_id",NDOCS,50);
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
