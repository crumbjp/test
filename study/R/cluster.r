#!/usr/bin/evn Rscript
library(rpart)
library(randomForest)
library(e1071)
data(iris)
d<-iris
plot(d[,1], d[,3], col=c(2,3,4)[unclass(d$Species)],main="Origin")

message("=== RandomForest ===")
forest <- randomForest(Species~.,data=d,ntree=500,importance=T)
predforest  <- predict(forest,d,type="class")
table(d$Species,predforest)
plot(d[,1], d[,3], col=c("red","blue","green")[unclass(predforest)],main="Forest")

message("=== Rpart ===")
tree <- rpart(Species~.,data=d,method="class")
predtree  <- predict(tree,d,type="class")
table(d$Species,predtree)
plot(d[,1], d[,3], col=c("red","blue","green")[unclass(predtree)],main="Rpart")

message("=== Cmeans ===")
r<-cmeans(d[,-5],3,150,verbose=F,method="cmeans",m=2)
c<-apply(r$membership,1,function(x){if ( max(x) < 0.6 ) { which.max(x)+10 }else{ which.max(x)}} )
table(d$Species,c)
c<-apply(r$membership,1,function(x){rgb(x[1],x[2],x[3])})
plot(d[,1], d[,3], col=c,main="Cmeans")

message("=== Ufcl ===")
r<-cmeans(d[,-5],3,150,verbose=F,method="ufcl",m=2)
c<-apply(r$membership,1,function(x){if ( max(x) < 0.6 ) { which.max(x)+10 }else{ which.max(x)}} )
table(d$Species,c)
c<-apply(r$membership,1,function(x){rgb(x[1],x[2],x[3])})
plot(d[,1], d[,3], col=c,main="Ufcl")

message("=== Kmeans hartigan-wong ===")
k<-kmeans(d[,-5],3,iter.max=10,nstart=1,algorithm="Hartigan-Wong")
table(d$Species,k$cluster)
plot(d[,1], d[,3], col=k$cluster,main="Kmeans hartigan-wong")

message("=== average ===")
h<-hclust(dist(d[,-5]),method = "average")
table(d$Species,cutree(h,3))
plot(d[,1], d[,3], col=cutree(h,3),main="average")
h<-hclust(dist(d[,-5])^2,method = "average")
table(d$Species,cutree(h,3))
plot(d[,1], d[,3], col=cutree(h,3),main="average^2")
message("=== complete ===")
h<-hclust(dist(d[,-5]),method = "complete")
table(d$Species,cutree(h,3))
plot(d[,1], d[,3], col=cutree(h,3),main="complete")
h<-hclust(dist(d[,-5])^2,method = "complete")
table(d$Species,cutree(h,3))
plot(d[,1], d[,3], col=cutree(h,3),main="complete^2")
message("=== median ===")
h<-hclust(dist(d[,-5]),method = "median")
table(d$Species,cutree(h,3))
plot(d[,1], d[,3], col=cutree(h,3),main="median")
message("=== ward ===")
h<-hclust(dist(d[,-5]),method = "ward")
table(d$Species,cutree(h,3))
plot(d[,1], d[,3], col=cutree(h,3),main="ward")
h<-hclust(dist(d[,-5])^2,method = "ward")
table(d$Species,cutree(h,3))
plot(d[,1], d[,3], col=cutree(h,3),main="ward^2")

