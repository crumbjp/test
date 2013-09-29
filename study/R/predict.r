#!/usr/bin/evn Rscript
library(rpart)
library(rpart.plot)
data(iris)
d<-iris
s <- sample(nrow(d),nrow(d)*0.8)
d.sample <- d[s,]
d.test <- d[-s,]


tree <- rpart(Species~.,data=d.sample,method="class")
predtree  <- predict(tree,d.test,type="class")

table(predtree,d.test$Species)
rpart.plot(tree,yesno=F,extra=101,type=4,faclen=15,varlen=15,fallen.leaves=T,snip=T)
