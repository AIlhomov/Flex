#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class Object {
public:
    std::unordered_map<std::string, int> fields; // Fields of the object
};

class Interpreter {
private:
    std::stack<int> dataStack; // Data stack for intermediate values
    std::unordered_map<std::string, int> variables; // Variable storage
    std::unordered_map<std::string, std::shared_ptr<Object>> objects; // Map of object references
    std::stack<int> callStack; // Stack to store return addresses
    size_t programCounter = 0; // Program counter to track the current instruction
    // Helper function to parse an instruction
    std::vector<std::string> parseInstruction(const std::string& line);
    std::vector<std::vector<std::string>> instructions; // Store parsed instructions
    // Execute a single instruction
    void executeInstruction(const std::vector<std::string>& instruction);

public:
    // Interpret the byte-code file
    void interpret(const std::string& filename);
};

#endif
