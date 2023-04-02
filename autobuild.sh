# 自动编译脚本
#!/bin/bash
set -x

rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make