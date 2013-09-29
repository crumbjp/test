#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <zookeeper.h>
using namespace std;
#ifdef ZOO_TEST
# define debug(msg) cerr << __PRETTY_FUNCTION__ << ":"  << __LINE__ << ": " << msg << endl
#else
# define debug(msg) 
#endif
class Zoo {
  zhandle_t *zk;
  string zoo_host;
  string prefix;
  struct ACL_vector aclv;
  struct ACL acl;
public:
  Zoo( const string &zoo_host , const string &prefix ) 
    : zk(0),
      prefix(prefix),
      zoo_host(zoo_host) {
    debug(zoo_host);
  }
  void init () {
    if ( !this->zk ){
      debug("");
      this->zk = zookeeper_init(zoo_host.c_str(),0,5000,0,0,0);
      if ( ! this->zk ) {
        debug("could not init zookeeper instance");
        return;
      }
      if ( ! this->prefix.empty() ) {
        debug(this->prefix);
        if ( ! this->is_exist() ) {
          this->create_dir();
        }
      }
    }
    aclv.data = &acl;
    aclv.count = 1;
    acl.perms = ZOO_PERM_ALL;
    acl.id.id = "anyone";
    acl.id.scheme = "world";
  }
  bool is_valid() {
    debug(zoo_host);
    return this->zk;
  }
  void close () {
    if ( this->is_valid() ) {
      zookeeper_close(this->zk);
      this->zk = 0;
    }
  }
  bool is_exist(const string &repset = "" , const string &name = "" ) {
    if ( this->is_valid() ) {
      string target = this->prefix + (repset.empty()?"":("/"+repset)) + (name.empty()?"":("/"+name));
      struct Stat stat;
      int ret = zoo_wexists(this->zk, target.c_str(), 0 , 0 , &stat);
      if ( ret != ZOK && ret != ZNONODE ) {
        this->close();
        debug("zoo_wexists error " << ret << " => " << target);
        return false;
      }
      if ( ret == ZNONODE ) {
        return false;
      }
      return true;
    }
    return false;
  }
  void create_dir(const string &repset = "" ){
    if ( this->is_valid() ) {
      string target = this->prefix + (repset.empty()?"":("/"+repset));
      debug(" => " << target);
      char realpath[256];
      int realpath_max = 256;
      int ret = zoo_create(this->zk, target.c_str(), "", 0 , &aclv,0,realpath, realpath_max);
      if ( ret != ZOK ) {
        this->close();
        debug("zoo_create error " << ret << " => " << target);
        return;
      }
    }
  }

  bool regist_node(const string &repset, const string &node , const string &status ){
    if ( node.empty() ) {
      return true;
    }
    string target = this->prefix + "/" + repset + "/" + node + "#" + status;
    if ( this->is_valid() ) {
      debug(" => " << target);
      char realpath[256];
      int realpath_max = 256;
      int ret = zoo_create(this->zk, target.c_str(), "", 0 , &aclv,ZOO_EPHEMERAL,realpath, realpath_max);
      if ( ret != ZOK ) {
        this->close();
        debug("zoo_create error " << ret << " => " << target);
      }
      return true;
    }
    return false;
  }
  void delete_node(const string &repset, const string &node){
    if ( this->is_valid() ) {
      string target = this->prefix + (repset.empty()?"":("/"+repset));
      long version = -1;
      int ret = zoo_delete(this->zk, target.c_str(), version);
      if ( ret != ZOK ) {
        this->close();
        debug("zoo_create error " << ret << " => " << target);
      }
    }
  }
};
extern "C" {
  struct mongostat {
    string repset;
    string name;
    string status;
    mongostat(const char * repset="",const char * name="" , const char * status="")
      :repset(repset),
       name(name),
       status(status){
    }
    bool operator!=(const mongostat &t) {
      return ( (repset.compare(t.repset)) ||
               (name.compare(t.name)) ||
               (status.compare(t.status)));
    }
  };
  
  typedef vector<Zoo*> zoo_list_t;
  static mongostat current;
  static mongostat last;
  static const char * g_arg;
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  void *zoothread(void *){
    string a = g_arg;
    string::size_type pos = a.find("/");
    string zoo_hosts = a.substr(0,pos);
    pos++;
    string prefix = a.substr(pos);

    zoo_list_t zoo_list;
    string::size_type hpos=0;
    for( ; ;){
      string::size_type pos = zoo_hosts.find(",",hpos);
      if ( pos == string::npos ) {
        zoo_list.push_back(new Zoo(zoo_hosts.substr(hpos),prefix));
        break;
      }
      zoo_list.push_back(new Zoo(zoo_hosts.substr(hpos,pos-hpos),prefix));
      hpos = pos+1;
    }
    if ( zoo_list.size() ) {
      Zoo *zoo = 0;
      
      for (zoo_list_t::iterator it(zoo_list.begin()),itend(zoo_list.end());; it++) {
        if ( it == itend ) {
          it = zoo_list.begin();
        }
        zoo = *it;
        for ( ; ; ) {
          sleep(1);
          pthread_mutex_lock(&mutex);
          mongostat c = current;
          pthread_mutex_unlock(&mutex);
          if ( c != last ) {
            zoo->close();
            zoo->init();
            if ( ! zoo->is_valid() ) {
              break;
            }
            if ( ! c.repset.empty() && ! c.name.empty() ) {
              debug(c.repset << " : " << c.name << " : "  << c.status);
              if ( ! zoo->is_exist(c.repset) ) {
                zoo->create_dir(c.repset);
              }
              if ( ! zoo->is_exist(c.repset,c.name) ) {
                if ( zoo->regist_node(c.repset,c.name,c.status) ) {
                  last = c;
                }
              }
            }
          }
        }
      }
    }
    return 0;
  }
  void zooinit(){
    debug("");
    pthread_t ph;
    pthread_create(&ph, 0, zoothread,0); 
  }

  static pthread_once_t once_control = PTHREAD_ONCE_INIT;
  void zoohook ( const char *repset, const char * name , const char * status , const char * arg  ){
    g_arg = arg;
    pthread_once(&once_control, zooinit ); 
    mongostat now = mongostat(repset?repset:"",name?name:"",status?status:"");
    if ( now != current ) {
      {
        pthread_mutex_lock(&mutex);
        current = now;
        pthread_mutex_unlock(&mutex);
      }
    }
  }
}

#ifdef ZOO_TEST
int main ( int argc, const char * argv[] ) {
//   zoohook(0,0,"TEST","127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
//   zoohook("T1","127.0.0.1:9999","TEST","127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
//   sleep(1);
  zoohook(0,0,0,"127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
//   zoohook("T1","127.0.0.1:9999","TEST","127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
  for ( int i = 0 ; i < 30 ; i++ ) {
    zoohook("T1","127.0.0.1:9999","PRIMARY","127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
    sleep(1);
    zoohook("T1","127.0.0.1:9999","PRIMARY","127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
    sleep(1);
    zoohook("T1","127.0.0.2:9999","PRIMARY","127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
    sleep(1);
    zoohook("T1","127.0.0.2:9999","PRIMARY","127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
    sleep(1);
    zoohook("T1","127.0.0.3:9999","PRIMARY","127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
    sleep(1);
    zoohook("T1","127.0.0.3:9999","SECONDARY","127.0.0.1:2183,127.0.0.1:2182,127.0.0.1:2181//test");
    sleep(1);
  }
  
  return 0;
}
#endif
