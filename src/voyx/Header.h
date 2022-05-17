#pragma once

/**
 * This file provides a set of common std modules
 * and is to be included in .h project files.
 **/

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <complex>
#include <condition_variable>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <regex>
#include <span>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

/**
 * Following by custom types.
 **/

#include <voyx/ETC/Assert.h>
#include <voyx/ETC/Vector.h>
#include <voyx/ETC/Matrix.h>

/**
 * And finally common data type definition,
 * e.g. float or double.
 **/

typedef float voyx_t;
