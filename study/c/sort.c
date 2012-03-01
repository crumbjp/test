/*
 * sort.c - ????
 *  
 *   Copyright (C) 2012 rakuten 
 *     by hiroaki.kubota <hiroaki.kubota@mail.rakuten.com> 
 *     Date : 2012/03/01
 */

#include <stdio.h>

void dump( char * const src[] , size_t num ){
  printf("=== dump ===\n");
  for( size_t i = 0; i < num; i ++ ) {
    printf("%02d : %s\n",i,src[i]);
  }
}

void sort( char * const src[] , size_t num ) {
}

void sort_copy( char * const * src , char **dst , size_t num ) {
}

int main ( int argc , char * argv[]  ){
  char *src[] = {"C9","B1","a001","a100","A100","A001"};
  dump(src,sizeof(src)/sizeof(char*));
  sort(src,sizeof(src)/sizeof(char*));
  dump(src,sizeof(src)/sizeof(char*));
  char **dst;

  // malloc
  sort_copy(src,dst,sizeof(src)/sizeof(char*));
  // free

//  dump(dst,sizeof(src)/sizeof(char*));
  return 0;
}
