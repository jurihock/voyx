#!/bin/bash

# ###########################################################################
#
# The qt@5 is keg-only, which means it was not symlinked into /opt/homebrew,
# because this is an alternate version of another formula.
#
# If you need to have qt@5 first in your PATH, run:
#   echo 'export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"' >> ~/.zshrc
#
# For compilers to find qt@5 you may need to set:
#   export LDFLAGS="-L/opt/homebrew/opt/qt@5/lib"
#   export CPPFLAGS="-I/opt/homebrew/opt/qt@5/include"
#
PATH="/opt/homebrew/opt/qt@5/bin:$PATH"
#
# ###########################################################################

SOURCE=$(dirname "$0")

if [[ ${1} == "xcode" ]]; then

  OPTIONS="-G Xcode"
  BUILD="${SOURCE}/build-xcode"
  RUN="${BUILD}/Debug/voyx ${@:2}"

elif [[ ${1} == "debug" ]]; then

  OPTIONS="-G Ninja -DCMAKE_BUILD_TYPE=Debug"
  BUILD="${SOURCE}/build-debug"
  RUN="${BUILD}/voyx ${@:2}"

elif [[ ${1} == "release" ]]; then

  OPTIONS="-G Ninja -DCMAKE_BUILD_TYPE=Release"
  BUILD="${SOURCE}/build-release"
  RUN="${BUILD}/voyx ${@:2}"

else

  OPTIONS="-G Ninja -DCMAKE_BUILD_TYPE=Release"
  BUILD="${SOURCE}/build-release"
  RUN="${BUILD}/voyx ${@}"

fi

cmake -S ${SOURCE} -B ${BUILD} ${OPTIONS} || exit $?
cmake --build ${BUILD} || exit $?

${RUN}
