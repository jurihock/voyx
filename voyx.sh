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

mkdir -p build-release

pushd build-release >/dev/null 2>&1
cmake -DCMAKE_BUILD_TYPE=Release .. || exit $?
cmake --build . || exit $?
popd >/dev/null 2>&1

build-release/voyx $@
