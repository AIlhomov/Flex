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
    BasicBlock* create_block(CFG* cfg) {
        BasicBlock* block = new BasicBlock();
        block->label = "block_" + std::to_string(block_counter++); // Unique label
        cfg->addBlock(block); // Add to CFG
        return block;
    }
public:

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
        if(!node) return "";
        else if(node->type =="INT" || node->type == "TRUE" || node->type == "FALSE"){
            std::string temp = this->new_temp();
            TAC ta(TACType::ASSIGN, temp, node->value, "","");
            ctx.current_block->tacInstructions.push_back(ta);
            return temp;
        }
        
        else if(node->type == "exp DOT ident LP exp COMMA exp RP"){

            std::string firstExpThis = visit_expr(node->children.front(),ctx); //NEW Bar
            std::string temp = this->new_temp();

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

            string temp = node->children.front()->value;

            int count = 0;
            for(auto arg: node->children){
                if (count == 0){
                    count++;
                    continue;
                } 
                temp += "," + visit_expr(arg,ctx);
            }

            return temp; 
        }

        else if(node->type == "argument"){
            std::string temp = this->new_temp();
            TAC ta(TACType::ASSIGN, temp, node->children.front()->value, "","");
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
            if(node->children.front()->type =="exp DOT ident LP exp COMMA exp RP"){
                BasicBlock* newBlock = create_block(ctx.cfg);
                ctx.current_block->successors.push_back(newBlock); // Ensure correct flow
                ctx.current_block = newBlock; // Switch to the new block
                return ctx.current_block;
            }

        }


        else if(node->type =="SOMETHING ASSIGNED = TO SOMETHING"){
            Node* left = node->children.front();
            Node* right = *std::next(node->children.begin());

            string temp = visit_expr(right, ctx);

            TAC t(TACType::ASSIGN, left->value, temp, "", "");
            ctx.current_block->tacInstructions.push_back(t);

            /*
            aux = 1;
            aux2 = true;

            t1 = 1 * 2
            aux = t1

            */

            // TAC t(TACType::ASSIGN, left->value, right->value, "", new_temp());
            // ctx.current_block->tacInstructions.push_back(t);
            
            // if(right->type == "exp DOT ident LP exp COMMA exp RP"){
                
            //     BasicBlock* newBlock = create_block(ctx.cfg);
            //     ctx.current_block->successors.push_back(newBlock); // Ensure correct flow
            //     ctx.current_block = newBlock; // Switch to the new block
            //     return ctx.current_block;
            // }
        }


        return ctx.current_block;

    }   
    
    
    void traverse_generic(Node* node, BlockContext& ctx) {
        if (!node) return;

       
        else if(node->type =="SIMPLE PRINT LOL"){
            BasicBlock *res = visit_stmt(node,ctx);
            if(node->type == "exp DOT ident LP exp COMMA exp RP"){
                //ctx.cfg->addBlock(res);
                
            }
        }
        else if(node->type =="SOMETHING ASSIGNED = TO SOMETHING"){
            BasicBlock *res = visit_stmt(node,ctx);
        }

        
        //default:
        else
        for(auto child : node->children){
            //std::cout <<" left To Process: " + child->type << std::endl;
            traverse_generic(child, ctx);
        }

    }
};