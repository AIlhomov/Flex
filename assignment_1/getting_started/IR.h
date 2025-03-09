// IR.h
#ifndef IR_H
#define IR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

using namespace std;
// Forward declarations
class BasicBlock;
class CFG;

struct BlockContext {
    BasicBlock* current_block; // Current block for TAC insertion
    CFG* cfg; // Entire control flow graph
};

//enum class OpCode { ADD, SUB, MOV, LOAD, STORE };

enum class TACType {
    DECLARE,    // e.g., DECLARE x: int
    ASSIGN,     // e.g., x := y
    BIN_OP,     // e.g., t1 = a + b
    COND_JUMP,  // e.g., if x < y goto L1
    JUMP,       // e.g., goto L2
    LABEL,      // e.g., L1:
    CALL,       // e.g., t1 = CALL foo, a, b
    RETURN,     // e.g., RETURN t1
    PRINT,      // e.g., PRINT t1
    NEW         // e.g., t1 = NEW Foo
};

class TAC {
public:
    TACType type;
    std::string dest;   // Destination operand (if applicable)
    std::string src1;   // First source operand
    std::string src2;   // Second source operand (for binary ops)
    std::string label;  // Label for jumps (e.g., "L1")
    std::string object; // Method calls
    std::string var_type;  // NEW: For declaration type

    TAC(TACType t, const std::string& d, const std::string& s1, 
        const std::string& s2 = "", const std::string& l = "", 
        const std::string& vt = "")
        : type(t), dest(d), src1(s1), src2(s2), label(l), var_type(vt) {}

    void printAll() const {
        switch (type) {
            case TACType::DECLARE:  // NEW CASE
                printf("DECLARE %s: %s\n", dest.c_str(), var_type.c_str());
                break;
            case TACType::ASSIGN:
                printf("%s := %s\n", dest.c_str(), src1.c_str());
                break;
            case TACType::BIN_OP:
                printf("%s := %s %s %s\n", dest.c_str(), src1.c_str(), label.c_str(), src2.c_str());
                break;
            case TACType::COND_JUMP:
                printf("if %s goto %s else goto %s\n", src1.c_str(), label.c_str(), src2.c_str());
                break;
            case TACType::JUMP:
                printf("goto %s\n", label.c_str());
                break;
            case TACType::CALL:
                printf("%s := CALL %s(%s)\n", dest.c_str(), src1.c_str(), src2.c_str());
                break;
            case TACType::RETURN:
                printf("RETURN %s\n", src1.c_str()); // Use src1, not dest
                break;
            case TACType::PRINT:
                printf("PRINT %s\n", src1.c_str());
                break;
            case TACType::NEW:
                printf("%s := NEW %s\n", dest.c_str(), src1.c_str());
                break;
            default:
                printf("Unknown TAC type\n");
        }
    }

    void addToTac() {
        // Implementation for adding to TAC (if needed)
    }
};

class BasicBlock {
public:
    string label;  // Unique identifier (e.g., "block_0")
    vector<TAC> tacInstructions;
    BasicBlock* next_true;   // Successor for true condition (if applicable)
    BasicBlock* next_false;  // Successor for false condition (if applicable)
    // For simplicity, track predecessors if needed


    // TAC condition;
    // Block *trueExit, *falseExit;

    //Block() : trueExit(NULL), falseExit(NULL) {}
    
    void printInstructions() {
        for (const auto& tac : tacInstructions) {
            tac.printAll();
        }
    }
};

class CFG {
public:
    BasicBlock* entry_block;   // Entry block of the CFG
    std::vector<BasicBlock*> blocks;  // All basic blocks

    void addBlock(BasicBlock* block){
        blocks.push_back(block);
    }
    void generateDot(const std::string& filename) {
        std::ofstream outStream(filename);
        outStream << "digraph CFG {" << std::endl;
        outStream << "graph [splines=ortho];" << std::endl; // Add ortho splines
        outStream << "node [shape=box];" << std::endl;       // Set node shape to box
    
        for (const auto& block : blocks) {
            // Build the label with TAC instructions
            std::string label = block->label + "\\n";
            for (const TAC& tac : block->tacInstructions) {
                switch (tac.type) {
                    case TACType::DECLARE:
                        label += "DECLARE " + tac.dest + ": " + tac.var_type + "\\n";
                        break;
                    case TACType::ASSIGN:
                        label += tac.dest + " := " + tac.src1 + "\\n";
                        break;
                    case TACType::BIN_OP:
                        label += tac.dest + " := " + tac.src1 + " " + tac.label + " " + tac.src2 + "\\n";
                        break;
                    case TACType::COND_JUMP:
                        label += "if " + tac.src1 + " goto " + tac.label + " else goto " + tac.src2 + "\\n";
                        break;
                    case TACType::JUMP:
                        label += "goto " + tac.label + "\\n";
                        break;
                    case TACType::CALL:
                        label += tac.dest + " := CALL " + tac.src1 + "(" + tac.src2 + ")\\n";
                        break;
                    case TACType::RETURN:
                        label += "RETURN " + tac.src1 + "\\n";
                        break;
                    case TACType::PRINT:
                        label += "PRINT " + tac.src1 + "\\n";
                        break;
                    case TACType::NEW:
                        label += tac.dest + " := NEW " + tac.src1 + "\\n";
                        break;
                    
                    default:
                        break;
                }
            }
    
            // Write the node with formatted label
            outStream << block->label << " [label=\"" << label << "\"];" << std::endl;
    
            // Add edges with xlabel
            if (block->next_true) {
                outStream << block->label << " -> " << block->next_true->label 
                          << " [xlabel=\"true\"];" << std::endl;
            }
            if (block->next_false) {
                outStream << block->label << " -> " << block->next_false->label 
                          << " [xlabel=\"false\"];" << std::endl;
            }
        }
    
        outStream << "}" << std::endl;
        outStream.close();
    }

    void printAllInstructions() {
        for (const auto& block : blocks) {
            block->printInstructions();
        }
    }
};

#endif