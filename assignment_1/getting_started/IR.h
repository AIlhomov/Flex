#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

//enum class OpCode { ADD, SUB, MOV, LOAD, STORE };

enum class TACType {
    ASSIGN,     // e.g., x := y
    BIN_OP,     // e.g., t1 = a + b
    COND_JUMP,  // e.g., if x < y goto L1
    JUMP,       // e.g., goto L2
    LABEL,      // e.g., L1:
    // Add other operations (e.g., function calls, returns)
};

class TAC {
public:
    TACType type;
    std::string dest;   // Destination operand (if applicable)
    std::string src1;   // First source operand
    std::string src2;   // Second source operand (for binary ops)
    std::string label;  // Label for jumps (e.g., "L1")

    TAC(TACType t, const std::string& d, const std::string& s1, const std::string& s2, const std::string& l = "")
        : type(t), dest(d), src1(s1), src2(s2), label(l) {}

    void printAll() const {
        printf("%s := %s %s %s\n", dest.c_str(), src1.c_str(), label.c_str(), src2.c_str());
    }

    void addToTac() {
        // Implementation for adding to TAC (if needed)
    }
};

class BasicBlock {
public:
    string label;  // Unique identifier (e.g., "block_0")
    vector<TAC> tacInstruction;
    BasicBlock* next_true;   // Successor for true condition (if applicable)
    BasicBlock* next_false;  // Successor for false condition (if applicable)
    // For simplicity, track predecessors if needed


    // TAC condition;
    // Block *trueExit, *falseExit;

    //Block() : trueExit(NULL), falseExit(NULL) {}
    
    void printInstructions() {
        for (const auto& tac : tacInstruction) {
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
        for (const auto& block : blocks) {
            outStream << block->label << " [label=\"" << block->label << "\"];" << std::endl;
            if (block->next_true) {
                outStream << block->label << " -> " << block->next_true->label << " [label=\"true\"];" << std::endl;
            }
            if (block->next_false) {
                outStream << block->label << " -> " << block->next_false->label << " [label=\"false\"];" << std::endl;
            }
        }
        outStream << "}" << std::endl;
        outStream.close();
    }
};