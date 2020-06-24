#/bin/bash

echo "Formatting CMakeLists"
cmake-format -c .cmake-format -i $(find ./ -name CMakeLists*)

echo "Formatting C++"
clang-format -i $(rg --files | rg -e ".*.cpp|.*.hpp")
