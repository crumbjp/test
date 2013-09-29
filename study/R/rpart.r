#!/usr/bin/evn Rscript
library(rpart.plot)
data(iris)
tree <- rpart(Species~.,data=iris,method="class")
rpart.plot(tree,yesno=F,extra=101,type=4,faclen=15,varlen=15)
rpart.plot(tree,yesno=F,extra=101,type=4,faclen=15,varlen=15,fallen.leaves=T,snip=T)

rpart.plot(tree,branch.type=4,yesno=F,faclen=15,varlen=15,extra=101)

