#!/usr/bin/evn Rscript
library(randomForest)
data(iris)
d<-iris
s <- sample(nrow(d),nrow(d)*0.5)
d.sample <- d[s,]
d.test <- d[-s,]

forest <- randomForest(Species~.,data=d.sample,ntree=500,importance=T)
forest
plot(forest)
varImpPlot(forest)
importance(forest)

predforest  <- predict(forest,d.test,type="class")
table(predforest,d.test$Species)
