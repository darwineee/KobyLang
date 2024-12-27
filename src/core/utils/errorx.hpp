#pragma once

#include "types/error.hpp"

#include <string>

namespace err {

Error make(int err_code, const std::string& message, int line);

}

