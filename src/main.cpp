#include "const/cmd.hpp"
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
        return 1;
    }

    if(argv[1] == cmd::HELP) {
        return procCmdHelp();
    }

    if(argv[1] == cmd::RUN) {
        if(argc < 3) {
            std::cerr << "Usage: koby run <filename>" << std::endl;
            return 1;
        }
        return procCmdRun(argv[2]);
    }

    if(argv[1] == cmd::REPL) {
        return procCmdRepl();
    }

    std::cerr << "Unknown command: " << argv[1] << std::endl;
    return 1;
}

int procCmdHelp() {
    std::cout << "Usage: koby <command> [file path]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  help - Display this help message." << std::endl;
    std::cout << "  run  - Run the code from file path." << std::endl;
    std::cout << "  repl - Start the REPL." << std::endl;
    return 0;
}

int procCmdRun(const std::string& path) {
    auto       scanner = Scanner::from_source(utils::read_file_contents(path));
    const auto scan_res  = scanner.scan_tokens();
    if(!scanner.success()) {
        printer::print_res_err(scan_res);
        return 1;
    }
    auto       parser       = Parser::from_tokens(std::get<0>(scan_res));
    const auto parse_res = parser.parse();
    if(!parser.success()) {
        printer::print_res_err(parse_res);
        return 1;
    }
    try {
        auto interp = Interpreter();
        interp.interpret(std::get<0>(parse_res));
    } catch(Error& error) {
        printer::print_err(error);
    }
    return 0;
}

int procCmdRepl() {
    return 0;
}
