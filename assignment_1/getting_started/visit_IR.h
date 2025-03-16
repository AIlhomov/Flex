#include "Node.h"
#include "IR.h"
#include "ByteCode.h"


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
    
    string curr_class_name;

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

    CFG* generate_IR(Node* root, SymbolTable& st) {
        CFG* cfg = new CFG(); // Create a CFG on the heap
        BasicBlock* entry_block = create_block(cfg); // Pass CFG to create_block
        cfg->entry_block = entry_block;
        cfg->entry_block->label = "Main";
        BlockContext ctx{entry_block, cfg};
        traverse_generic(root, ctx, st);

        return cfg; // Return the fully built CFG
    }

private:

    std::string visit_expr(Node* node, BlockContext& ctx, SymbolTable& st) {
        if(!node) return "";
        else if (node->type == "INT"|| node->type == "TRUE" || node->type == "FALSE" || node->type == "identifier"){
            return node->value;
        }
        else if(node->type == "exp DOT ident LP exp COMMA exp RP"){
            Node* child = node->children.front();
            std::string firstExpThis = visit_expr(child, ctx, st); //NEW Bar

            std::string temp = this->new_temp();

            string nameOfClass = firstExpThis;
            if (child->type == "NEW identifier LP RP"){
                Node* childChild = child->children.front();

                nameOfClass = visit_expr(childChild,ctx, st); //BAR
            }
    
            // if (node->children.front()->type == "THIS"){
                
            //     firstExpThis = curr_class_name;
            // }   
            // COME HERE
            Node* getFuncName = *std::next(node->children.begin()); //aka FOO
            Node* argNode = *std::next(node->children.begin(),2); //aka FOO
            std::string argruments = visit_expr(argNode,ctx, st);  //can be argument_list or emptyArgumet
            
            int argCount = argNode->children.size();
            std::cout << "argNodeVal  : " + argNode->value << endl;
            std::cout << "argNodeType  : " + argNode->type << endl;



            for(auto child : argNode->children){
                std::string argrument = visit_expr(child,ctx, st);  //can be argument_list or emptyArgumet

                std::cout << "arguments are : " + argrument << endl;
                TAC ta("Args", "", argrument,"");  

                ctx.current_block->tacInstructions.push_back(ta);

            }

            
        

            if (getFuncName && !getFuncName->value.empty()) {
                cout << "firstExpThis : " + firstExpThis << endl; 
                //  std::to_string(argCount) = Bar.foo
                string test = nameOfClass + "." + getFuncName->value;

                TAC ta("CALL", temp, firstExpThis, test);  
                ctx.current_block->tacInstructions.push_back(ta);
            } else {
                std::cerr << "Error: Invalid method name or null node in CALL operation." << std::endl;
            }



            return temp;
        }

        else if(node->type =="NEW identifier LP RP"){

            Node* idNode = node->children.front();
            std::string temp = this->new_temp();
            TAC ta("NEW", temp, idNode->value, "");
            ctx.current_block->tacInstructions.push_back(ta);
            return temp;

        }


        else if(node->type == "argument_list"){
            std::string args;
            for (auto arg : node->children){
                if (!args.empty()) args += ",";
                args += visit_expr(arg, ctx, st);
            }
            return args; 
        }

        else if(node->type == "argument"){
            return visit_expr(node->children.front(), ctx, st);
        }
        else if (node->type == "LESS THAN"){

        }
        else if(node->type == "LC statement RC"){
            return visit_expr(node->children.front(),ctx, st);
        }
        else if(node->type == "statements"){
            std::string stmts;
            for (auto arg : node->children){
                if (node->children.size()-1 >0) stmts += ",";
                
                stmts += visit_expr(arg, ctx, st);
            }
            return stmts;        
        }
        else if (node->type == "SubExpression"){
            Node* leftVal = node->children.front();
            Node* rightVal = *std::next(node->children.begin());

            string getExpLeft = visit_expr(leftVal, ctx, st);
            string getExpRight = visit_expr(rightVal, ctx, st);

            std::string temp = this->new_temp();
            TAC ta("SUB", temp, getExpLeft, getExpRight);
            ctx.current_block->tacInstructions.push_back(ta);
            return temp;
        }

        else if (node->type == "AddExpression"){
            Node* leftVal = node->children.front();
            Node* rightVal = *std::next(node->children.begin());

            string getExpLeft = visit_expr(leftVal, ctx, st);
            string getExpRight = visit_expr(rightVal, ctx, st);

            std::string temp = this->new_temp();
            TAC ta("ADD", temp, getExpLeft, getExpRight);
            ctx.current_block->tacInstructions.push_back(ta);
            return temp;
        }
        if (node->type == "LESS_THAN"){
            std::string temp = this->new_temp();

            Node* f1 = node->children.front();
            Node* f2 = *std::next(node->children.begin());

            
            
            std::string condTemp1 = visit_expr(f1, ctx, st);
            std::string condTemp2 = visit_expr(f2, ctx, st);
            TAC ta("LESS_THAN", temp, condTemp1, condTemp2);
            ctx.current_block->tacInstructions.push_back(ta);


            return temp;
        }
        // HANDLE MORE CASES:
        else if (node->type == "MORE_THAN"){
            std::string temp = this->new_temp();

            Node* f1 = node->children.front();
            Node* f2 = *std::next(node->children.begin());

            
            std::string condTemp1 = visit_expr(f1, ctx, st);
            std::string condTemp2 = visit_expr(f2, ctx, st);
            TAC ta("MORE_THAN", temp, condTemp1, condTemp2);
            ctx.current_block->tacInstructions.push_back(ta);

            return temp;
        }
        else if (node->type == "OR"){
            std::string temp = this->new_temp();

            Node* f1 = node->children.front();
            Node* f2 = *std::next(node->children.begin());

            
            std::string condTemp1 = visit_expr(f1, ctx, st);
            std::string condTemp2 = visit_expr(f2, ctx, st);
            TAC ta("OR", temp, condTemp1, condTemp2);
            ctx.current_block->tacInstructions.push_back(ta);

            return temp;
        }
        else if (node->type == "EQUAL"){
            std::string temp = this->new_temp();

            Node* f1 = node->children.front();
            Node* f2 = *std::next(node->children.begin());

            
            std::string condTemp1 = visit_expr(f1, ctx, st);
            std::string condTemp2 = visit_expr(f2, ctx, st);
            TAC ta("EQUAL", temp, condTemp1, condTemp2);
            ctx.current_block->tacInstructions.push_back(ta);

            return temp;
        }
        else if (node->type == "AND"){
            std::string temp = this->new_temp();

            Node* f1 = node->children.front();
            Node* f2 = *std::next(node->children.begin());

            
            std::string condTemp1 = visit_expr(f1, ctx, st);
            std::string condTemp2 = visit_expr(f2, ctx, st);
            TAC ta("AND", temp, condTemp1, condTemp2);
            ctx.current_block->tacInstructions.push_back(ta);

            return temp;
        }

        else if (node->type == "EXCLAMATION_MARK expression"){
            std::string temp = this->new_temp();

            Node* f1 = node->children.front();

            
            std::string condTemp1 = visit_expr(f1, ctx, st);
            TAC ta("NOT", temp, condTemp1, "");
            ctx.current_block->tacInstructions.push_back(ta);

            return temp;
        }

        else if (node->type == "MultExpression"){
            std::string temp = this->new_temp();

            Node* f1 = node->children.front();
            Node* f2 = *std::next(node->children.begin());

            
            std::string condTemp1 = visit_expr(f1, ctx, st);
            std::string condTemp2 = visit_expr(f2, ctx, st);
            TAC ta("MULT", temp, condTemp1, condTemp2);
            ctx.current_block->tacInstructions.push_back(ta);

            return temp;
        }

        

        return "";

    }

    BasicBlock* visit_stmt(Node* node, BlockContext& ctx, SymbolTable& st) {
        //if(!node) return ctx.current_block;
        if(!node) return nullptr;
        else if(node->type == "SIMPLE PRINT LOL"){    
            std::string t = visit_expr(node->children.front(),ctx, st);
            TAC ta ("PRINT","",t,"");
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
            if (node->children.size() == 1) return visit_stmt(node->children.front(),ctx, st);
        }
        else if (node->type == "statements"){
            for (auto child : node->children){
                 visit_stmt(child, ctx, st);
            }

        }
        else if(node->type == "statement"){
            std::cout << "uuuuuuuuuuUUU";
            if (node->children.size() == 1) return visit_stmt(node->children.front(),ctx, st);
        }

        else if (node->type == "RETURN"){
            Node* first_CHILD = node->children.front();



            TAC ta("RETURN", "", first_CHILD->value, "");  
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
            if(right->type == "MultExpression" || right->type == "AddExpression"|| right->type == "SubExpression" ) {

                std::string src = visit_expr(right, ctx, st);
                TAC ta("ASSIGN", left->value, src, "");  // foo2 
                ctx.current_block->tacInstructions.push_back(ta);
                return ctx.current_block;

            } 
            
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
                
                int argCount = thirdChild->children.size();
                
                for(auto child : thirdChild->children){
                    string arg = visit_expr(child, ctx, st);
                    TAC ta("Args", "", arg,"");  

                    ctx.current_block->tacInstructions.push_back(ta);

                }


                TAC ta("CALL", left->value, isThis +"."+ secChild->value, to_string(argCount));  // foo2 
                ctx.current_block->tacInstructions.push_back(ta);

                // TAC ta2(TACType::JUMP, "", "", "", secChild->value, "");
                // ctx.current_block->tacInstructions.push_back(ta2);

                //BasicBlock* newBlock = create_block(ctx.cfg);
                //ctx.current_block->successors.push_back(newBlock); // Ensure correct flow
                //ctx.current_block = newBlock; // Switch to the new block
                return ctx.current_block;
            }

            

            else {
                string leftVal = visit_expr(left,ctx, st);
                string rightVal = visit_expr(right,ctx, st);

                TAC ta("ASSIGN", leftVal, rightVal, "");
                ctx.current_block->tacInstructions.push_back(ta);
                return ctx.current_block;


            }
        }
        else if (node->type == "IF LP expression RP statement ELSE statement"){

            Node* conditionNode = node->children.front();
            Node* thenStmtNode = *std::next(node->children.begin());
            Node* elseStmtNode = *std::next(node->children.begin(), 2);



            // 1. Evaluate condition to a temporary variable
            //std::string condTemp = visit_expr(conditionNode, ctx);
            string tempting = visit_expr(conditionNode,ctx, st);

            

            // 2. Create basic blocks for control flow
            BasicBlock* thenBlock = create_block(ctx.cfg);
            BasicBlock* elseBlock = create_block(ctx.cfg);
            BasicBlock* mergeBlock = create_block(ctx.cfg);

            // 3. Emit conditional jump (true: thenBlock, false: elseBlock)
            TAC condJump("COND_JUMP", 
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

            BasicBlock* thenEnd = visit_stmt(thenStmtNode, ctx, st);
            TAC thenGoto("JUMP",  mergeBlock->label, "", "" );
            thenEnd->tacInstructions.push_back(thenGoto);
            thenEnd->successors.push_back(mergeBlock);

            

            // 5. Process ELSE block
            ctx.current_block = elseBlock;
            
            BasicBlock* elseEnd = visit_stmt(elseStmtNode, ctx, st);
            TAC elseGoto("JUMP", mergeBlock->label ,"", "");
            elseEnd->tacInstructions.push_back(elseGoto);
            elseEnd->successors.push_back(mergeBlock);
            
            // 6. Set merge block as new current
            ctx.current_block = mergeBlock;
            return mergeBlock;
        }


        else if (node->type == "WHILE LP expression RP statement") {
            // Extract condition and body nodes
            Node* conditionNode = node->children.front(); 
            Node* bodyNode = *std::next(node->children.begin());
        
            // 1. Create blocks
            BasicBlock* whileCondition = create_block(ctx.cfg, "whileCondition_" + std::to_string(block_counter++));
            BasicBlock* whileBody = create_block(ctx.cfg, "whileBody_" + std::to_string(block_counter++));
            BasicBlock* whileExit = create_block(ctx.cfg, "whileExit_" + std::to_string(block_counter++));
        
            // 2. Jump to the condition block from the current block
            TAC jumpToCond("JUMP", whileCondition->label, "", "");
            ctx.current_block->tacInstructions.push_back(jumpToCond);
            ctx.current_block->successors.push_back(whileCondition);
        
            // 3. Evaluate the condition
            ctx.current_block = whileCondition;
            std::string conditionTemp = visit_expr(conditionNode, ctx, st); // Evaluate condition
            TAC condJump("COND_JUMP", conditionTemp, whileBody->label, whileExit->label);
            whileCondition->tacInstructions.push_back(condJump);
            whileCondition->successors.push_back(whileBody);
            whileCondition->successors.push_back(whileExit);
        
            // 4. Process while body
            ctx.current_block = whileBody;
            BasicBlock* bodyEnd = visit_stmt(bodyNode, ctx, st);
        
            // 5. Add a jump back to the condition
            TAC loopBack("JUMP", whileCondition->label, "","");
            bodyEnd->tacInstructions.push_back(loopBack);
            bodyEnd->successors.push_back(whileCondition);
        
            // 6. Continue execution in the exit block
            ctx.current_block = whileExit;
            return whileExit;
        }
                



        return ctx.current_block;

    }   
    
    
    void traverse_generic(Node* node, BlockContext& ctx, SymbolTable& st) {
        if (!node) return;

        // if(node->type =="SIMPLE PRINT LOL"){
        //     BasicBlock *res = visit_stmt(node,ctx);

        // }
        // else if(node->type =="SOMETHING ASSIGNED = TO SOMETHING"){
            
        //     BasicBlock *res = visit_stmt(node,ctx);
        // }
        if (node->type == "statement"){
            Node* stmt = node->children.front();
            BasicBlock* res = visit_stmt(stmt, ctx, st);
        }
        else if (node->type == "var declaration"){
            //handle it if the right child is subexpression or addexpression MAKE TEMP VALUES


        }
        else if (node->type == "classDeclarations"){ // used just for the EXIT tac instruction
            TAC ta ("EXIT","","","");
            ctx.current_block->tacInstructions.push_back(ta);
            for (auto child : node->children){
                traverse_generic(child, ctx, st);
            }
        }
        else if (node->type == "classDeclaration"){
            curr_class_name = node->value;
            for (auto child : node->children){
                traverse_generic(child, ctx, st);
            }
        }
        else if (node->type == "methodDec"){
            
            string resThis = curr_class_name + "_" + node->value; // also that is has a class name
            BasicBlock *res = create_block(ctx.cfg, resThis); //provided  method name AS BLOCK NAME

            ctx.current_block = res;
            
            for (auto child : node->children){
                traverse_generic(child, ctx, st);
            }
            
        }

        else if (node->type == "methodBody"){
            for (auto child : node->children){
                traverse_generic(child, ctx, st);
            }
        }


        // else if (node->type == "IF LP expression RP statement ELSE statement"){
        //     BasicBlock *res = visit_stmt(node, ctx);
        // }
        else if (node->type == "RETURN"){
            BasicBlock *res = visit_stmt(node, ctx, st);
        }
        //default:
        else
        for(auto child : node->children){
            //std::cout <<" left To Process: " + child->type << std::endl;
            traverse_generic(child, ctx, st);
        }
    }
};

void generateByteCode(CFG* cfg, ByteCode& byteCode) {
    std::unordered_set<BasicBlock*> visitedBlocks;
    std::vector<BasicBlock*> stack = {cfg->entry_block};

    while (!stack.empty()) {
        BasicBlock* block = stack.back();
        stack.pop_back();

        if (visitedBlocks.count(block)) continue;
        visitedBlocks.insert(block);

        for (const auto& tac : block->tacInstructions) {
            if (tac.op == "ASSIGN") {
                byteCode.addInstruction("istore", tac.dest);
            } else if (tac.op == "ADD") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("iadd");
                byteCode.addInstruction("istore", tac.dest);
            } else if (tac.op == "SUB") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("isub");
                byteCode.addInstruction("istore", tac.dest);
            } else if (tac.op == "MULT") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("imul");
                byteCode.addInstruction("istore", tac.dest);
            } else if (tac.op == "PRINT") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("print");
            } else if (tac.op == "RETURN") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("return");
            } else if (tac.op == "COND_JUMP") {
                byteCode.addInstruction("iload", tac.dest);
                byteCode.addInstruction("iffalse", tac.src2);
                byteCode.addInstruction("goto", tac.src1);
            } else if (tac.op == "JUMP") {
                byteCode.addInstruction("goto", tac.dest);
            }
            else if (tac.op == "CALL") {
                // Push arguments onto the stack
                // COME HERE2
                byteCode.addInstruction("iload", tac.src1); // Object reference
                byteCode.addInstruction("invoke", tac.src2); // Method label
                byteCode.addInstruction("istore", tac.dest); // Store the return value
            }
            else if (tac.op == "NEW") {
                byteCode.addInstruction("new", tac.src1); // Class name
                byteCode.addInstruction("istore", tac.dest); // Store object reference
            }
            else if (tac.op == "Args") {
                byteCode.addInstruction("iload", tac.src1); // Push argument onto the stack
            }
            else if (tac.op == "EQUAL") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("equal");
                byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "OR") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("ior");
                byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "AND") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("iand");
                byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "LESS_THAN") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("ilt");
                byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "MORE_THAN") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("igt");
                byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "NOT") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("inot");
                byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "CLASS") {
                byteCode.addInstruction("class", tac.dest);
            }
            else if (tac.op == "METHOD") {
                byteCode.addInstruction("method", tac.dest, tac.src1);
            }
            else if (tac.op == "LABEL") {
                byteCode.addInstruction("label", tac.dest);
            }
            else if (tac.op == "EXIT") {
                byteCode.addInstruction("exit");
            }
        }
        
        for (auto successor : block->successors) {
            stack.push_back(successor);
        }
    }
}