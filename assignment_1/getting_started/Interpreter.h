#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>

class Interpreter {
private:
    std::stack<int> dataStack; // Data stack for intermediate values
    std::unordered_map<std::string, int> variables; // Variable storage

    // Helper function to parse an instruction
    std::vector<std::string> parseInstruction(const std::string& line);

    // Execute a single instruction
    void executeInstruction(const std::vector<std::string>& instruction);

public:
    // Interpret the byte-code file
    void interpret(const std::string& filename);
};

#endif
