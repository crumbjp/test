/*
 * sort.c - ????
 *  
 *   Copyright (C) 2012 rakuten 
 *     by hiroaki.kubota <hiroaki.kubota@mail.rakuten.com> 
 *     Date : 2012/03/01
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dump( const char * const src[] , size_t num ){
  printf("=== dump ===\n");
  for( size_t i = 0; i < num; i ++ ) {
    printf("%02d : %s\n",i,src[i]);
  }
}

int len(const char *c ){
  for(int i = 0;;i++){
    if ( c[i] == 0 ){
      return i;
    }
  }
}
void copy( char *dst , const char *src) {
  int n = len(src);
  for ( int i = 0 ; i < n ; i++ ) {
    dst[i] = src[i];
  }
  dst[n] = 0;
}

int cmp(const char *c1,const char *c2) {
  int l1 = len(c1);
  int l2 = len(c2);
  for ( int i = 0 ; ; i++ ) {
    if ( ! (i < l1 && i < l2) ) {
      return (l2 - l1);
    }
    if ( c1[i] == c2[i] ) {
      continue;
    }
    return c2[i] - c1[i];
  }
  return 0;
}

void sort( const char * src[] , size_t num ) {
  const char *tmp;
  if ( num <= 1 ) {
    return;
  }
  for(int i = 0; i < num -1 ; i++ ) {
    if ( cmp(src[i],src[i+1]) < 0 ) {
      tmp    = src[i];
      src[i] = src[i+1];
      src[i+1] = tmp;
    }
  }
  sort( src , num -1 );
}

void sort_copy( const char * src[] , const char *dst[] , size_t num ) {
  if ( num == 0 ){
    return;
  }
  if ( num == 1 ) {
    copy(dst[0],src[0]);
    return;
  }
  char * center = src[0];
  const char ** lower = malloc(sizeof(char*)*num);
  const char ** upper = malloc(sizeof(char*)*num);
  int lower_c = 0;
  int upper_c = 0;
  for(int i = 1; i < num ; i++ ) {
    if ( cmp(center,src[i] ) < 0 ) {
      lower[lower_c++] = src[i];
    }else{
      upper[upper_c++] = src[i];
    }
  }
  sort_copy(lower,dst,lower_c);
  copy(dst[lower_c],center);
  sort_copy(upper,dst+lower_c+1,upper_c);

  free(lower);
  free(upper);
}

int main ( int argc , char * argv[]  ){
  const char *src[] = {"C9","B1","A1001","a001","a100","A100","A001"};
  dump(src,sizeof(src)/sizeof(char*));
  sort(src,sizeof(src)/sizeof(char*));
  dump(src,sizeof(src)/sizeof(char*));

  int num = sizeof(src)/sizeof(char*);
  const char **dst = (const char**)malloc(sizeof(char*)*num);

  for(int i = 0 ; i < num ; i++ ) {
    dst[i] = malloc(sizeof(char)*10);
  }

  // malloc
  sort_copy(src,dst,sizeof(src)/sizeof(char*));
  // free

  dump(dst,sizeof(src)/sizeof(char*));
  return 0;
}
// gcc -m32 -std=c99 sort.c ; ./a.out
