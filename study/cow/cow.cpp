/*
 * cow.cpp - ????
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
  class Directory;
  void ls( const Directory *dir, string indent = "  ");
  void ls( const inode_t &inode, string indent = "  ");

  class FS {
  public:
    typedef map<inode_t,FS*>  fs_map_t;
  protected:
    static inode_t  INODE;
    static fs_map_t FS_MAP;
  public:
    inode_t       parent;
    const inode_t inode;
    FS(const inode_t &parent=0)
      :inode(++INODE),
       parent(parent){
      FS_MAP.insert(fs_map_t::value_type(this->inode,this));
    }
    void invalidate(){
      fs_map_t::iterator it = FS_MAP.find(this->inode);
      if ( it != FS_MAP.end() ){
        FS_MAP.erase(it);
      }
    }
    static FS * get(const inode_t &inode){
      fs_map_t::iterator it = FS_MAP.find(inode);
      if ( it != FS_MAP.end() ){
        return it->second;
      }
      throw runtime_error("(FS) not found ");
    }
    typedef vector<inode_t> alist_t;
    void ancestors(alist_t &alist){
      if ( this->parent == 0 ){
        return;
      }
      alist.push_back(this->parent);
      FS::get(this->parent)->ancestors(alist);
    }
    virtual ~FS(){
      invalidate();
    }
  };
  inode_t FS::INODE=0;
  FS::fs_map_t FS::FS_MAP;
  
  class File : public FS {
    string value;
  public:
    File(const inode_t & parent,const string &value = ""):FS(parent),value(value){
    }
    File(const File &in):FS(in.parent){
      this->value = in.value;
    }
    string get() {
      return this->value;
    }
    void update(const string &value) {
      this->value = value;
    }
    virtual ~File(){
    }
  };
  
  class Directory : public FS {
  public:
    typedef map<string,inode_t> child_map_t;
  protected:
    child_map_t child_map;
    
    void add(const string &name , FS *node){
      child_map_t::iterator it = this->child_map.find(name);
      if ( it != this->child_map.end() ){
        throw logic_error("(B) already exists");
      }
      this->child_map.insert(child_map_t::value_type(name,node->inode));
      node->parent = this->inode;
    }
  public:
    Directory(const inode_t & parent):FS(parent){
    }
    Directory(const Directory &in):FS(in.parent){
      this->child_map = in.child_map;
    }
    File * add_file(const string &name , const string &value){
      auto_ptr<File> p(new File(this->inode,value));
      this->add(name,p.get());
      return p.release();
    }
    Directory * mkdir(const string &name){
      auto_ptr<Directory> p(new Directory(this->inode));
      this->add(name,p.get());
      return p.release();
    }

    void cow_child_map(const FS *present,const FS *new_node){
      // should use index instead of all scan.
      for ( child_map_t::iterator it = this->child_map.begin(),itend = this->child_map.end();
            it != itend;
            it++ ){
        if ( it->second == present->inode){
          it->second = new_node->inode;
          return;
        }
      }
      throw runtime_error("(B) not found in children : ");
    }

    typedef vector<FS*> clist_t;
    void cow(const FS *present,const FS *new_node, Directory *top,clist_t &clist){
      cout << "COW: (" << this->inode << ") " << present->inode << " , " << new_node->inode << " / " << top->parent << endl;
      if ( this->parent == top->parent ) {
        top->cow_child_map(present,new_node);
        return;
      }
      Directory *new_this = new Directory(*this);
      new_this->cow_child_map(present,new_node);
      clist.push_back(this);
      dynamic_cast<Directory*>(FS::get(this->parent))->cow(this,new_this,top,clist);
    }
    void mv(const string &name , const string &new_name , Directory * target ){
      child_map_t::iterator it = this->child_map.find(name);
      if ( it == this->child_map.end() ){
        throw logic_error("(B) not found : " + name);
      }
      // Normal
      // target->add(new_name,it->second);
      // this->child_map.erase(it);
      // COW
      Directory *new_dir  = new Directory(*target);
      Directory *new_this = new Directory(*this);
      new_dir->add(new_name,FS::get(it->second));
      new_this->rm(it->first);

      FS::alist_t nd_alist;
      this->ancestors(nd_alist);
      FS::alist_t nt_alist;
      target->ancestors(nt_alist);
      for(FS::alist_t::iterator it = nd_alist.begin(), itend = nd_alist.end();
          it != itend;
          it++){
        for(FS::alist_t::iterator itt = nt_alist.begin(), ittend = nt_alist.end();
            itt != ittend;
            itt++){
          if ( *itt == *it ) {
            clist_t clist;
            Directory * old_top = dynamic_cast<Directory*>(FS::get(*it));
            cout << "###### ORIGIN ##########" << endl;
            ls(old_top->inode);
            Directory * top = new Directory(*old_top);
            cout << "###### TMP ##########" << endl;
            ls(top->inode);
            dynamic_cast<Directory*>(FS::get(target->parent))->cow(target,new_dir,top,clist);
            dynamic_cast<Directory*>(FS::get(new_this->parent))->cow(this,new_this,top,clist);
            cout << "###### TMP ##########" << endl;
            ls(top->inode);
            cout << "###### ROOT ##########" << endl;
            ls(1);
            dynamic_cast<Directory*>(FS::get(top->parent))->cow_child_map(old_top,top);
            cout << "###### ROOT ##########" << endl;
            ls(1);
            for(clist_t::iterator itc = clist.begin(), itcend = clist.end();
                itc != itcend;
                itc++){
              delete (*itc);
            }
            delete old_top;
            delete target;
            delete this;
            return;
          }
        }
      }
    }
    void rm(const string &name){
      child_map_t::iterator it = this->child_map.find(name);
      if ( it == this->child_map.end() ){
        throw logic_error("(B) not found " + name);
      }
      this->child_map.erase(it);
    }
    FS * get(const string & name ) {
      child_map_t::iterator it = this->child_map.find(name);
      if ( it == this->child_map.end() ){
        throw logic_error("(B) not found : " + name);
      }
      return FS::get(it->second);
    }

    typedef map<string,FS* >    child_list_t;
    child_list_t children() const{
      child_list_t ret;
      for ( child_map_t::const_iterator it = this->child_map.begin(),itend = this->child_map.end();
            it != itend;
            it++ ){
        ret.insert(child_list_t::value_type(it->first,FS::get(it->second)));
      }
      return ret;
    }
    virtual void invalidate(){
      if ( this->child_map.size() != 0 ){
        throw logic_error("(B) still have children : ");
      }
      this->FS::invalidate();
    }
    virtual ~Directory(){
    }
  };
  static Directory ROOT(0);

  void ls( const Directory *dir, string indent){
    Directory::child_list_t children =  dir->children();
    for( Directory::child_list_t::iterator it = children.begin(),itend=children.end();
         it != itend;
         it++){
      Directory *dir = dynamic_cast<Directory*>(it->second);
      File *file = dynamic_cast<File*>(it->second);
      if ( dir ){
        cout << "dir   " << setw(4) << right << it->second->parent << " -> " << setw(4) << right << it->second->inode << indent << "[" << it->first << "]" << endl;
        ls(dir, indent + "  ");
      }else if(file) {
        cout << "file  " << setw(4) << right << it->second->parent << " -> " << setw(4) << right << it->second->inode  << indent  << it->first << "        \"" << file->get() << "\"" << endl;
      }
    }
  }
  void ls( const inode_t &inode, string indent){
    cout << "=== " << inode << " ===" << endl;
    Directory * dir = dynamic_cast<Directory*>(FS::get(inode));
    if ( dir ) {
      ls(dir,indent);
    }
  }

  class DUMMY : FS {
  public:
    static void dump(){
      for ( fs_map_t::iterator it(FS_MAP.begin()),itend(FS_MAP.end());
            it != itend;
            it++){
        ls(it->first);
      }
    }
  };
  void dump(){
    DUMMY::dump();
  }
}

using namespace cow;
int main ( int argc , char * argv[]  ){
  Directory *root = ROOT.mkdir("/");
  Directory *usr = root->mkdir("usr");
  usr->mkdir("local");
  usr->mkdir("bin");
  usr->mkdir("lib");
  Directory *tmp = root->mkdir("tmp");
  Directory *tmp_sub = tmp->mkdir("sub");
  Directory *tmp_SUB = tmp->mkdir("SUB");
  Directory *tmp_sub_sub2 = tmp_sub->mkdir("sub2");
  tmp_sub_sub2->mkdir("T0");
  tmp_sub_sub2->add_file("T1","TEST");
  tmp_sub_sub2->add_file("T2","TEST");
  Directory *etc = root->mkdir("etc");
  File *etc_hosts = etc->add_file("hosts","root : : : :");
  root->mkdir("var");
  cout << "------------------" << endl;
  ls(&ROOT);
//  etc_hosts->update("ROOT : : : :");
  tmp_sub->mv("sub2","subMV",tmp_SUB);
  cout << "------------------" << endl;
  ls(&ROOT);
  cout << "------------------" << endl;
  dump();
  return 0;
}
