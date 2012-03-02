/*
 * marubatu.c - ????
 *  
 *   Copyright (C) 2012 rakuten 
 *     by hiroaki.kubota <hiroaki.kubota@mail.rakuten.com> 
 *     Date : 2012/03/02
 */

#include <stdio.h>
#include <string.h>

char bord[3][3] = { 0,0,1,-1,0,1,-1,0,0};

void view(){
  printf("V:\n");
  printf("+------+\n");
  for( int i = 0;i < 3; i++){
    printf("|");
    for( int j = 0;j < 3; j++){
      if ( bord[i][j] == 0 ) {
        printf("  ");
      }else if ( bord[i][j] == 1 ) {
        printf("o ");
      }else{
        printf("x ");
      }
    }
    printf("|\n");
  }
  printf("+------+\n");
}

// required format "x,y"
void proc(const char *in){
  printf("P:%s\n",in);
  // parse();
  // put();
  // turn();
  // judge_end();
}
void cui(){
  while(1){
    view();

    char buf[BUFSIZ];
    const char *input = gets(buf);
    printf("I:%s\n",input);
    if ( strlen(input) == 0 ){
      break;
    }
    
    proc(input);
  }
}

int main ( int argc , char * argv[]  ){
  cui();
  return 0;
}
