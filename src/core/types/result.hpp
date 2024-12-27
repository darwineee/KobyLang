#pragma once

#include "error.hpp"
#include "stmt.hpp"
#include <tuple>
#include <vector>

using ScanResult  = std::tuple<std::vector<Token>, std::vector<Error>>;
using ParseResult = std::tuple<std::vector<Stmt>, std::vector<Error>>;