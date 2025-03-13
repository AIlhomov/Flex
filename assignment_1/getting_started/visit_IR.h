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
    
    string new_temp() {
        return "__t" + std::to_string(temp_counter++);
    }
    BasicBlock* create_block(CFG* cfg, const string& label = "") {
        BasicBlock* block = new BasicBlock();
        if (!label.empty()) block->label = label;
        else block->label = "block_" + std::to_string(block_counter++); // Unique label
        cfg->addBlock(block); // Add to CFG
        return block;
    }
public:

    CFG* generate_IR(Node* root) {
        CFG* cfg = new CFG(); // Create a CFG on the heap
        BasicBlock* entry_block = create_block(cfg); // Pass CFG to create_block
        cfg->entry_block = entry_block;
        cfg->entry_block->label = "Main";
        BlockContext ctx{entry_block, cfg};
        traverse_generic(root, ctx);

        return cfg; // Return the fully built CFG
    }

private:

    std::string visit_expr(Node* node, BlockContext& ctx) {
        if(!node) return "";
        // else if(node->type =="INT" || node->type == "TRUE" || node->type == "FALSE" || node->type == "identifier"){
        //     std::string temp = this->new_temp();
        //     TAC ta(TACType::ASSIGN, temp, node->value, "","");
        //     ctx.current_block->tacInstructions.push_back(ta);
        //     return temp;
        // }
        // else if (node->type == "THIS"){
        //     std::string temp = this->new_temp();
        //     TAC ta(TACType::ASSIGN, temp, node->type, "","");
        //     ctx.current_block->tacInstructions.push_back(ta);
        //     return temp;
        // }
        else if (node->type == "INT"|| node->type == "TRUE" || node->type == "FALSE" || node->type == "identifier"){
            return node->value;
        }
        else if(node->type == "exp DOT ident LP exp COMMA exp RP"){

            std::string firstExpThis = visit_expr(node->children.front(),ctx); //NEW Bar
            std::string temp = this->new_temp();

            if (node->children.front()->type == "THIS"){
                firstExpThis = "THIS";
            }   

            Node* getFuncName = *std::next(node->children.begin()); //aka FOO
            Node* argNode = *std::next(node->children.begin(),2); //aka FOO
            std::string argruments = visit_expr(argNode,ctx);  //can be argument_list or emptyArgumet
            
            TAC ta(TACType::CALL, temp, firstExpThis +"."+ getFuncName->value, argruments,"");  
            ctx.current_block->tacInstructions.push_back(ta);



            return temp;
        }

        else if(node->type =="NEW identifier LP RP"){

            Node* idNode = node->children.front();
            std::string temp = this->new_temp();
            TAC ta(TACType::NEW, temp, idNode->value, "","");
            ctx.current_block->tacInstructions.push_back(ta);
            return temp;

        }


        else if(node->type == "argument_list"){
            std::string args;
            for (auto arg : node->children){
                if (!args.empty()) args += ",";
                args += visit_expr(arg, ctx);
            }
            return args; 
        }

        else if(node->type == "argument"){
            return visit_expr(node->children.front(), ctx);
        }
        else if (node->type == "LESS THAN"){

        }
        else if(node->type == "LC statement RC"){
            return visit_expr(node->children.front(),ctx);
        }
        else if(node->type == "statements"){
            std::string stmts;
            for (auto arg : node->children){
                if (node->children.size()-1 >0) stmts += ",";
                
                stmts += visit_expr(arg, ctx);
            }
            return stmts;        
        }
        else if (node->type == "SubExpression"){
            Node* leftVal = node->children.front();
            Node* rightVal = *std::next(node->children.begin());

            std::string temp = this->new_temp();
            TAC ta(TACType::BIN_OP, temp, leftVal->value, rightVal->value, "", "-");
            ctx.current_block->tacInstructions.push_back(ta);
            return temp;
        }
        

        return "";

    }

    BasicBlock* visit_stmt(Node* node, BlockContext& ctx) {
        //if(!node) return ctx.current_block;
        if(!node) return nullptr;
        else if(node->type == "SIMPLE PRINT LOL"){    
            std::string t = visit_expr(node->children.front(),ctx);
            TAC ta (TACType::PRINT,"",t,"","");
            ctx.current_block->tacInstructions.push_back(ta);  
            // if(node->children.front()->type =="exp DOT ident LP exp COMMA exp RP"){
            //     BasicBlock* newBlock = create_block(ctx.cfg);
            //     ctx.current_block->successors.push_back(newBlock); // Ensure correct flow
            //     ctx.current_block = newBlock; // Switch to the new block
            //     return ctx.current_block;
            // }
            return ctx.current_block;
        }

        // //karmaHere
        // else if(node->type == "LC statement RC"){
        //     return visit_stmt(node->children.front(),ctx);
        // }

        // else if(node->type == "statements"){
        //     BasicBlock* lastBlock = ctx.current_block;
        //     for (Node* stmt : node->children) {
        //         lastBlock = visit_stmt(stmt, ctx); // Process each statement
        //     }
        //     return lastBlock; // Return the last processed block

        // }
        else if (node->type == "LC statement RC"){
            std::cout << "KKKKKKKKKKKKKKKK";
            if (node->children.size() == 1) return visit_stmt(node->children.front(),ctx);
        }
        else if (node->type == "statements"){
            for (auto child : node->children){
                return visit_stmt(child, ctx);
            }
        }
        else if(node->type == "statement"){
            std::cout << "uuuuuuuuuuUUU";
            if (node->children.size() == 1) return visit_stmt(node->children.front(),ctx);
        }

        else if (node->type == "RETURN"){
            Node* first_CHILD = node->children.front();



            TAC ta(TACType::RETURN, "", first_CHILD->value, "","");  
            ctx.current_block->tacInstructions.push_back(ta);

            // BasicBlock* newBlock = create_block(ctx.cfg);
            // //ctx.current_block->successors.push_back(newBlock); // Ensure correct flow
            // ctx.current_block = newBlock; // Switch to the new block
            return ctx.current_block;
        }
        else if(node->type =="SOMETHING ASSIGNED = TO SOMETHING"){
            Node* left = node->children.front();
            Node* right = *std::next(node->children.begin());

            //string temp = visit_expr(right, ctx);

            // TAC t(TACType::ASSIGN, left->value, temp, "", "");
            // ctx.current_block->tacInstructions.push_back(t);

            if (right->type == "exp DOT ident LP exp COMMA exp RP"){
                // handle single do not make temp variables.
                Node* firstChild = right->children.front();
                Node* secChild = *std::next(right->children.begin());
                Node* thirdChild = *std::next(right->children.begin(), 2);
                
                //std::string arg = visit_expr(thirdChild, ctx);
                // string arguments = "";
                // if (thirdChild->type == "argument_list"){
                //     if (thirdChild->children.size() == 1) 
                //     for (auto i : thirdChild->children){
                //         arguments += i->value + ",";
                //     }
                // }

                string isThis = "";
                if (firstChild->type == "THIS") isThis = "this";
                else isThis = firstChild->value;
                string arg = visit_expr(thirdChild, ctx);
                
                TAC ta(TACType::CALL, left->value, isThis +"."+ secChild->value, arg,"");  // foo2 
                ctx.current_block->tacInstructions.push_back(ta);

                // TAC ta2(TACType::JUMP, "", "", "", secChild->value, "");
                // ctx.current_block->tacInstructions.push_back(ta2);

                //BasicBlock* newBlock = create_block(ctx.cfg);
                //ctx.current_block->successors.push_back(newBlock); // Ensure correct flow
                //ctx.current_block = newBlock; // Switch to the new block
                return ctx.current_block;
            }
            else {
                TAC ta(TACType::ASSIGN, left->value, right->value, "","");
                ctx.current_block->tacInstructions.push_back(ta);
            }
        }
        else if (node->type == "IF LP expression RP statement ELSE statement"){

            Node* conditionNode = node->children.front();
            Node* thenStmtNode = *std::next(node->children.begin());
            Node* elseStmtNode = *std::next(node->children.begin(), 2);



            // 1. Evaluate condition to a temporary variable
            //std::string condTemp = visit_expr(conditionNode, ctx);
            string tempting = "";

            if (conditionNode->type == "LESS_THAN"){
                Node* f1 = conditionNode->children.front();
                Node* f2 = *std::next(conditionNode->children.begin());

                std::string condTemp1 = visit_expr(f1, ctx);
                std::string condTemp2 = visit_expr(f2, ctx);

                tempting = condTemp1 + " < " + condTemp2;
            }
            // HANDLE MORE CASES:
            else if (conditionNode->type == "MORE_THAN"){
                Node* f1 = conditionNode->children.front();
                Node* f2 = *std::next(conditionNode->children.begin());

                std::string condTemp1 = visit_expr(f1, ctx);
                std::string condTemp2 = visit_expr(f2, ctx);

                tempting = condTemp1 + " > " + condTemp2;
            }
            else {
                tempting = conditionNode->value; //for identifiers
            }

            // 2. Create basic blocks for control flow
            BasicBlock* thenBlock = create_block(ctx.cfg);
            BasicBlock* elseBlock = create_block(ctx.cfg);
            BasicBlock* mergeBlock = create_block(ctx.cfg);

            // 3. Emit conditional jump (true: thenBlock, false: elseBlock)
            TAC condJump(TACType::COND_JUMP, 
                        "",              // No destination
                        tempting,        // Condition (src1)
                        elseBlock->label, // False target (src2)
                        thenBlock->label // True target (label field)
            );
            ctx.current_block->tacInstructions.push_back(condJump);

            // Link current block to successors
            ctx.current_block->successors.push_back(thenBlock);
            ctx.current_block->successors.push_back(elseBlock);
            


            // DO IT RECURSIVE JUST CALL VISIT_STMT AGAIN.
            // 4. Process THEN block
            ctx.current_block = thenBlock;

            BasicBlock* thenEnd = visit_stmt(thenStmtNode, ctx);
            TAC thenGoto(TACType::JUMP, "", "", "", mergeBlock->label );
            thenEnd->tacInstructions.push_back(thenGoto);
            thenEnd->successors.push_back(mergeBlock);

            

            // 5. Process ELSE block
            ctx.current_block = elseBlock;
            
            BasicBlock* elseEnd = visit_stmt(elseStmtNode, ctx);
            TAC elseGoto(TACType::JUMP, "", "", "", mergeBlock->label);
            elseEnd->tacInstructions.push_back(elseGoto);
            elseEnd->successors.push_back(mergeBlock);
            
            // 6. Set merge block as new current
            ctx.current_block = mergeBlock;
            return mergeBlock;
        }

        return ctx.current_block;

    }   
    
    
    void traverse_generic(Node* node, BlockContext& ctx) {
        if (!node) return;

        // if(node->type =="SIMPLE PRINT LOL"){
        //     BasicBlock *res = visit_stmt(node,ctx);

        // }
        // else if(node->type =="SOMETHING ASSIGNED = TO SOMETHING"){
            
        //     BasicBlock *res = visit_stmt(node,ctx);
        // }
        if (node->type == "statement"){
            Node* stmt = node->children.front();
            BasicBlock* res = visit_stmt(stmt, ctx);
        }
        else if (node->type == "var declaration"){
            //handle it if the right child is subexpression or addexpression MAKE TEMP VALUES


        }
        else if (node->type == "methodDec"){
            BasicBlock *res = create_block(ctx.cfg, node->value); //provided  method name AS BLOCK NAME

            //push it back
            //ctx.cfg->addBlock(res);

            ctx.current_block = res;

            for (auto child : node->children){
                traverse_generic(child, ctx);
            }
            
        }

        else if (node->type == "methodBody"){
            for (auto child : node->children){
                traverse_generic(child, ctx);
            }
        }


        // else if (node->type == "IF LP expression RP statement ELSE statement"){
        //     BasicBlock *res = visit_stmt(node, ctx);
        // }
        else if (node->type == "RETURN"){
            BasicBlock *res = visit_stmt(node, ctx);
        }
        //default:
        else
        for(auto child : node->children){
            //std::cout <<" left To Process: " + child->type << std::endl;
            traverse_generic(child, ctx);
        }
    }
};