#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>

void Interpreter::interpret(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue; // Skip empty lines
        auto instruction = parseInstruction(line);
        executeInstruction(instruction);
    }

    inFile.close();
}

std::vector<std::string> Interpreter::parseInstruction(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void Interpreter::executeInstruction(const std::vector<std::string>& instruction) {
    if (instruction.empty()) return;

    const std::string& opcode = instruction[0];

    if (opcode == "iload") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing operand for iload\n";
            return;
        }
        const std::string& operand = instruction[1];
        if (operand == "True") {
            dataStack.push(1); // Push 1 for True
        } else if (operand == "False") {
            dataStack.push(0); // Push 0 for False
        } else if (isdigit(operand[0]) || (operand[0] == '-' && isdigit(operand[1]))) {
            // Operand is a constant integer
            dataStack.push(std::stoi(operand));
        } else if (variables.find(operand) != variables.end()) {
            // Operand is a variable
            dataStack.push(variables[operand]);
        } else {
            std::cerr << "Error: Variable " << operand << " used before assignment.\n";
        }
    } else if (opcode == "istore") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing variable name for istore\n";
            return;
        }
        if (dataStack.empty()) {
            std::cerr << "Error: Stack underflow in istore\n";
        } else {
            variables[instruction[1]] = dataStack.top();
            dataStack.pop();
        }
    } else if (opcode == "iadd" || opcode == "isub" || opcode == "imul" || opcode == "idiv") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in " << opcode << "\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        if (opcode == "iadd") dataStack.push(a + b);
        else if (opcode == "isub") dataStack.push(a - b);
        else if (opcode == "imul") dataStack.push(a * b);
        else if (opcode == "idiv") {
            if (b == 0) {
                std::cerr << "Error: Division by zero\n";
                return;
            }
            dataStack.push(a / b);
        }
    } else if (opcode == "inot") {
        if (dataStack.empty()) {
            std::cerr << "Error: Stack underflow in inot\n";
            return;
        }
        int value = dataStack.top();
        dataStack.pop();
        dataStack.push(!value);
    } else if (opcode == "iand") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in iand\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a && b);
    } else if (opcode == "ior") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in ior\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a || b);
    } else if (opcode == "ieq") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in ieq\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a == b);
    } else if (opcode == "igt") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in igt\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a > b);
    } else if (opcode == "ilt") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in ilt\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a < b);
    } else if (opcode == "print") {
        if (dataStack.empty()) {
            std::cerr << "Error: Stack underflow in print\n";
        } else {
            std::cout << dataStack.top() << std::endl;
            dataStack.pop();
        }
    } else if (opcode == "equal") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in iequal\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a == b);
    } else if (opcode == "exit") {
        exit(0);
    } else {
        std::cerr << "Unknown opcode: " << opcode << std::endl;
    }
}