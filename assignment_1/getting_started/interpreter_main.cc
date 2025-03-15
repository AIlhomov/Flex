#include "Interpreter.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./interpreter <bytecode_file>" << std::endl;
        return 1;
    }

    Interpreter interpreter;
    interpreter.interpret(argv[1]);

    return 0;
}