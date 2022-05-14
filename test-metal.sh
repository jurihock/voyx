#!/bin/bash

SOURCE=$(dirname "$0")

if [[ ${1} == "xcode" ]]; then

  OPTIONS="-G Xcode"
  BUILD="${SOURCE}/build-xcode"
  RUN="${BUILD}/Debug/test-metal ${@:2}"

elif [[ ${1} == "debug" ]]; then

  OPTIONS="-G Ninja -DCMAKE_BUILD_TYPE=Debug"
  BUILD="${SOURCE}/build-debug"
  RUN="${BUILD}/test-metal ${@:2}"

elif [[ ${1} == "release" ]]; then

  OPTIONS="-G Ninja -DCMAKE_BUILD_TYPE=Release"
  BUILD="${SOURCE}/build-release"
  RUN="${BUILD}/test-metal ${@:2}"

else

  OPTIONS="-G Ninja -DCMAKE_BUILD_TYPE=Release"
  BUILD="${SOURCE}/build-release"
  RUN="${BUILD}/test-metal ${@}"

fi

cmake -S ${SOURCE} -B ${BUILD} ${OPTIONS} || exit $?
cmake --build ${BUILD} || exit $?

${RUN}
