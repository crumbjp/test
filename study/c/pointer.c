#include <stdio.h>

int* funcQ1( int *i){
  return i;
}
//?? funcQ2(???) {}

void q1(){
  int i[10] = { 1,2,3,4,5,6,7,8,9,10};

  for ( int k = 0 ; k < 10; k++ )
    printf("%d\n",i[k]);

  int *j = funcQ1(i);

  for ( int k = 0 ; k < 10; k++ )
    printf("%d\n",j[k]);

  printf("%08x != %08x\n",i,j);
}

void q2(){
  int i[10][10];
  for ( int k = 0 ; k < 10; k++ )
    for ( int l = 0 ; l < 10; l++ )
      i[k][l] = k*l;
  for ( int k = 0 ; k < 10; k++ )
    for ( int l = 0 ; l < 10; l++ )
      printf("%d\n",i[k][l]);
  // int j = funcQ2(i);
  for ( int k = 0 ; k < 10; k++ )
    for ( int l = 0 ; l < 10; l++ )
      printf("%d\n",j[k][l]);
  printf("%08x != %08x\n",i,j);
}


int main ( int argc , char * argv[] ){
  q1();
  q2();
}
