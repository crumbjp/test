/*
 * string.c - ????
 *  
 *   Copyright (C) 2012 rakuten 
 *     by hiroaki.kubota <hiroaki.kubota@mail.rakuten.com> 
 *     Date : 2012/03/01
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ( int argc , char * argv[]  ){
  char str[] = "abcefgh";
  char *str1 = "abcefgh";
  char *str2 = "abcefgh"; 
//  str1=str;
//  str1[3]=0;

  printf("%d %d\n",strlen(str),sizeof(str)); 
  int len = strlen(str);
  char *dst = (char*)malloc((size_t)sizeof(char)*(len+1));
  for(int i = 0 ; i < len + 1 ; i++ ) {
    dst[i] = str[i];
  }
  printf("%s\n%s\n",str,dst); 
  

  printf("%d %d %d\n",sizeof(str),sizeof(str1),sizeof(dst)); 
  printf("%08x %08x %08x o%08x\n",str,str1,dst,str2); 

  return 0;
}
