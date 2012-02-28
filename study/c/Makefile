# all: 
# install:
# uninstall:
# clean:
# 
# これらはターゲット(行頭->コロンまで)
# 
# ターゲットからターゲットまでの間がそのターゲットをビルドする処理
# 
# 処理の行頭はタブが必須、処理はshellライクに記述する
# 
# 
# $ make <target>
# 　のように起動するが、<target>を省略した場合は先頭のターゲットを実行する
# 　よって普通はallターゲットを一番上に書いておく
# 
# ターゲットはターゲットに依存できる
# 
# install:all
# この場合は、installターゲットの実行の前にallターゲットの実行が必要
# 
# ターゲットは実ファイルであるのが普通
# 慣例的にall,install,uninstall,clean などは使われている
# 
# 実ファイルを指定した場合は、タイムスタンプで新旧を判定し実行をスキップできる
# 
# 以下を実行して理解すること
# -- new --
# $ make clean
# $ make target.test
# -- retry --
# $ make target.test
# -- no change required --
# $ rm target.test
# $ make target.test
# -- update required.test --
# $ touch required.test
# $ make target.test
# 
#
# いくつか書式があるがあまり理解しなくても良い
# $ make test.test
# 　の辺りが理解できればとりあえず十分
#

all:target.test
install:all
clean:
	rm *.test
	rm *.o
	rm *.out

target.test:required.test
	echo 'target.test'
	touch target.test

required.test:
	echo 'required.test'
	touch required.test

test.test:a.test b.test
	echo $@
	echo $<
	echo $^

a.test:
	touch $@
b.test:
	touch $@
