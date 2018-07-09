ALL: bt1 bt2 res-bt1/out res-bt1/out-mrgd bt1-consensus-ss

bt2: bt2.cc
	g++ bt2.cc -o bt2

bt1: bt1.cc
	g++ bt1.cc -o bt1

blast: blast.cc
	g++ blast.cc -o blast

res-bt1/out: bt1 
	./bt1 data/hud.txt > res-bt1/out 2>err

res-bt1/out-mrgd: bt1-remove-non-maximal.py res-bt1/out
	python bt1-remove-non-maximal.py < res-bt1/out > res-bt1/out-mrgd 2>err

bt1-consensus-ss: bt1-consensus-ss.cc
	g++ -O2 bt1-consensus-ss.cc -o bt1-consensus-ss

