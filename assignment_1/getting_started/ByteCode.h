#ifndef BYTECODE_H
#define BYTECODE_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class ByteCodeInstruction {
public:
    std::string opcode; // The operation code (e.g., "iload", "iadd", "print")
    std::string operand1; // First operand (optional)
    std::string operand2; // Second operand (optional)

    ByteCodeInstruction(const std::string& op, const std::string& op1 = "", const std::string& op2 = "")
        : opcode(op), operand1(op1), operand2(op2) {}

    // Serialize the instruction to a string
    std::string toString() const {
        if (operand1.empty() && operand2.empty()) {
            return opcode;
        } else if (operand2.empty()) {
            return opcode + " " + operand1;
        } else {
            return opcode + " " + operand1 + " " + operand2;
        }
    }
};

class ByteCode {
public:
    std::vector<ByteCodeInstruction> instructions;

    // Add a new instruction
    void addInstruction(const std::string& opcode, const std::string& operand1 = "", const std::string& operand2 = "") {
        instructions.emplace_back(opcode, operand1, operand2);
    }

    // Serialize all instructions to a file
    void serializeToFile(const std::string& filename) const {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
            return;
        }

        for (const auto& instr : instructions) {
            outFile << instr.toString() << std::endl;
        }

        outFile.close();
        std::cout << "Byte-code serialized to file: " << filename << std::endl;
    }

    // Print all instructions to the console
    void print() const {
        for (const auto& instr : instructions) {
            std::cout << instr.toString() << std::endl;
        }
    }
};

#endif