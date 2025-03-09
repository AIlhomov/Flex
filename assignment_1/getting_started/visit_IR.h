#include "Node.h"
#include "IR.h"


/*

Expressions: Return a std::string (temporary variable/value).

    Examples: AddExpression, SubExpression, MethodCall, NEW, THIS, INT, TRUE, FALSE.

Statements: Return BasicBlock* (exit block for control flow).

    Examples: IfStatement, WhileLoop, Assignment, PrintStatement, ReturnStatement.

Declarations: Process children but return "" (no value).

    Examples: methodDec, classDeclaration, varDeclaration.

Generic Nodes: Process all children without specific handling.

    Examples: goal, statements, methodBody

*/

class IRvisitor{
private:
    // counters
    int temp_counter = 0;
    int block_counter = 0;
    
    std::unordered_map<std::string, CFG*> method_cfgs; // Track all method CFGs

    string new_temp() {
        return "__t" + std::to_string(temp_counter++);
    }
    BasicBlock* create_block(CFG* cfg) {
        BasicBlock* block = new BasicBlock();
        block->label = "block_" + std::to_string(block_counter++); // Unique label
        cfg->addBlock(block); // Add to CFG
        return block;
    }
public:

    void generateAllDotFiles() {
        for (auto& [method_name, cfg] : method_cfgs) {
            cfg->generateDot(method_name + "_cfg.dot");
        }
    }

    CFG* generate_IR(Node* root) {
        CFG* cfg = new CFG(); // Create a CFG on the heap
        BasicBlock* entry_block = create_block(cfg); // Pass CFG to create_block
        cfg->entry_block = entry_block;

        BlockContext ctx{entry_block, cfg};
        traverse_generic(root, ctx);

        return cfg; // Return the fully built CFG
    }

private:

    std::string visit_expr(Node* node, BlockContext& ctx) {
        if (!node) return "";

        if (node->type == "AddExpression") {
            std::string left = visit_expr(node->children.front(), ctx);
            std::string right = visit_expr(*std::next(node->children.begin()), ctx);
            std::string temp = new_temp();
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::BIN_OP, temp, left, right, "+"}
            );
            return temp;
        }
        
        else if (node->type == "NEW identifier LP RP") {
            Node* class_node = node->children.front(); // Get the class name
            std::string temp = new_temp();
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::NEW, temp, class_node->value, ""}
            );
            return temp;
        }
        else if (node->type == "SubExpression") {
            std::string left = visit_expr(node->children.front(), ctx);
            std::string right = visit_expr(*std::next(node->children.begin()), ctx);
            std::string temp = new_temp();
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::BIN_OP, temp, left, right, "-"}
            );
            return temp;
        }
        else if (node->type == "exp DOT ident LP exp COMMA exp RP") {
            Node* obj_node = node->children.front();    // Object (e.g., "this" or "new Bar")
            Node* method_node = *std::next(node->children.begin()); // Method name
            Node* args_node = *std::next(node->children.begin(), 2); // Arguments

            // Process object and arguments
            std::string obj_temp = visit_expr(obj_node, ctx);
            std::vector<std::string> arg_temps;
            for (auto arg_child : args_node->children) {
                arg_temps.push_back(visit_expr(arg_child, ctx));
            }
        
            // Build args string (e.g., "t1, t2")
            std::string args_str;
            for (size_t i = 0; i < arg_temps.size(); i++) {
                args_str += arg_temps[i];
                if (i != arg_temps.size() - 1) args_str += ", ";
            }
        
            // Generate CALL instruction
            std::string result_temp = new_temp();
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::CALL, result_temp, method_node->value, args_str, obj_temp}
            );
            return result_temp;
        }
        else if (node->type == "LESS_THAN") {
            std::string left = visit_expr(node->children.front(), ctx);
            std::string right = visit_expr(*std::next(node->children.begin()), ctx);
            std::string temp = new_temp();
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::BIN_OP, temp, left, right, "<"}
            );
            return temp;
        }
        else if (node->type == "INT") {
            std::string temp = new_temp();
            ctx.current_block->tacInstructions.push_back(TAC{TACType::ASSIGN, temp, node->value, ""});
            return temp;
        }
        else if (node->type == "TRUE" || node->type == "FALSE") {
            std::string temp = new_temp();
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::ASSIGN, temp, node->type == "TRUE" ? "1" : "0", ""}
            );
            return temp;
        }
        return "";
    }


    BasicBlock* visit_stmt(Node* node, BlockContext& ctx) {
        if (!node) return ctx.current_block;
    
        if (node->type == "IfStatement") {
            BasicBlock* cond_block = create_block(ctx.cfg);
            BasicBlock* true_block = create_block(ctx.cfg);
            BasicBlock* false_block = create_block(ctx.cfg);
            BasicBlock* merge_block = create_block(ctx.cfg);
    
            // Link current block to cond_block
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::JUMP, "", "", "", cond_block->label}
            );
            ctx.cfg->addBlock(cond_block);
    
            // Evaluate condition
            ctx.current_block = cond_block;
            std::string cond_temp = visit_expr(node->children.front(), ctx);
            cond_block->tacInstructions.push_back(
                TAC{TACType::COND_JUMP, cond_temp, "", true_block->label, false_block->label}
            );
    
            // Process true branch
            ctx.current_block = true_block;
            BasicBlock* true_exit = visit_stmt(*std::next(node->children.begin()), ctx);
            true_exit->tacInstructions.push_back(TAC{TACType::JUMP, "", "", "", merge_block->label});
    
            // Process false branch
            ctx.current_block = false_block;
            BasicBlock* false_exit = visit_stmt(*std::next(node->children.begin(), 2), ctx);
            false_exit->tacInstructions.push_back(TAC{TACType::JUMP, "", "", "", merge_block->label});
    
            // Update current block to merge_block
            ctx.current_block = merge_block;
            return merge_block;
        }
        else if (node->type == "SOMETHING ASSIGNED = TO SOMETHING") {
            Node* lhs = node->children.front(); // Left-hand side (variable)
            Node* rhs = *std::next(node->children.begin()); // Right-hand side (expression)
            //cout << "ASDIOJASDOIJNASDIONADSIO "<<lhs->value<< endl;
            std::string rhs_temp = visit_expr(rhs, ctx);
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::ASSIGN, lhs->value, rhs_temp, ""}
            );
            return ctx.current_block;
        }
        else if (node->type == "RETURN") {
            Node* ret_val = node->children.front();
            std::string ret_temp = visit_expr(ret_val, ctx);
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::RETURN, "", ret_temp, ""}
            );
            return ctx.current_block;
        }
        else if (node->type == "SIMPLE PRINT LOL") {
            Node* value_node = node->children.front();
            std::string print_temp = visit_expr(value_node, ctx);
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::PRINT, "", print_temp, ""}
            );
            return ctx.current_block;
        }
        else if (node->type == "var declaration") {
            Node* typeNode = node->children.front();
            Node* idNode = *std::next(node->children.begin());
            
            // Add declaration to CURRENT block
            ctx.current_block->tacInstructions.push_back(
                TAC{TACType::DECLARE, idNode->value, "", "", "", typeNode->value}
            );
            
            // Handle initialization if exists
            if (node->children.size() > 2) {
                Node* initNode = *std::next(node->children.begin(), 2);
                std::string init_temp = visit_expr(initNode, ctx);
                ctx.current_block->tacInstructions.push_back(
                    TAC{TACType::ASSIGN, idNode->value, init_temp, ""}
                );
            }
            return ctx.current_block;
        }
        else if (node->type == "MAIN METHOD") {
            for (auto child : node->children) {
                ctx.current_block = visit_stmt(child, ctx); // Process ALL child statements
            }
        }
        // ... handle other statements (WhileLoop, Assignment, etc.)
        return ctx.current_block; // Default return
    }
    
    void traverse_generic(Node* node, BlockContext& ctx) {
        if (!node) return;
        
        //std::cerr << "Visiting node: " << node->type << std::endl;

        // if (node->type == "classDeclaration") {
        //     CFG* class_cfg = new CFG();
        //     BasicBlock* class_entry = create_block(class_cfg);
        //     class_cfg->entry_block = class_entry;

        //     BlockContext class_ctx{class_entry, class_cfg}; // Correct initialization
        //     for (auto child : node->children) {
        //         if (child->type == "methodDeclarations" || child->type == "methodDec") {
        //             traverse_generic(child, class_ctx);
        //         } else {
        //             traverse_generic(child, ctx);
        //         }
        //     }

        // } 
        
        if (node->type == "methodDec") {
            CFG* method_cfg = new CFG();
            Node* method_name_node = *std::next(node->children.begin()); // Adjust index based on your AST structure
            method_cfgs[method_name_node->value] = method_cfg;
            
            BasicBlock* method_entry = create_block(method_cfg);
            method_cfg->entry_block = method_entry;
            
            BlockContext method_ctx{method_entry, method_cfg};
            for (auto child : node->children) {
                if (child->type == "methodBody") {
                    traverse_generic(child, method_ctx);
                } else {
                    traverse_generic(child, ctx);
                }
            }
        }
        else if (node->type == "statement") {
            for (auto child : node->children) {
                if (child->type == "SIMPLE PRINT LOL") {
                    // Handle top-level statements in the main method
                    ctx.current_block = visit_stmt(child, ctx);
                } 
                else if (child->type == "SOMETHING ASSIGNED = TO SOMETHING") {
                    // Handle top-level statements in the main method
                    ctx.current_block = visit_stmt(child, ctx);

                    // //add it
                    // //ctx.current_block
                    // CFG* method_cfg = new CFG();
                    // BasicBlock* method_entry = visit_stmt(child, ctx);
                    // method_cfg->entry_block = method_entry;
                    // BlockContext method_ctx{method_entry, method_cfg}; // Correct initialization
                    
                    

                } 
                else if (child->type == "RETURN") {
                    // Handle top-level statements in the main method
                    ctx.current_block = visit_stmt(child, ctx);
                    
                }
                else {
                    traverse_generic(child, ctx);
                }
            }
        }
        else if (node->type == "MAIN METHOD") {
            for (auto child : node->children) {
                if (child->type == "SIMPLE PRINT LOL") {
                    // Handle top-level statements in the main method
                    ctx.current_block = visit_stmt(child, ctx);
                } else {
                    traverse_generic(child, ctx);
                }
            }
        }
        else if (node->type == "var declaration") {
            visit_stmt(node, ctx);
        }
        else {
            //std::cerr << "Generic traversal for node type: " << node->type << std::endl;
            for (auto child : node->children) {
                traverse_generic(child, ctx);
            }
        }
    }

};


