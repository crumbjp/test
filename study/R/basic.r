#!/usr/bin/evn Rscript
# Vector
message("== Vector ==")
A<-c(1,2,3,4,5)
print(A)
A<-1:5
print(A)
print(length(A))
# Label
message("== Labeling ==")
names(A)<-c('aa','bb','cc','dd','ee')
print(A)
# Matrix
message("== Matrix ==")
B<-matrix(0:9,2,5)
message("== dim(B) ==")
print(dim(B))
message("== B ==")
print(B)
B[1,2]<-99
message("== B[1,] ==")
B[1,]
message("== B[,3] ==")
B[,3]
print(B)
# Matrix label
message("== Matrix labeling ==")
colnames(B)<-c('aa','bb','cc','dd','ee')
rownames(B)<-c('XX','YY')
message("== B(labeled) ==")
print(B)
# Array
message("== Array ==")
C<-array(1:30,c(2,5,3))
print(C)
print(C[1,4,2])
# List
message("== List ==")
D<-list(A,B,C)
print(D)
print(D[[2]])
print(D[[3]][2,5,3])

# Write
message("== Write ==")
write(A,"A.txt")
write.table(t(A),"At.txt")
write(B,"B.txt")
write.table(B,"Bt.txt")
write(C,"C.txt")

for(i in 1:dim(C)[3]){
      write(t(C[,,i]),paste("Ct",i,".txt",sep=""))
}
save(D,file="Ds.txt")

# Read
message("== Read ==")
read.table("At.txt")
read.table("Bt.txt")
matrix(scan("B.txt"),2,5,byrow=T)
data.frame(scan("B.txt",what=list(aa=0,bb=0,cc=0,dd=0,ee=0)))

Cr<-array(0,c(2,5,3))
Cr[,,1]=matrix(scan("Ct1.txt"),2,5,byrow=T)
Cr[,,2]=matrix(scan("Ct2.txt"),2,5,byrow=T)
Cr[,,3]=matrix(scan("Ct3.txt"),2,5,byrow=T)
print(Cr)

rm(D)
load("Ds.txt")
print(D)
