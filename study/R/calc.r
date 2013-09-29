#!/usr/bin/evn Rscript
message("== Data ==")
B<-matrix(0:24,5,5)
colnames(B)<-c('C1','C2','C3','C4','C5')
rownames(B)<-c('R1','R2','R3','R4','R5')
B[1,2]<-99
message("== B ==")
B
message("== B*2 ==")
B*2
message("== B+matrix(100:129,5,5) ==")
B+matrix(100:129,5,5)
message("== matrix(c(10,20,30,40,50),5,1)*c(1,2,3,4,5) ==")
matrix(c(10,20,30,40,50),5,1)*c(1,2,3,4,5)
message("== B*c(1,2,3,4,5) ==")
B*c(1,2,3,4,5)

message("== SUM col ==")
apply(B,1,sum)
message("== MEAN col ==")
apply(B,1,mean)
message("== SUM row ==")
apply(B,2,sum)
message("== MY row ==")
my<-function(x){
  x-mean(x)
}
apply(B,2,my)
