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


class TAC {
public:
    std::string dest;   // Destination operand (if applicable)
    std::string src1;   // First source operand
    std::string src2;   // Second source operand (for binary ops)

    std::string op;

    TAC(std::string o, const std::string& d, const std::string& s1, const std::string& s2)
        : op(o), dest(d), src1(s1), src2(s2) {}

        void printAll() const {
            if (op == "ASSIGN") {
                    printf("%s := %s\n", dest.c_str(), src1.c_str());
                }

                else if (op =="ADD"){
                    printf("%s := %s + %s\n", dest.c_str(), src1.c_str(), src2.c_str()); // FIX THIS IN FINAL ANSWER
                }
                else if (op =="LESS_THAN"){
                    printf("%s := %s < %s\n", dest.c_str(), src1.c_str(), 
                     src2.c_str()); // FIX THIS IN FINAL ANSWER
                }
                else if (op =="MORE_THAN"){
                    printf("%s := %s > %s\n", dest.c_str(), src1.c_str(), 
                     src2.c_str()); // FIX THIS IN FINAL ANSWER
                }
                else if (op =="MULT"){
                    printf("%s := %s * %s\n", dest.c_str(), src1.c_str(), 
                     src2.c_str()); // FIX THIS IN FINAL ANSWER
                }
                else if (op =="SUB"){
                    printf("%s := %s - %s\n", dest.c_str(), src1.c_str(), 
                     src2.c_str()); // FIX THIS IN FINAL ANSWER
                }
                else if (op =="AND"){
                    printf("%s := %s - %s\n", dest.c_str(), src1.c_str(), 
                     src2.c_str()); // FIX THIS IN FINAL ANSWER
                }

                else if (op =="COND_JUMP"){
                    printf("if %s goto %s else goto %s\n", src1.c_str(), dest.c_str(), src2.c_str());
                    }
                else if( op =="JUMP"){
                    printf("goto %s\n", dest.c_str());

                }
                else if(op == "CALL"){
                    printf("%s := CALL %s: %s\n", dest.c_str(), src1.c_str(), src2.c_str() );

                }                
                else if(op == "Args"){
                    printf(" @param: %s \n", src1.c_str());

                }
                else if(op == "RETURN"){
                    printf("RETURN %s\n", src1.c_str());

                }
                else if(op =="PRINT" ){
                    
                    printf("PRINT %s\n", src1.c_str());
                }
                else if( op == "NEW"){

                    printf("%s := NEW %s\n", dest.c_str(), src1.c_str());
                }
                else if (op == "CLASS"){
                    
                    printf("CLASS %s\n", dest.c_str());
                }
    
                else if (op == "METHOD"){
                    printf("METHOD %s IN %s\n", dest.c_str(), src1.c_str());
                }
                else if(op == "NOT"){

                    printf("%s := !%s\n",  dest.c_str(), src1.c_str());
                }
                else if( op == "EXIT"){

                    printf("EXIT");
                }
                else {

                    printf("Unknown TAC type == ", op.c_str() ," \n");
                }
        }
        
  
};

class BasicBlock {
public:
    string label;  // Unique identifier (e.g., "block_0")
    vector<TAC> tacInstructions;
    std::vector<BasicBlock*> successors; // Replaces next_true/false
    std::string condition; // Optional: for branch conditions

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


    // DONT USE THIS BIG MISTAKES CAN HAPPEN WITH SUCCESORS (ARROWS)
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
                if (tac.op == "ASSIGN" ) {
                        label += tac.dest + " := " + tac.src1 + "\\n";
                    }
      
                else if (tac.op == "ADD" ) {
                    label += tac.dest + " := " + tac.src1 + " + " + tac.src2 + "\\n";
                }
                else if (tac.op == "SUB" ) {
                    label += tac.dest + " := " + tac.src1 + " - " + tac.src2 + "\\n";
                }
                else if (tac.op == "MULT" ) {
                    label += tac.dest + " := " + tac.src1 + " * " + tac.src2 + "\\n";
                }
                else if (tac.op == "LESS_THAN" ) {
                    label += tac.dest + " := " + tac.src1 + " < " + tac.src2 + "\\n";
                }
                else if (tac.op == "MORE_THAN" ) {
                    label += tac.dest + " := " + tac.src1 + " > " + tac.src2 + "\\n";
                }
                else if (tac.op == "AND" ) {
                    label += tac.dest + " := " + tac.src1 + " && " + tac.src2 + "\\n";
                }
                else if (tac.op == "COND_JUMP" ) {

                    label += "if " + tac.dest + " goto " + tac.src1 + " else goto " + tac.src2 + "\\n";
                    }
                else if (tac.op == "JUMP" ) {
                        label += "goto " + tac.dest + "\\n";
                    }
                else if (tac.op == "CALL" ) {
                        label += tac.dest + " := CALL " + tac.src1+ ": " +tac.src2+  "\\n";
                    }
                else if (tac.op == "Args" ) {
                    label +=  " @Param: " + tac.src1 + "\\n";
                }
                else if (tac.op == "RETURN" ) {
                        label += "RETURN " + tac.src1 + "\\n";
                    }
                else if (tac.op == "PRINT" ) {
                        label += "PRINT " + tac.src1 + "\\n";
                }
                else if (tac.op == "NEW" ) {

                        label += tac.dest + " := NEW " + tac.src1 + "\\n";
                }
                else if (tac.op == "CLASS" ) {
                
                    label += "CLASS " + tac.dest + "\\n";
                }
                else if (tac.op == "METHOD" ) {

                    label += "METHOD " + tac.dest + " IN " + tac.src1 + "\\n";
                }
                else if (tac.op == "LABEL" ) {

                    label += "LABEL " + tac.dest + "\\n";
                }
                else if (tac.op == "EXIT" ) {
                    label += "EXIT\\n";
                }
                else if (tac.op == "NOT" ) {
                    label += tac.dest +" := !" + tac.src1 + "\\n";
                }                

            }
            // Add edges to successors
            for (BasicBlock* succ : block->successors) {
                string edge_label = getEdgeLabel(block, succ);
                outStream << block->label << " -> " << succ->label;
                if (!edge_label.empty()) {
                    outStream << " [xlabel=\"" << edge_label << "\"]";
                }
                outStream << ";\n";
            }
    
            // Write the node with formatted label
            outStream << block->label << " [label=\"" << label << "\"];" << std::endl;
    
        }
    
        outStream << "}" << std::endl;
        outStream.close();
    }

    void printAllInstructions() {
        for (const auto& block : blocks) {
            block->printInstructions();
        }
    }

    void serializeToFile(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
            return;
        }

        for (const auto& block : blocks) {
            outFile << block->label << ":\n"; // Write the block label
            for (const auto& tac : block->tacInstructions) {
                outFile << "    "; // Indent instructions for readability
                if (tac.op == "ASSIGN") {
                    outFile << tac.dest << " := " << tac.src1 << "\n";
                } else if (tac.op == "ADD") {
                    outFile << tac.dest << " := " << tac.src1 << " + " << tac.src2 << "\n";
                } else if (tac.op == "SUB") {
                    outFile << tac.dest << " := " << tac.src1 << " - " << tac.src2 << "\n";
                } else if (tac.op == "MULT") {
                    outFile << tac.dest << " := " << tac.src1 << " * " << tac.src2 << "\n";
                } else if (tac.op == "LESS_THAN") {
                    outFile << tac.dest << " := " << tac.src1 << " < " << tac.src2 << "\n";
                } else if (tac.op == "COND_JUMP") {
                    outFile << "if " << tac.dest << " goto " << tac.src1 << " else goto " << tac.src2 << "\n";
                } else if (tac.op == "JUMP") {
                    outFile << "goto " << tac.dest << "\n";
                } else if (tac.op == "CALL") {
                    outFile << tac.dest << " := CALL " << tac.src1 << ": " << tac.src2 << "\n";
                } else if (tac.op == "RETURN") {
                    outFile << "RETURN " << tac.src1 << "\n";
                } else if (tac.op == "PRINT") {
                    outFile << "PRINT " << tac.src1 << "\n";
                } else if (tac.op == "NEW") {
                    outFile << tac.dest << " := NEW " << tac.src1 << "\n";
                } else if (tac.op == "EXIT") {
                    outFile << "EXIT\n";
                } 
                else if (tac.op == "CLASS") {
                    outFile << "CLASS " << tac.dest << "\n";
                } 
                else if (tac.op == "METHOD") {
                    outFile << "METHOD " << tac.dest << " IN " << tac.src1 << "\n";
                } 
                else if (tac.op == "LABEL") {
                    outFile << "LABEL " << tac.dest << "\n";
                } 
                else if (tac.op == "NOT") {
                    outFile << tac.dest << " := !" << tac.src1 << "\n";
                }
                else if (tac.op == "AND") {
                    outFile << tac.dest << " := " << tac.src1 << " && " << tac.src2 << "\n";
                }
                else if (tac.op == "Args") {
                    outFile << " @param: " << tac.src1 << "\n";
                }
                else if (tac.op == "MORE_THAN") {
                    outFile << tac.dest << " := " << tac.src1 << " > " << tac.src2 << "\n";
                }
                else {
                    outFile << "Unknown TAC: " << tac.op << "\n";
                }
            }
            outFile << "\n"; // Separate blocks with a blank line
        }

        outFile.close();
        std::cout << "IR serialized to file: " << filename << std::endl;
    }

private:
    string getEdgeLabel(const BasicBlock* from, const BasicBlock* to) const {
        if (from->tacInstructions.empty()) return "";

        const TAC& last_tac = from->tacInstructions.back();
        if (last_tac.op == "COND_JUMP") {
            // First successor is true branch, second is false branch
            if (!from->successors.empty() && from->successors[0] == to) {
                return "true";
            }
            if (from->successors.size() > 1 && from->successors[1] == to) {
                return "false";
            }
        }
        return ""; // No label for unconditional jumps
    }
};

#endif