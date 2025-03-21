#include "Node.h"
#include "IR.h"
#include "ByteCode.h"
#include <queue>

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

// make clean; make s; make interpreter; ./interpreter output.bytecode 

class IRvisitor{
private:
    // counters
    int temp_counter = 0;
    int block_counter = 0;
    
    string curr_class_name;
    
    std::unordered_map<std::string, std::vector<std::string>> methodParams; // Store method parameters for bytecode.

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

    std::unordered_map<std::string, std::vector<std::string>> getMethodParams() {
        return methodParams;
    }

private:

    std::string visit_expr(Node* node, BlockContext& ctx, SymbolTable& st) {
        if(!node) return "";
        else if (node->type == "INT"|| node->type == "TRUE" || node->type == "FALSE"){
            
            std::string temp = this->new_temp();
            std::string value = node->value;

            // Map TRUE/FALSE to 1/0
            if (node->type == "TRUE") value = "1";
            else if (node->type == "FALSE") value = "0";

            
            return value;
        }
        else if (node->type == "identifier"){

            // TAC ta("identifiersInIF", node->value, "", "");
            // ctx.current_block->tacInstructions.push_back(ta);

            return node->value;
        }
        else if(node->type == "exp DOT ident LP exp COMMA exp RP"){
            Node* child = node->children.front();
            //std::string firstExpThis = visit_expr(child, ctx, st); //NEW Bar

            std::string temp = this->new_temp();

            //string nameOfClass = firstExpThis;
            // if (child->type == "NEW identifier LP RP"){
            //     Node* childChild = child->children.front();

            //     nameOfClass = visit_expr(childChild,ctx, st); //BAR
            // }
            string classNAME; // = nameOfClass;
            if (child->type == "THIS"){
                classNAME = curr_class_name;
            }   
            // COME HERE
            Node* getFuncName = *std::next(node->children.begin()); //aka FOO
            Node* argsNode = *std::next(node->children.begin(),2); // Arguments
            //std::string argruments = visit_expr(argsNode,ctx, st);  //can be argument_list or emptyArgumet
            
            int argCount = argsNode->children.size();
            
            


            // Process arguments
            for (auto arg : argsNode->children) {
                std::string argTemp = visit_expr(arg, ctx, st);
                TAC argTAC("Args", "", argTemp, "");
                ctx.current_block->tacInstructions.push_back(argTAC);
            }

            
        

            if (getFuncName && !getFuncName->value.empty()) {
                //  std::to_string(argCount) = Bar.foo
                //string test = nameOfClass + "." + getFuncName->value;
                
                if (child->type == "NEW identifier LP RP"){
                    classNAME = child->children.front()->value;
                }
                // COME HERE4
                //std::string methodLabel = curr_class_name + "." + getFuncName->value;

                

                string methodLabel = classNAME + "." + getFuncName->value;
                TAC ta("CALL", temp, classNAME, methodLabel);  
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
                if (firstChild->type == "THIS") isThis = "this"; // it was "this" before
                else isThis = firstChild->value;
                
                int argCount = thirdChild->children.size();
                
                //make a tac instruction for class_name
                TAC taArg("Args", "", isThis, "");
                ctx.current_block->tacInstructions.push_back(taArg);

                for(auto child : thirdChild->children){
                    string arg = visit_expr(child, ctx, st);
                    TAC ta("Args", "", arg,"");  

                    ctx.current_block->tacInstructions.push_back(ta);

                }
                string getFullClassAndMethod = curr_class_name + "." + secChild->value;
                // COME HERE3
                //TAC ta("CALL", left->value, isThis +"."+ secChild->value, to_string(argCount));  // foo2
                
                
                
                TAC ta("CALL", left->value, isThis, getFullClassAndMethod);  // this 
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
                string constIfNotTrue;
                if (right->type == "TRUE") constIfNotTrue = "1";
                else if (right->type == "FALSE") constIfNotTrue = "0";
                else constIfNotTrue = right->value;

                //ADD TAC FOR CONST:
                TAC taC("CONST", rightVal, constIfNotTrue, "");
                ctx.current_block->tacInstructions.push_back(taC);

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
            TAC thenGoto("JUMP",  mergeBlock->label, "doit", "" ); // "doit" is for bytecode to generate block_5 for example for the first THEN block
            thenEnd->tacInstructions.push_back(thenGoto);
            thenEnd->successors.push_back(mergeBlock);

            

            // 5. Process ELSE block
            ctx.current_block = elseBlock;
            //add label
            TAC taLabelForThenEnd("LABEL", elseBlock->label, "", "");
            ctx.current_block->tacInstructions.push_back(taLabelForThenEnd);
            
            BasicBlock* elseEnd = visit_stmt(elseStmtNode, ctx, st);
            

            TAC elseGoto("JUMP", mergeBlock->label ,"", "");
            elseEnd->tacInstructions.push_back(elseGoto);
            elseEnd->successors.push_back(mergeBlock);
            
            
            
            // 6. Set merge block as new current
            ctx.current_block = mergeBlock;

            // add label
            TAC ta("LABEL", mergeBlock->label, "", "");
            ctx.current_block->tacInstructions.push_back(ta);
            

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
            

            // add label
            TAC ta2("LABEL", whileCondition->label, "", "");
            ctx.current_block->tacInstructions.push_back(ta2);
            //COME HERE5
            // 2. Jump to the condition block from the current block
            // TAC jumpToCond("JUMP", whileCondition->label, "", "");
            // ctx.current_block->tacInstructions.push_back(jumpToCond);
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

            

            // // add label
            // TAC ta("JUMP", "", "loopStart", ""); // WHILE LOOP NEEDS TO JUMP BACK
            // ctx.current_block->tacInstructions.push_back(ta);

            TAC ta3("LABEL", whileExit->label, "", "");
            ctx.current_block->tacInstructions.push_back(ta3);

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
            
            string resThis = curr_class_name + "." + node->value; // also that is has a class name
            BasicBlock *res = create_block(ctx.cfg); //provided  method name AS BLOCK NAME

            // NEW CHANGES:
            ctx.current_block->successors.push_back(res); // Ensure correct flow

            TAC ta("ENTRY",resThis,"","");
            res->tacInstructions.push_back(ta);

            ctx.current_block = res;
            
            Node* methodParamsNode = *std::next(node->children.begin(), 2); // Parameters

            // Process method parameters
            for (auto param : methodParamsNode->children) {
                Node* paramVar = *std::next(param->children.begin()); // Variable name
                std::string paramName = paramVar->value;
                methodParams[resThis].push_back(paramName);
            }
            

            for (auto child : node->children){
                traverse_generic(child, ctx, st);
            }
            
        }

        else if (node->type == "methodBody"){
            for (auto child : node->children){
                traverse_generic(child, ctx, st);
            }
        }
        else if (node->type == "MAIN CLASS"){
            for (auto child : node->children){
                traverse_generic(child, ctx, st);
            }
            TAC ta ("EXIT","","","");
            ctx.current_block->tacInstructions.push_back(ta);
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

bool isClassName(const std::string& name, SymbolTable& symbolTable) {
    // Use the symbol table to check if the name corresponds to a class
    return symbolTable.get_class_scope(name) != nullptr;
}
bool willBeUsedAgain(const std::string& var, const TAC& currentTac, const std::vector<TAC>& tacList, bool isThisAnFunction) {
    
    if (isThisAnFunction) return false;
    // Iterate through the remaining TAC instructions
    for (const auto& tac : tacList) {
        if (&tac == &currentTac) continue; // Skip the current instruction

        // Check if the variable is used in src1, src2, or dest
        if (tac.src1 == var || tac.src2 == var || tac.dest == var) {
            return true; // Variable will be used again
        }
    }
    return false; // Variable will not be used again
}

bool checkIfThisStatementDoesContainAnCall(const std::string& var, const TAC& currentTac, const std::vector<TAC>& tacList) {
    
    //check if the currentTac is a CALL
    if (currentTac.op == "CALL") return true;
    // Iterate through the remaining TAC instructions
    for (const auto& tac : tacList) {
        //check if in the statement there is a CALL
        if (tac.op == "CALL") return true;
    }
    return false; // Variable will not be used again
}

void generateByteCode(CFG* cfg, ByteCode& byteCode, SymbolTable& symbolTable, std::unordered_map<std::string, std::vector<std::string>>& methodParams) {
    std::unordered_set<BasicBlock*> visitedBlocks;
    //std::vector<BasicBlock*> stack = {cfg->entry_block};
    std::string lastInstruction = ""; // Track the last instruction type
    std::queue<BasicBlock*> queue; // Use a queue for breadth-first traversal
    queue.push(cfg->entry_block);
    bool dontDoIstore = false; // Flag to prevent istore or iload for parameters
    while (!queue.empty()) {
        BasicBlock* block = queue.front();
        queue.pop();

        if (visitedBlocks.count(block)) continue;
        visitedBlocks.insert(block);
        
        std::cout << "Processing block: " << block->label << std::endl;
        for (auto successor : block->successors) {
            std::cout << "  Successor: " << successor->label << std::endl;
        }
        for (const auto& tac : block->tacInstructions) {
            if (tac.op == "ASSIGN") {
                byteCode.addInstruction("istore", tac.dest);
            } else if (tac.op == "ADD") {
                
                //check if we already have the src1 or src2 in the stack
                //if we have just do iadd

                // THIS IS A BIG TRADEOFF. IF ADDED OR NOT
                // if (willBeUsedAgain(tac.src1, tac, block->tacInstructions)) {
                //     byteCode.addInstruction("iadd");
                //     continue;
                // } 

                //check if src1 or src2 are constants
                if (isdigit(tac.src1[0]) || (tac.src1[0] == '-' && isdigit(tac.src1[1]))) {
                    byteCode.addInstruction("iconst", tac.src1);
                } else if (!dontDoIstore) {
                    byteCode.addInstruction("iload", tac.src1);

                    // // Check if src1 will be used again
                    // if (willBeUsedAgain(tac.src1, tac, block->tacInstructions)) { // FOR D3.java or BIGGER EXPRESSIONS..
                    //     byteCode.addInstruction("iload", tac.src1); // Push it again
                    // }
                }
                if (isdigit(tac.src2[0]) || (tac.src2[0] == '-' && isdigit(tac.src2[1]))) {
                    byteCode.addInstruction("iconst", tac.src2);
                } else if (!dontDoIstore){
                    byteCode.addInstruction("iload", tac.src2);
                }
                dontDoIstore = false;
                byteCode.addInstruction("iadd");
                //byteCode.addInstruction("istore", tac.dest);
            } else if (tac.op == "SUB") {
                

                //check if src1 or src2 are constants
                if (isdigit(tac.src1[0]) || (tac.src1[0] == '-' && isdigit(tac.src1[1]))) {
                    byteCode.addInstruction("iconst", tac.src1);
                } else {
                    byteCode.addInstruction("iload", tac.src1);

                    // Check if src1 will be used again
                    // if (willBeUsedAgain(tac.src1, tac, block->tacInstructions, dontDoIstore)) { // FOR D3.java or BIGGER EXPRESSIONS..
                    //     byteCode.addInstruction("iload", tac.src1); // Push it again // WORKS SOMETIMES:
                    // }
                    if (checkIfThisStatementDoesContainAnCall(tac.src1, tac, block->tacInstructions)) {
                        cout << "NDAWOINDAWOIDNAWOIDN" <<endl;
                        byteCode.addInstruction("iload", tac.src1); // Push it again // WORKS SOMETIMES:
                    }
                }
                if (isdigit(tac.src2[0]) || (tac.src2[0] == '-' && isdigit(tac.src2[1]))) {
                    byteCode.addInstruction("iconst", tac.src2);
                } else {
                    byteCode.addInstruction("iload", tac.src2);
                }
                dontDoIstore = false;
                byteCode.addInstruction("isub");
                //byteCode.addInstruction("istore", tac.dest);
            } else if (tac.op == "MULT") {
                
                //check if src1 or src2 are constants
                if (isdigit(tac.src1[0]) || (tac.src1[0] == '-' && isdigit(tac.src1[1]))) {
                    byteCode.addInstruction("iconst", tac.src1);
                } else {
                    byteCode.addInstruction("iload", tac.src1);

                    // Check if src1 will be used again
                    if (willBeUsedAgain(tac.src1, tac, block->tacInstructions, dontDoIstore)) { // FOR D3.java or BIGGER EXPRESSIONS..
                        byteCode.addInstruction("iload", tac.src1); // Push it again
                    }
                }
                if (isdigit(tac.src2[0]) || (tac.src2[0] == '-' && isdigit(tac.src2[1]))) {
                    byteCode.addInstruction("iconst", tac.src2);
                } else {
                    byteCode.addInstruction("iload", tac.src2);
                }
                dontDoIstore = false;
                byteCode.addInstruction("imul");
                //byteCode.addInstruction("istore", tac.dest);
            } else if (tac.op == "PRINT") {
                
                //check if the src1 is an parameter then dont do istore, check in the methodParams
                for (const auto& [method, params] : methodParams) {
                    if (std::find(params.begin(), params.end(), tac.src1) != params.end()) {
                        dontDoIstore = true;
                        break;
                    }
                }
                // add istore 
                //byteCode.addInstruction("istore", tac.src1);
                if (isdigit(tac.src1[0]) || (tac.src1[0] == '-' && isdigit(tac.src1[1]))) {
                    // If it's a constant, use iconst
                    byteCode.addInstruction("iconst", tac.src1);
                } else if (!dontDoIstore && (tac.src1[0] == '_' || isalpha(tac.src1[0]))) { // DO NOT HANDLE CONSTANTS!!
                    // Otherwise, use iload for variables or other values
                    byteCode.addInstruction("istore", tac.src1);
                    byteCode.addInstruction("iload", tac.src1);
                }
                else if (dontDoIstore && (tac.src1[0] == '_' || isalpha(tac.src1[0]))){
                    byteCode.addInstruction("iload", tac.src1);
                }
                dontDoIstore = false;
                //byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("print");
            } else if (tac.op == "RETURN") {
                // Check if tac.src1 is a constant
                if (isdigit(tac.src1[0]) || (tac.src1[0] == '-' && isdigit(tac.src1[1]))) {
                    // If it's a constant, use iconst
                    byteCode.addInstruction("iconst", tac.src1);
                } else {
                    // Otherwise, use iload for variables or other values
                    byteCode.addInstruction("iload", tac.src1);
                }
                byteCode.addInstruction("ireturn");
            } else if (tac.op == "COND_JUMP") {
                //add a goto "else_foo2" if wanted otherwise it just works with block labels.
                if (tac.dest[0] != '_') { // DO NOT HANDLE TEMPS!!
                    //byteCode.addInstruction("istore", tac.dest);
                    byteCode.addInstruction("iload", tac.dest);
                }
                // check if its an while loop that starts with whileExit
                if (tac.src2.rfind("whileExit", 0) == 0) { // Check if tac.src2 starts with "whileExit"
                    byteCode.addInstruction("iffalse goto", tac.src2); // go to else block
                } else {
                    byteCode.addInstruction("iffalse goto", tac.src1); // go to then block
                }
                //byteCode.addInstruction("iffalse goto", tac.src1); // go to else block
                
                //byteCode.addInstruction("goto", tac.src1);
            } else if (tac.op == "JUMP") {
                if (tac.src1 == "doit"){
                    byteCode.addInstruction("goto", tac.dest);
                }
                else if (tac.dest.rfind("whileCondition", 0) == 0) { // Check if tac.src1 starts with "whileCondition"
                    cout << "DNAWIODNAWDIUAWBNI"<<endl;
                    byteCode.addInstruction("goto", tac.dest);
                }
            }
            else if (tac.op == "CALL") {
                dontDoIstore = true;
                // Call the method
                byteCode.addInstruction("invokevirtual", tac.src2); // Method label
                if (tac.dest[0] != '_') { // Check if it's not a temporary variable
                    byteCode.addInstruction("istore", tac.dest); // Store the return value CHANGED
                }
                //byteCode.addInstruction("iload", tac.dest); // Load the return value
            }
            else if (tac.op == "NEW") {
                byteCode.addInstruction("new", tac.src1); // Class name
                byteCode.addInstruction("istore", tac.dest); // Store object reference
            }
            else if (tac.op == "Args") {
                if (isdigit(tac.src1[0]) || (tac.src1[0] == '-' && isdigit(tac.src1[1]))) {
                    byteCode.addInstruction("iconst", tac.src1);
                    lastInstruction = "iconst";
                } else if (tac.src1 == "this") {
                    //byteCode.addInstruction("aload", tac.src1);
                    lastInstruction = "aload";
                } 
                else if (tac.src1[0] == '_') {
                    // Skip handling temporary variables
                    continue;
                } 
                else {
                    if (lastInstruction != "iconst") { // Skip iload if the last instruction was iconst
                        //byteCode.addInstruction("iload", tac.src1);
                    }
                    //byteCode.addInstruction("istore", tac.src1);
                    byteCode.addInstruction("iload", tac.src1); //CHANGED
                    lastInstruction = "iload";
                }
            }
            else if (tac.op == "EQUAL") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("equal");
                //byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "OR") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("ior");
                //byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "AND") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("iload", tac.src2);
                byteCode.addInstruction("iand");
                //byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "LESS_THAN") {
                // Check if src1 is a digit
                if (isdigit(tac.src1[0]) || (tac.src1[0] == '-' && isdigit(tac.src1[1]))) {
                    byteCode.addInstruction("iconst", tac.src1);
                } else {
                    //byteCode.addInstruction("istore", tac.src1);
                    byteCode.addInstruction("iload", tac.src1);
                }

                // Check if src2 is a digit
                if (isdigit(tac.src2[0]) || (tac.src2[0] == '-' && isdigit(tac.src2[1]))) {
                    byteCode.addInstruction("iconst", tac.src2);
                } else {
                    //byteCode.addInstruction("istore", tac.src2);
                    byteCode.addInstruction("iload", tac.src2);
                }
                byteCode.addInstruction("ilt");
                //byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "MORE_THAN") {
                // Check if src1 is a digit
                if (isdigit(tac.src1[0]) || (tac.src1[0] == '-' && isdigit(tac.src1[1]))) {
                    byteCode.addInstruction("iconst", tac.src1);
                } else {
                    byteCode.addInstruction("iload", tac.src1);
                }

                // Check if src2 is a digit
                if (isdigit(tac.src2[0]) || (tac.src2[0] == '-' && isdigit(tac.src2[1]))) {
                    byteCode.addInstruction("iconst", tac.src2);
                } else {
                    byteCode.addInstruction("iload", tac.src2);
                }
                byteCode.addInstruction("igt");
                //byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "NOT") {
                byteCode.addInstruction("iload", tac.src1);
                byteCode.addInstruction("inot");
                //byteCode.addInstruction("istore", tac.dest);
            }
            else if (tac.op == "CLASS") {
                byteCode.addInstruction("class", tac.dest);
            }
            else if (tac.op == "METHOD") {
                byteCode.addInstruction("method", tac.dest, tac.src1);
            }
            else if (tac.op == "ENTRY") {
                byteCode.addInstruction("label", tac.dest);
                //cout << "DWDWADWADAWDWAD"<<endl;
                cout << "LABEL: " << tac.dest << endl;
                // maybe show all arguments in the method with "iload" and "iconst" ?
                //use methodParams
                //reverse methodParams for the stack implementation:

                
                
                auto it = methodParams.find(tac.dest);
                if (it != methodParams.end()) {
                std::vector<std::string> reversedParams(it->second.rbegin(), it->second.rend());
                for (const auto& param : reversedParams) {
                    byteCode.addInstruction("istore", param);
                }
                // if (it != methodParams.end()) {
                //     for (const auto& param : it->second) {
                //         byteCode.addInstruction("istore", param);
                //     }
                // }
                }
            }
            else if (tac.op == "EXIT") {
                byteCode.addInstruction("stop");
            }
            else if (tac.op == "CONST") {
                byteCode.addInstruction("iconst", tac.src1); // Push constant value onto the stack
                //byteCode.addInstruction("istore", tac.dest); // Store it in a temporary variable
            }
            else if (tac.op == "LABEL") {
                byteCode.addInstruction("label", tac.dest);           
            }
            // else {
            //     std::cerr << "Error: Unrecognized TAC operation: " << tac.op << std::endl;
            // }
        }
        
        for (auto successor : block->successors) {
            if (!visitedBlocks.count(successor)) {
                queue.push(successor);
            }
        }
        // Ensure the true block is processed before the false block
    }

    // for (auto block : cfg->blocks) {
    //     std::cout << "Block: " << block->label << "\n";
    //     for (auto successor : block->successors) {
    //         std::cout << "  Successor: " << successor->label << "\n";
    //     }
    // }
}
/*
; Main method
iconst 100
invokevirtual Sum.calcSum
print
stop

; Sum.calcSum method
label Sum.calcSum
iconst 0           ; Initialize sum = 0
istore sum

label loopStart
iload num           ; Check loop condition: 0 < num
iconst 0
ilt                 ; Push 1 if 0 < num, else 0
iffalse goto loopExit ; Exit loop if condition is false

iload sum           ; sum = sum + num
iload num
iadd
istore sum

iload num           ; num = num - 1
iconst 1
isub
istore num

goto loopStart      ; Repeat loop

label loopExit
iload sum           ; Return sum
ireturn

*/


/*
D1.java:
# Main program
new Sum             # Create new Sum object
istore __t0         # Store object reference
iload __t0          # Load object for method call
iconst 100          # Push argument 100
invokevirtual Sum.calcSum  # Call calcSum(100)
istore __t1         # Store result (5050)
iload __t1          # Load result for printing
print               # Print 5050
stop                # End program

# Sum.calcSum method
label Sum.calcSum
istore num          # Store parameter: num = 100
iconst 0            
istore sum          # sum = 0

label loop_start
iload num           # Load num for comparison
iconst 0
igt                 # Check if num > 0
iffalse goto loop_end  # Exit loop if false

# Loop body - sum += num
iload sum
iload num
iadd
istore sum          # sum = sum + num

# Decrement num
iload num
iconst 1
isub
istore num          # num = num - 1

goto loop_start     # Repeat loop

label loop_end
iload sum           # Load final sum value
ireturn             # Return 5050

*/