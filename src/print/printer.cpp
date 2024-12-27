#include "types/error.hpp"
#include "utils/to_string.hpp"

#include <iostream>
#include <ostream>

namespace printer {

void print(const Value& value) {
    std::cout << utils::to_string(value) << std::endl;
}

void print_waring(const Error& error) {
    std::cout << "Warning: " << error.what() << std::endl;
}

void print_err(const Error& error) {
    std::cerr << error.what() << std::endl;
}

void print_err(const std::vector<Error>& errors) {
    for(const auto& error : errors)
        print_err(error);
}

void print_res_err(const ScanResult& result) {
    print_err(std::get<1>(result));
}

void print_res_err(const ParseResult& result) {
    print_err(std::get<1>(result));
}

} // namespace printer