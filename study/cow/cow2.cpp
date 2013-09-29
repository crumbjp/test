/*
 * cow2.cpp - ????
 *  
 *   Copyright (C) 2012 rakuten 
 *     by hiroaki.kubota <hiroaki.kubota@mail.rakuten.com> 
 *     Date : 2012/04/13
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <memory>
#include <stdexcept>
using namespace std;

namespace cow {
  typedef unsigned int inode_t;
  class FS;
  void ls( const FS &fs, string indent = "  ");
  void ls( const inode_t &inode, string indent = "  ");
  class FS {
  public:
    typedef map<inode_t,FS>  fs_map_t;
    typedef map<string,inode_t> child_map_t;
  public:
    static inode_t  INODE;
    static fs_map_t FS_MAP;
    const inode_t inode;
    inode_t       parent;
  protected:
    child_map_t child_map;
  public:
    FS(const FS &fs):inode(fs.inode){
    }
    FS(const inode_t &parent=0)
      :inode(++INODE),
       parent(parent){
      FS_MAP.insert(fs_map_t::value_type(this->inode,*this));
    }
    virtual ~FS(){
    }
    void set(const string &name , FS &fs){
      child_map_t::iterator it = this->child_map.find(name);
      if ( it != this->child_map.end() ){
        it->second = fs.inode;
      }else{
        this->child_map.insert(child_map_t::value_type(name,fs.inode));
      }
      fs.parent = this->inode;
    }
    typedef vector<inode_t> alist_t;
    void ancestors(alist_t &alist){
      if ( this->parent == 0 ){
        return;
      }
      alist.push_back(this->parent);
      FS::get(this->parent).ancestors(alist);
    }
    const child_map_t& children() const{
      return this->child_map;
    }
    static FS& get(const inode_t &inode){
      fs_map_t::iterator it = FS_MAP.find(inode);
      if ( it != FS_MAP.end() ){
        return it->second;
      }
      throw runtime_error("(FS) not found ");
    }
/*
    static void invalidate(const inode_t &inode){
      fs_map_t::iterator it = FS_MAP.find(inode);
      if ( it != FS_MAP.end() ){
        FS_MAP.erase(it);
      }
    }
*/
  };

  inode_t FS::INODE=0;
  FS::fs_map_t FS::FS_MAP;

  static FS ROOT(0);

  void ls( const FS &fs, string indent){
    const FS::child_map_t &children =  fs.children();
    for( FS::child_map_t::const_iterator it = children.begin(),itend=children.end();
         it != itend;
         it++){
      cout << "---   " << setw(4) << right << FS::get(it->second).parent << " -> " << setw(4) << right << FS::get(it->second).inode << indent << "[" << it->first << "]" << endl;

//       FS *dir = dynamic_cast<FS*>(it->second);
//       File *file = dynamic_cast<File*>(it->second);
//       if ( dir ){
//         cout << "dir   " << setw(4) << right << it->second->parent << " -> " << setw(4) << right << it->second->inode << indent << "[" << it->first << "]" << endl;
//         ls(dir, indent + "  ");
//       }else if(file) {
//         cout << "file  " << setw(4) << right << it->second->parent << " -> " << setw(4) << right << it->second->inode  << indent  << it->first << "        \"" << file->get() << "\"" << endl;
//       }
    }
  }
  void ls( const inode_t &inode, string indent){
    cout << "=== " << inode << " ===" << endl;
    ls(FS::get(inode),indent);
  }
  void dump(){
    for ( FS::fs_map_t::iterator it(FS::FS_MAP.begin()),itend(FS::FS_MAP.end());
          it != itend;
          it++){
      ls(it->first);
    }
  }

}
using namespace cow;
int main ( int argc , char * argv[]  ){
  FS fs;
  // FS::FS_MAP.insert(FS::fs_map_t::value_type(fs.inode,fs));
  cout << FS::FS_MAP.size();
  ROOT.set("root",fs);
  ls(ROOT);
//  FS *root = ROOT.set("root",fs);
/*
  FS *usr = root->mkdir("usr");
  usr->mkdir("local");
  usr->mkdir("bin");
  usr->mkdir("lib");
  FS *tmp = root->mkdir("tmp");
  FS *tmp_sub = tmp->mkdir("sub");
  FS *tmp_sub_sub2 = tmp_sub->mkdir("sub2");
  tmp_sub_sub2->mkdir("T0");
  tmp_sub_sub2->add_file("T1","TEST");
  tmp_sub_sub2->add_file("T2","TEST");
  FS *etc = root->mkdir("etc");
  File *etc_hosts = etc->add_file("hosts","root : : : :");
  root->mkdir("var");
  cout << "------------------" << endl;
  ls(&ROOT);
//  etc_hosts->update("ROOT : : : :");
  tmp_sub->mv("sub2","subMV",etc);
  cout << "------------------" << endl;
  ls(&ROOT);
  cout << "------------------" << endl;
*/
  dump();
  return 0;
}


class FS
class Directory : FS

FS * fs = (Directory*)dir;
FS fs = (Directory)dir;
