#!/bin/bash

cd ../UR_Agent
find . -name '*.h' -or -name '*.hpp' -or -name '*.cpp' | xargs clang-format-3.8 -i ../bin/clang-format.sql  $1
cd ../NIST
find . -name '*.h' -or -name '*.hpp' -or -name '*.cpp' | xargs clang-format-3.8 -i ../bin/clang-format.sql  $1
