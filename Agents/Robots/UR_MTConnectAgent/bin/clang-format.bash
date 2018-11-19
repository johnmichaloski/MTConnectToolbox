#!/bin/bash

CWD="$(pwd)"
SAVEDWD="$(pwd)"
echo $CWD
function clangit(){
local FILES="*.cpp"
for f in $FILES
do
echo "cpp=${f}" 
	clang-format-3.4 -i -style=$SAVEDWD/clang-format.sql $f 
done

FILES="*.h"
for f in $FILES
do
echo "*h=${f}"
	clang-format-3.4 -i -style==$SAVEDWD/clang-format.sql  $f 
done
}

cd ../NIST
clangit $CWD
cd ../UR_MTConnectAgent
clangit $CWD
