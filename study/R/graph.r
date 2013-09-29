#!/usr/bin/evn Rscript
data(iris)
# PLOT
plot(iris[,1],iris[,3])
# PLOT VAL
plot(iris[,1],iris[,3],type='n')
text(iris[,1],iris[,3])
# PLOT TXT
plot(iris[,1],iris[,3],type='n')
text(iris[,1],iris[,3],rep(c("S","C","V"),rep(50,3)))
# BIG
plot(iris[,1],iris[,3],type="p",xlab="Length of Sepal", ylab="Length of Petal",cex=2,col="red")
# Color
plot(iris[,1],iris[,3],pch = 21, cex=0.5,bg = c(2, 3, 4)[unclass(iris$Species)],col=c(2,3,4)[unclass(iris$Species)])
# Cloud
library(lattice)
cloud(Sepal.Length ~ Petal.Length * Petal.Width, data = iris,groups=Species,pch=c(16,17,18),cex=0.5,bg = c(2, 3, 4),col=c(2,3,4))
cloud(Sepal.Length ~ Petal.Length * Petal.Width, data = iris,groups=Species,pch=c(16,17,18),cex=0.5,bg = c(2, 3, 4),col=c(2,3,4),screen=list(z=0,x=10,y=-15))

