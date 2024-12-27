#include "const/cmd.hpp"
#include "const/prelude_func.hpp"
#include "interpreter/interpreter.hpp"
#include "interpreter/parser.hpp"
#include "interpreter/scanner.hpp"
#include "print/printer.hpp"
#include "utils/file.hpp"

#include <fstream>
#include <iostream>
#include <string>

int procCmdHelp();
int procCmdRun(const std::string& path);
int procCmdRepl();

int main(const int argc, char* argv[]) {
    if(argc < 2) {
        procCmdHelp();
        return EXIT_FAILURE;
    }

    if(argv[1] == cmd::HELP) {
        return procCmdHelp();
    }

    if(argv[1] == cmd::RUN) {
        if(argc < 3) {
            std::cerr << "Usage: koby run <filename>" << std::endl;
            return EXIT_FAILURE;
        }
        return procCmdRun(argv[2]);
    }

    if(argv[1] == cmd::REPL) {
        return procCmdRepl();
    }

    std::cerr << "Unknown command: " << argv[1] << std::endl;
    return EXIT_FAILURE;
}

int procCmdHelp() {
    std::cout << "Usage: koby <command> [file path]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  help - Display this help message." << std::endl;
    std::cout << "  run  - Run the code from file path." << std::endl;
    std::cout << "  repl - Start the REPL." << std::endl;
    std::cout << "       - Type 'exit' to exit the REPL." << std::endl;
    return EXIT_SUCCESS;
}

int procCmdRun(const std::string& path) {
    auto       scanner  = Scanner::from_source(utils::read_file_contents(path));
    const auto scan_res = scanner.scan_tokens();
    if(!scanner.success()) {
        printer::print_res_err(scan_res);
        return EXIT_FAILURE;
    }
    auto       parser    = Parser::from_tokens(std::get<0>(scan_res));
    const auto parse_res = parser.parse();
    if(!parser.success()) {
        printer::print_res_err(parse_res);
        return EXIT_FAILURE;
    }
    try {
        Interpreter().interpret(std::get<0>(parse_res));
    } catch(Error& error) {
        printer::print_err(error);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int procCmdRepl() {
    std::cout << "Koby REPL" << std::endl;
    std::string input;
    auto        interp = Interpreter();
    interp.exclude_native_func({prelude::PUT, prelude::GET});
    while(true) {
        std::cout << "\033[1;32m>>> \033[0m";
        std::getline(std::cin, input);

        if(input == cmd::EXIT) {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        // Convenient syntax for repl, auto add a last semicolon
        if(input.back() != symbol::Semicolon)
            input += symbol::Semicolon;

        auto       scanner  = Scanner::from_source(input);
        const auto scan_res = scanner.scan_tokens();
        if(!scanner.success()) {
            printer::print_res_err(scan_res);
            continue;
        }
        auto       parser    = Parser::from_tokens(std::get<0>(scan_res));
        const auto parse_res = parser.parse();
        if(!parser.success()) {
            printer::print_res_err(parse_res);
            continue;
        }
        try {
            auto [control, value] = interp.interpret(std::get<0>(parse_res));
            printer::print(value);
        } catch(Error& error) {
            printer::print_err(error);
        }
    }
    return EXIT_SUCCESS;
}
