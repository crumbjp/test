/*
 * mincore.cc - ????
 *  
 *   Copyright (C) 2013 rakuten 
 *     by Hiroaki.Kubota <hiroaki.kubota@mail.rakuten.com> 
 *     Date : 2013/04/03
 */

#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
using namespace std;

//static size_t SIZE=20*1024*1024*1024;
//static size_t SIZE=1*1024*1024;
static size_t SIZE = (size_t)4*1024*1024*1024;
void get_proc_stat_vsize();
void mincore_size(void * p);
int main ( int argc , char * argv[]  ){
  if ( argc != 2 ) {
    return 1;
  }
  unsigned int offset = strtoul(argv[1],0,0);
  FILE *fp =  fopen("_MAPFILE", "a+b");
  if ( ! fp ) {
    cout << "fopen" << endl;
    perror(0);
    return 1;
  }
  int fd = fileno(fp);
  get_proc_stat_vsize();
  cout << "mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd, " << offset << "GB )" << endl;
  void *p = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd, ((size_t)offset*1024*1024*1024) );
  // MAP_NORESERVE
  // void *p = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_NORESERVE,fd, 0);
  if ( ! p || p == (void*)-1 ) {
    cout << "mmap" << endl;
    perror(0);
    return 1;
  }
  get_proc_stat_vsize();
  mincore_size(p);
  unsigned char *c = (unsigned char *)p;
  for ( size_t i = 0 ; i < SIZE/4096 ; i++ ) {
     c[i*4096] = i;
     if ( ! ( (i * 4096) % (256 * 1024 * 1024) ) ){
       cout << "== " << i * 4096 / 1024 / 1024 << " (MB) ==" << endl;
       get_proc_stat_vsize();
       mincore_size(p);
     }
  }
  get_proc_stat_vsize();
  mincore_size(p);
  munmap(p,SIZE);
  return 0;
}
void mincore_size(void * p){
  vector<unsigned char> vec;
  size_t num_pages = SIZE/4096;
  vec.resize(num_pages,0);
  int ret = mincore(p, SIZE, &vec[0]);
  if ( ret == -1  ) {
    cout << "mincore" << endl;
    perror(0);
  }
  size_t count = 0;
  for ( vector<unsigned char>::iterator it(vec.begin()),itend(vec.end());
        it != itend;
        it++ ){
    count += (*it & 0x1);
  }
  cout << "mincore: "  << count << "(pages) : " << count*4096/1024/1024 << "(MB)" << endl;
}
void get_proc_stat_vsize(){
  FILE *fp = fopen("/proc/self/stat", "r");
  if ( ! fp )   {
    perror(0);
  }

  char line[BUFSIZ];
  fgets(line, BUFSIZ, fp);
  fclose(fp);
  char *p = line;
  for ( int i = 0 ; i < 22; i++ ){
    p = strchr(p,' ')+1;
  }
  char *vss = p;
  p = strchr(p,' ')+1;
  char *rss = p;
  unsigned long vsize = strtoul(strtok(vss," "),0,0);
  unsigned long rsize = strtoul(strtok(rss," "),0,0)*4096;
  cout << "|vss : " << vsize <<  " : " <<  vsize/1024/1024 << " (MB) ";
  cout << "|rss : " << rsize <<  " : " <<  rsize/1024/1024 << " (MB) ";
  cout << endl;
//  return (unsigned long)atol(items[22]);
}

