# include "Node.h"
# include "symtab.h"
# include <algorithm>
# include <unordered_set>
# include "IR.h"
 /*
 My own lexicon of errors:  
 Node* id_node = *std::next(var_node->children.begin(), 2000); gives segmentation fault if the child/node does not exist.
 
 Use the identifier node's line number for DECLARATIONS (classes/methods/variables).
 and Use the node's own line number for OTHER CONSTRUCTS:(statements/expressions)

 for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child); RUN THIS TO SEE ALL CHILDS
 
 recipe:
    add symbol
    enter scope
    do something
    exit scope

maybe in future problem with scoping in class.method like 

class DuplicateIdentifiers {    
    public boolean func() {
        return true;
    }
}
in this case its: "DuplicateIdentifiers.func"
 */

 //python3 testScript.py -semantic -valid -lexical -syntax

class ASTVisitor {
private:
    SymbolTable &symtab;
    vector<tuple<int, string>> res; // line number, error message
    string curr_class_name; // Track current class name

    Node* curr_class_for_returns = nullptr;
    //Node* curr_method_for_returns = nullptr;
    bool flag = false; //used for when its okay for example i1 = ia1[1] + ia2[2]; or b1 = i1 < i2;
    Symbol* a1 = nullptr;

    vector<string> method_scope_name;
    unordered_set<string> declared_vars; // Track local declarations
    // IR:
public:
    // variables for the IR generation
    BasicBlock* current_block;
    std::unordered_map<std::string, BasicBlock*> method_entry_blocks;

    int temp_counter = 0;
    CFG cfg;
    int block_counter = 0;
    BasicBlock* create_block() {
        BasicBlock* block = new BasicBlock();
        block->label = "block_" + std::to_string(block_counter++); // Unique label
        cfg.addBlock(block); // Add to CFG
        return block;
    }
public:
    ASTVisitor(SymbolTable &st) : symtab(st), current_block(new BasicBlock()) {}

    void visit_THE_WHOLE_AST_FOR_THE_SYMTAB(Node* node){
        if (!node) return;
        
        
        if (node->type == "goal" || node->type == "classDeclarations"){
            
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);
        }
        if (node->type == "classDeclaration"){
            Node* class_name_node = node->children.front(); // identifier:DuplicateIdentifiers
            
            curr_class_name = class_name_node->value; // Set current class

            Symbol class_sym {
                class_name_node->value,
                CLASS,
                "regular class",
                class_name_node->lineno ///SDASDASDASDASD
            };
            
            symtab.add_symbol(class_sym);
            symtab.enter_scope(class_sym.name);
            // identifier:DuplicateIdentifiers, reqVarDeclaration, reqMethodDeclaration methodDeclaration:
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);  
            symtab.exit_scope();
            curr_class_name.clear(); // Reset after class processing
        }
        
        if (node->type == "var declarations"){
            
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);  
            
        }

        if (node->type == "var declaration"){
            
            handle_variable(node);
        }

        if (node->type == "methodDeclarations"){
            // look for all childs until it sees something in style with "METHODDECLARATION VARDECLARATION:"
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);
            // THEN IT WILL GO TO AN IF STATEMENT (DOWN BELOW)
        }

        if (node->type == "methodDec"){
            string type_char_check_or_NOT;
            Node* method_type = node->children.front(); // INT RETURN TYPE (HERERERERE) open it up if it needs usage

            if (method_type->type == "typechar"){
                Node* method_type_ident_because_it_has_typechar = method_type->children.front();
                type_char_check_or_NOT = method_type_ident_because_it_has_typechar->value;
            }
            else { type_char_check_or_NOT = method_type->type; }

            Node* indentifier_method = *std::next(node->children.begin()); // identifier:func

            // Add for IR
            BasicBlock* entry_block = create_block();
            method_entry_blocks[indentifier_method->value] = entry_block;
            current_block = entry_block;

            //cout << "TESTING " << indentifier_method->value << endl;
            method_scope_name.push_back(curr_class_name + "." + indentifier_method->value); // Class.method

            Symbol method_sym {
                indentifier_method->value,
                METHOD,
                type_char_check_or_NOT,
                indentifier_method->lineno
            };
            //symtab.exit_scope();
            symtab.add_symbol(method_sym);
            symtab.enter_scope(method_sym.name); // different here
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);  

            // Collect parameter types from current scope (method's scope)
            vector<string> param_types;

            for (const auto& entry : symtab.current_scope->symbols){
                if (entry.second.kind == PARAMETER){
                    //cout << entry.second.name <<" "; 
                    param_types.push_back(entry.second.name);
                }
            }

            // Update method symbol in CLASS scope with parameters
            Scope* class_scope = symtab.current_scope->parent;
            if (class_scope) {
                Symbol* method_symbol = class_scope->lookup(method_sym.name);
                if (method_symbol) {
                    method_symbol->param_types = param_types;
                }
            }

            symtab.exit_scope();

        }
        if (node->type == "methodBody"){
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child); //int param; // @error - semantic (Already Declared parameter: 'param')
        }

        if (node->type == "parameters"){
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);             
        }
        if (node->type == "parameter"){
            Node* typeNode = node->children.front();    // e.g. "INT LB RB" OR SOME CASES TYPECHAR
            Node* idNode   = node->children.back();     // e.g. "identifier:param"
            
            string type_str;
            if (typeNode->type == "typechar") {
                Node* class_node_val = typeNode->children.front();
                type_str = class_node_val->value;
            } else {
                type_str = typeNode->type;
            }

            Symbol paramSym {
                idNode->value,            // "param"
                PARAMETER,
                type_str,           // "INT LB RB" or "some_class_name"
                node->lineno
            };
            symtab.add_symbol(paramSym);

        }

        

        if (node->type == "MAIN CLASS"){
            Node* main_class_name_node = node->children.front();

            Symbol main_sym {
                main_class_name_node->value,
                CLASS,
                main_class_name_node->type,
                node->lineno
            };

            symtab.add_symbol(main_sym);
            symtab.enter_scope(main_sym.name);
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);  
            symtab.exit_scope();
        }

        
        
    }

    void visit(Node* node){ /* VISIT ALL THE NODES IN THE AST (pdf file or smthn)*/ // should this be Node* ?
        if (!node) return;
        flag = false; //reset
        
        if (node->type == "goal" || node->type == "classDeclarations"){
            for (auto child : node->children) visit(child);
        }
        if (node->type == "classDeclaration"){
            curr_class_for_returns = node;
            

            Node* class_name_node = node->children.front(); // identifier:DuplicateIdentifiers
            
            curr_class_name = class_name_node->value; // Set current class

        
            symtab.enter_scope(class_name_node->value);
            // identifier:DuplicateIdentifiers, reqVarDeclaration, reqMethodDeclaration methodDeclaration:
            for (auto child : node->children) visit(child); // visit all children of classDeclaration
            declared_vars.clear();
            symtab.exit_scope();
            curr_class_name.clear(); // Reset after class processing

        }
        if (node->type == "methodDeclarations"){ for (auto child : node->children) visit(child);}

        if (node->type == "methodDec") 
        {   
            
            Node* first_type_of_method = node->children.front();

            Node* methodDec_return_node_but_not_type = *std::next(node->children.begin(), 4); //RETURN
            Node* method_return_node_type = methodDec_return_node_but_not_type->children.front(); // INT:0

            
            Node* indentifier_method = *std::next(node->children.begin()); // identifier:func
           
            symtab.enter_scope(indentifier_method->value); // different here

            if (method_return_node_type->type != first_type_of_method->type){
                
                
                if (method_return_node_type->type == "identifier"){
                    Symbol* found = symtab.lookup(method_return_node_type->value);
                    if (first_type_of_method->type == "typechar"){
                        Node* what_is_this_typechar = first_type_of_method->children.front(); //identifier:A 
                        if (found->type != what_is_this_typechar->value){
                            res.push_back(std::make_tuple(first_type_of_method->lineno, "semantic (type mismatch)"));
                            symtab.error_count++;
                        }
                    }
                    else {
                        
                        //cout << symtab.writeAllSymbols();

                        // THIS IS // @error - semantic (type mismatch)
                        // BUT IN InvalidReturn ITS // @error - semantic (invalid return type) LIKE WHAT
                        if (found){
                            if (found->type != first_type_of_method->type){
                                res.push_back(std::make_tuple(first_type_of_method->lineno, "semantic (type mismatch)"));
                                symtab.error_count++;
                            }
                        }
                    }
                }
            }
            if (methodDec_return_node_but_not_type->type == "RETURN"){
                Node* check_if_its_an_exp = methodDec_return_node_but_not_type->children.front();

                if (check_if_its_an_exp->type == "exp DOT ident LP exp COMMA exp RP"){
                    Node* get_var_name_for_return = *std::next(check_if_its_an_exp->children.begin());
                    Node* get_arguments = *std::next(check_if_its_an_exp->children.begin(), 2);
                    //cout <<"THIS SHOULD BE zxFu " <<  get_var_name_for_return->value << endl;
                    Symbol* get_sym_var_name_for_return = symtab.lookup(get_var_name_for_return->value);

                    //go into Element to find InInt.
                    string found_class = extractClass(method_scope_name, get_var_name_for_return->value);
                    int arg_count = 0;
                    if (get_arguments->type == "empty argument"){ // no arguments given in the function.
                        arg_count = 0;
                    }
                    else if (get_arguments->type == "argument_list"){ //there is atleast 1 argument
                        arg_count++;
                    }
                    
                    Symbol* findInInt = getSymbolForFunction_For_parameters(get_var_name_for_return->value);

                    if (findInInt->param_types.size() != arg_count){
                        // @error - semantic (missing parameter)
                        res.push_back(std::make_tuple(get_var_name_for_return->lineno, "semantic (missing parameter)"));
                        symtab.error_count++;
                    }
                    
                    
                    if (get_sym_var_name_for_return){
                        if (get_sym_var_name_for_return->type != first_type_of_method->type){
                            res.push_back(std::make_tuple(first_type_of_method->lineno, "semantic (invalid return type)"));
                            symtab.error_count++;
                        }
                    }
                }
                else if (check_if_its_an_exp->type == "expression LEFT_BRACKET expression RIGHT_BRACKET"){
                    Node* check_if_its_an_exp2 = *std::next(check_if_its_an_exp->children.begin());

                    Node* get_var_for_THIS = *std::next(check_if_its_an_exp2->children.begin());//yzFunc

                    Symbol* find_return_THIS = symtab.lookup(get_var_for_THIS->value);

                    if (find_return_THIS){
                        if (find_return_THIS->type != first_type_of_method->type){
                            res.push_back(std::make_tuple(get_var_for_THIS->lineno, "semantic (invalid type in the return statement)"));
                            symtab.error_count++;
                        }
                    }
                }
            }
            
            


            for (auto child : node->children) visit(child);
            symtab.exit_scope();

        }
        if (node->type == "methodBody") for (auto child : node->children) visit(child);

        if (node->type == "statement"){
            // here we visit SOMETHING [ASSIGNED] = TO SOMETHING, IF LP expression RP statement ELSE statement    etc..
            for (auto child : node->children) visit(child); 
        } 

        if (node->type == "SOMETHING ASSIGNED = TO SOMETHING"){
            Node* either_an_ident_or_exp_DOT_ident = *std::next(node->children.begin());
            Node* left_assign = node->children.front();
            for (auto child : node->children) visit(child);
            
            //Node* either_an_ident_or_exp_DOT_ident = *std::next(node->children.begin()); // it can even be an operator like ADD OR EVEN AN NEW LIKE WHAAAT
            // @error - semantic ('e' does not exist in the current scope)
            Symbol* found_the_non_existent = symtab.lookup(left_assign->value); // IMPORTANT


            if(getNodeReturnType(left_assign)!= getNodeReturnType(either_an_ident_or_exp_DOT_ident)){

    
                res.push_back(std::make_tuple(node->lineno, "semantic (invalid right hand side)"));
                symtab.error_count++;

            }


            if (either_an_ident_or_exp_DOT_ident->type == "NEW INT LEFT_BRACKET expression RIGHT_BRACKET"){
                Node* ident = either_an_ident_or_exp_DOT_ident->children.front();
                
                if (ident->type == "TRUE" || ident->type == "FALSE"){ //FALSE doesnt exist in the tests but who knows?
                    // @error - semantic ('true': is of wrong type)
                    string error_msg = "semantic ('" + ident->type + "': is of wrong type)";
                    res.push_back(std::make_tuple(node->lineno, error_msg));
                    symtab.error_count++;
                }

                if (found_the_non_existent->type != ident->type &&
                    found_the_non_existent->type != "INT LB RB"){
                    // @error - semantic ('x' and expression new int[1] new are of different types)
                    string error_msg = "semantic ('" + found_the_non_existent->name + "' and expression new int[1] new are of different types)";
                    res.push_back(std::make_tuple(node->lineno, error_msg));
                    symtab.error_count++;
                }
                
            }

            if (either_an_ident_or_exp_DOT_ident->type == "expression DOT LENGTH"){
                Node* get_exp = either_an_ident_or_exp_DOT_ident->children.front();

                Symbol* get_sym_exp = symtab.lookup(get_exp->value); // identifier:e

                if (get_sym_exp){
                    //cout << "name: "<<get_sym_exp->name<<" "<< get_sym_exp->type << endl;
                    if (get_sym_exp->type == "INT LB RB" && found_the_non_existent->type == "INT"){
                        flag = true; //its okay because a = e.length; is valid since a is an INT and e is an INT LB RB
                    }

                    if (get_sym_exp->type != found_the_non_existent->type && !flag){
                        // @error - semantic ('x' and expression 'e.length' are of different types)
                        string error_msg = "semantic ('" + found_the_non_existent->name + "' and expression '" +\
                        get_sym_exp->name + ".length' are of different types)";
                        res.push_back(std::make_tuple(node->lineno, error_msg));
                        symtab.error_count++;
                    }
                }

                if (get_exp->type == "expression LEFT_BRACKET expression RIGHT_BRACKET"){
                    Node* get_var_name_arr = get_exp->children.front();
                    // @error - semantic ('e' is undefined or wrong)
                    string error_msg = "semantic ('" + get_var_name_arr->value + "' is undefined or wrong)";
                    res.push_back(std::make_tuple(node->lineno, error_msg));
                    symtab.error_count++;
                }
            }

            else if (either_an_ident_or_exp_DOT_ident->type == "NEW identifier LP RP"){
                Node* find_var_in_NEW = either_an_ident_or_exp_DOT_ident->children.front();
                Symbol* find_the_NEW_var = symtab.lookup(find_var_in_NEW->value);

                if (!find_the_NEW_var){
                    string error_msg = "semantic ('" + find_var_in_NEW->value + "' is not a valid Class)";
                    res.push_back(std::make_tuple(node->lineno, error_msg));
                    symtab.error_count++;
                }
                else if (find_the_NEW_var){
                    //c = new Element();// @error - semantic ('c' and expression 'Element' new are of different types)
                    if (found_the_non_existent->type != find_the_NEW_var->name){ //BECAUSE THIS IS A CLASS WE USE NAME NOT TYPE (CLASS NAME IS THE TYPE)
                        string error_msg = "semantic ('" + found_the_non_existent->name + "'and expression '" +\
                        find_the_NEW_var->name + "' new are of different types)";
                        res.push_back(std::make_tuple(node->lineno, error_msg));
                        symtab.error_count++;
                    }
                }
            }

            else if (either_an_ident_or_exp_DOT_ident->type == "EXCLAMATION_MARK expression"){
                Node* var_in_exclamation_mark = either_an_ident_or_exp_DOT_ident->children.front(); 

                // find it
                Symbol* var_sym_in_exclamation_mark = symtab.lookup(var_in_exclamation_mark->value);

                if (var_sym_in_exclamation_mark){
                    if (var_sym_in_exclamation_mark->type != "BOOLEAN"){
                        res.push_back(std::make_tuple(node->lineno, "semantic"));
                        symtab.error_count++;
                    }
                }
            }

            if (either_an_ident_or_exp_DOT_ident->type == "AddExpression"){
                
                //then get the two types yalla
                Node* left_after_equal = either_an_ident_or_exp_DOT_ident->children.front(); //identifier:i1
                Node* right_after_equal = *std::next(either_an_ident_or_exp_DOT_ident->children.begin()); //identifier:b1
                //does it even exist?
                Symbol* left_sym_after_equal = symtab.lookup(left_after_equal->value);
                Symbol* right_sym_after_equal =  symtab.lookup(right_after_equal->value);


                if (found_the_non_existent->type == "BOOLEAN"){//b1 = b1 + b1;// @error - semantic
                    if (left_sym_after_equal->type == "BOOLEAN" && right_sym_after_equal->type == "BOOLEAN"){
                        res.push_back(std::make_tuple(node->lineno, "semantic"));
                        symtab.error_count++;
                    }
                }
               

                if (left_sym_after_equal && right_sym_after_equal) { //it does indeed exist.
                    if (found_the_non_existent->type != left_sym_after_equal->type){ 
                        //i1 = b1 + b1;// @error - semantic ('b1' is of wrong type)
                        string error_msg = "semantic ('" + left_sym_after_equal->name +\
                        "' is of wrong type)";
                        res.push_back(std::make_tuple(node->lineno, error_msg));
                        symtab.error_count++;
                    }
                    else if (found_the_non_existent->type != right_sym_after_equal->type){
                        // now the same as before but for right side.
                        string error_msg = "semantic ('" + right_sym_after_equal->name +\
                        "' is of wrong type)";
                        res.push_back(std::make_tuple(node->lineno, error_msg));
                        symtab.error_count++;
                    }    
                    else if (found_the_non_existent->type == "INT LB RB"){//ia1 = ia1 + ia2;// @error - semantic
                        res.push_back(std::make_tuple(node->lineno, "semantic"));
                        symtab.error_count++;
                    }       
                }
                if (left_after_equal->type == "expression LEFT_BRACKET expression RIGHT_BRACKET" &&
                    right_after_equal->type == "expression LEFT_BRACKET expression RIGHT_BRACKET"){
                    
                    Node* vararr_of_left = left_after_equal->children.front();
                    Node* vararr_of_right = right_after_equal->children.front();
                    
                    Symbol* left_sym = symtab.lookup(vararr_of_left->value);
                    Symbol* right_sym = symtab.lookup(vararr_of_right->value);

                    if (left_sym && right_sym){
                        if (left_sym->type == right_sym->type){ //THEY BOTH ARE SAME TYPE arrays.
                            if (left_sym->type == "INT LB RB" && right_sym->type == "INT LB RB"){
                                if (found_the_non_existent->type == "INT"){
                                    flag = true; // ITS OKAY THEY BOTH ARRAYS ARE INT AND THE LEFT ASSIGN VAR IS ALSO AN INT.
                                }
                            }
                        }
                    }
                }
                if (right_after_equal->type == "expression LEFT_BRACKET expression RIGHT_BRACKET"){
                    //i1 = ia1 + ia1[0];// @error - semantic ('ia1' is of wrong type)

                    //now get the two childs:
                    Node* var_name_for_arr = right_after_equal->children.front(); //identifier:ia1
                    Node* get_inside_brackets = *std::next(right_after_equal->children.begin()); // INT:0
                    //cout << var_name_for_arr->value << endl;
                    //cout << get_inside_brackets->type << endl;
                    Symbol* find_the_variable_as_arr = symtab.lookup(var_name_for_arr->value);
                    if (find_the_variable_as_arr){
                        // its okay if they both are INT [] and the left  assign (found_the_non_existent) is an int.
                        
                        if (found_the_non_existent->type != find_the_variable_as_arr->type && !flag){
                            
                            string error_msg = "semantic ('" + find_the_variable_as_arr->name +\
                            "' is of wrong type)";
                            res.push_back(std::make_tuple(node->lineno, error_msg));
                            symtab.error_count++;
                        }
                    }
                }
                
            }

            else if (either_an_ident_or_exp_DOT_ident->type == "AND"){
                //get the two childs:
                Node* left_in_AND = either_an_ident_or_exp_DOT_ident->children.front();
                Node* right_in_AND = *std::next(either_an_ident_or_exp_DOT_ident->children.begin());

                Symbol* left_sym_in_AND = symtab.lookup(left_in_AND->value);
                Symbol* right_sym_in_AND = symtab.lookup(right_in_AND->value);

                if (left_sym_in_AND->type != right_sym_in_AND->type && !flag){
                    res.push_back(std::make_tuple(node->lineno, "semantic"));
                    symtab.error_count++;
                }

                if (left_sym_in_AND && right_sym_in_AND){
                    
                    if (found_the_non_existent->type != left_sym_in_AND->type && found_the_non_existent->type != right_sym_in_AND->type){
                        //@error - semantic ('c1' is of wrong type, 'i1' and expression 'c1 && c1' are of different types)
                        string error_msg = "semantic ('" + left_sym_in_AND->name +\
                        "' is of wrong type, '" + found_the_non_existent->name + "' and expression '" +\
                        left_sym_in_AND->name + " && " + right_sym_in_AND->name + "' are of different types)";
                        res.push_back(std::make_tuple(node->lineno, error_msg));
                        symtab.error_count++;
                    }
                    else if (found_the_non_existent->type == "INT LB RB"){//ia1 = ia1 + ia2;// @error - semantic
                        res.push_back(std::make_tuple(node->lineno, "semantic"));
                        symtab.error_count++;
                    }  
                }
            }

            else if (either_an_ident_or_exp_DOT_ident->type == "OR" ||
                either_an_ident_or_exp_DOT_ident->type == "LESS_THAN" ||
                either_an_ident_or_exp_DOT_ident->type == "EQUAL"){
                
                Node* left_of_op = either_an_ident_or_exp_DOT_ident->children.front();
                Node* right_of_op = *std::next(either_an_ident_or_exp_DOT_ident->children.begin());

                Symbol* left_sym_of_op = symtab.lookup(left_of_op->value);
                Symbol* right_sym_of_op = symtab.lookup(right_of_op->value);
                
                //sometimes its okay.
                

                if (left_sym_of_op && right_sym_of_op){

                    if (left_sym_of_op->type == right_sym_of_op->type){
                        if (left_sym_of_op->type == "BOOLEAN" &&
                            right_sym_of_op->type == "BOOLEAN" &&
                            found_the_non_existent->type == "BOOLEAN"){
                            res.push_back(std::make_tuple(node->lineno, "semantic"));
                            symtab.error_count++;
                        }
                        if (found_the_non_existent->type == "BOOLEAN" &&
                            right_sym_of_op->type != "BOOLEAN" &&
                            left_sym_of_op->type != "BOOLEAN"){
                            flag = true; //THIS IS OKAY b1 = i1 < i2;
                        }
                    }
                    
                    if (left_sym_of_op->type != right_sym_of_op->type && !flag){
                        res.push_back(std::make_tuple(node->lineno, "semantic"));
                        symtab.error_count++;
                    }

                    if (found_the_non_existent->type != left_sym_of_op->type &&
                        found_the_non_existent->type != right_sym_of_op->type && !flag){
                            res.push_back(std::make_tuple(node->lineno, "semantic"));
                            symtab.error_count++;
                        }
                }

            }
            //cout<< "SOMETHING " << found_the_non_existent<< endl;
            // kolla i klassen scope
            if (found_the_non_existent){ // variable exists later or sooner.
                //cout << "is this a d ?? " << found_the_non_existent->name << endl;
                
                    //Scope* check_if_var_in_scope = symtab.get_class_scope(found_the_non_existent->name);
                    // cout << "LASDASDASDASD" << endl;
                    // for (auto i : declared_vars){
                        
                    //     cout << i << " ";
                    // }
                    if (!declared_vars.count(left_assign->value)){
                        
                        string error_message = "semantic ('" + left_assign->value + "' is not defined yet)";
                        res.push_back(std::make_tuple(node->lineno, error_message));
                        symtab.error_count++;
                    }
                
                
            }
            
            
            if (either_an_ident_or_exp_DOT_ident->type == "identifier" && found_the_non_existent){


                Symbol* right_assign = symtab.lookup(either_an_ident_or_exp_DOT_ident->value);

                



                // @error - semantic ('a' and expression 'b' are of different types)
                if (found_the_non_existent->type != right_assign->type){
                    string error_message = "semantic ('" + found_the_non_existent->name + "' and expression '" \
                    + right_assign->name + " are of different types)";
                    res.push_back(std::make_tuple(node->lineno, error_message));
                    symtab.error_count++;
                }
                // else if (!declared_vars.count(either_an_ident_or_exp_DOT_ident->value)){
                //     string error_message = "semantic ('" + left_assign->value + "' is not defined yet)";
                //     res.push_back(std::make_tuple(node->lineno, error_message));
                //     symtab.error_count++;
                // }
            }

            if (!found_the_non_existent ){ // @error - semantic ('e' does not exist in the current scope) 
                // Scope* class_scoping = symtab.get_class_scope(found_the_non_existent->type); //Get class scope (e.g., "classdata")
                // if (class_scoping){
                //     string error_message = "semantic ('" + left_assign->value + "' does not exist in the current scope)";
                //     res.push_back(std::make_tuple(node->lineno, error_message));
                //     symtab.error_count++;
                // }
                string error_message = "semantic ('" + left_assign->value + "' does not exist in the current scope)";
                res.push_back(std::make_tuple(node->lineno, error_message));
                symtab.error_count++;
                //cout << "AAAAAAAAAAAAA"<<endl;
            }

            // om d är en identifier (classdata) så går vi in i d. Sen kollar vi om d har funktionen yfunc.
            // kolla return type of yfunc jämför (if) om a = d.func om a är valid type boolean
            
            //PROBABLY CHANGE LATER:
            if (either_an_ident_or_exp_DOT_ident->type == "exp DOT ident LP exp COMMA exp RP"){
                Node* method_name_node = *std::next(either_an_ident_or_exp_DOT_ident->children.begin()); //yFunc
                Node* obj_node = either_an_ident_or_exp_DOT_ident->children.front(); //
                Node* argument_list_or_argument = *std::next(either_an_ident_or_exp_DOT_ident->children.begin(), 2);
                // try to see if even the function (.zzFunc) even exists.
                Symbol* does_this_exist = symtab.lookup(method_name_node->value);
                
                if (argument_list_or_argument->type == "argument_list"){
                    // this one below can be 3 things (1. exp_dot_ident 2. argument 3. INT)
                    Node* check_if_argument_or_DOT_ident = argument_list_or_argument->children.front();
                    if (check_if_argument_or_DOT_ident->type == "identifier"){
                        Node* check_if_argument_exist = check_if_argument_or_DOT_ident->children.front();
                        if (check_if_argument_exist){ // loop through all arguments.
                            for (auto child : node->children) visit(child);
                        }
                    }
                }
                else if (argument_list_or_argument->type == "argument"){
                    Node* check_if_its_an_THIS = either_an_ident_or_exp_DOT_ident->children.front();
                    Node* get_the_name = *std::next(either_an_ident_or_exp_DOT_ident->children.begin());
                    cout<<"YWDAWDAWD "<<check_if_its_an_THIS->type<<endl;
                    if (check_if_its_an_THIS->type == "THIS"){
                        Symbol* check_its_type = symtab.lookup(get_the_name->value);
                        cout << "FOUND YUEA " << check_its_type<<endl;
                    }
                }

                // KIND OF CORRECT BUT MAY NEEDS CHANGINGS.
                if (obj_node->type == "exp DOT ident LP exp COMMA exp RP"){
                    
                    Node* check_this = obj_node->children.front(); // THIS
                    Node* ident_name_to_find = *std::next(obj_node->children.begin()); // identifier:extract 
                    // Node* argument_list_or_argument = *std::next(obj_node->children.begin(), 2);
                    // cout << argument_list_or_argument->type<<endl;
                    // if (check_this->type == "exp DOT ident LP exp COMMA exp RP" ){

                    // }

                    if (check_this->type == "THIS"){
                        Symbol* method_sym = symtab.lookup(ident_name_to_find->value);
    
                        if (method_sym && method_sym->kind == METHOD) {
                            // 1. Get class scope (parent of current method scope)
                            Scope* class_scope = symtab.current_scope->parent;
                            
                            // 2. Find method's own scope
                            Scope* method_scope = nullptr;
                            for (Scope* child : class_scope->children) {
                                if (child->name == method_sym->name) {
                                    method_scope = child;
                                    break;
                                }
                            }
                            
                            // 3. Extract parameter types
                            vector<string> param_types;
                            if (method_scope) {
                                for (const auto& entry : method_scope->symbols) {
                                    if (entry.second.kind == PARAMETER) {
                                        param_types.push_back(entry.second.type);
                                    }
                                }
                            }
                            
                            // Now param_types contains ["int"] for a1()
                            // Use these to validate arguments
                        }
                    }
                }

                
                


                // THIS IS GOOD: (other errors.)
                if (obj_node->type == "THIS"){
                    Symbol* obj_sym = symtab.lookup(method_name_node->value);
                    //cout << "found it name " << obj_sym->name <<" type "<<obj_sym->type<< endl;
                    if (obj_sym){
                        if (obj_sym->type != found_the_non_existent->type){
                            res.push_back(std::make_tuple(node->lineno, "semantic (type mismatch)"));
                            symtab.error_count++;
                        }
                    }
                    else {
                        //// @error - semantic ('zFunc' does not exist)
                        string error_msg = "semantic ('" + method_name_node->value + "' does not exist)";
                        res.push_back(std::make_tuple(node->lineno, error_msg));
                        symtab.error_count++;
                    }
                }
                else if (obj_node->type == "identifier") {
                    Symbol* obj_sym = symtab.lookup(obj_node->value); // check type of the first node (type of "d")
                    //cout << symtab.writeAllSymbols() << endl;
                    if (obj_sym) {
                        string class_name = obj_sym->type;
                        Scope* class_scope = symtab.get_class_scope(class_name); //Get class scope (e.g., "classdata")
                        
                        //cout << "just look here: " << method_name_node->value <<" and here "<<class_scope->name<< endl;
                        if (class_scope) {
                            //cout << "class scope name " << class_scope->name<<endl; 
                            // Look up the method in the class's scope
                            Symbol* method_sym = class_scope->lookup(method_name_node->value);
                            //cout << "NEWEWEWEWE " << method_sym->name <<" AADNADNADNADNADNANDNA " << method_sym->type<< endl;
                            if (method_sym) {
                                // Check return type compatibility, etc.
                                if (found_the_non_existent->type != method_sym->type){
                                    string error_msg = "semantic ('" + found_the_non_existent->name + "' and expression '" +\
                                    obj_node->value + "." + method_name_node->value + "()' are of different types)";
                                    
                                    // ('a' and expression 'd.yFunc()' are of different types)
                                    res.push_back(std::make_tuple(node->lineno, error_msg));
                                    symtab.error_count++;
                                }  
                            }
                            else {
                                string error_msg = "semantic ('" + method_name_node->value + "' does not exist)";
                                res.push_back(std::make_tuple(node->lineno, error_msg));
                                //semantic ('zzFunc' does not exist)
                                symtab.error_count++;
                            } 
                        }
                    }
                } 
            }  
        }



        if (node->type == "SOMETHING [ASSIGNED] = TO SOMETHING"){
            Node* identifier_arr = node->children.front(); // identifier:num_aux
            Node* inside_arr_brackets = *std::next(node->children.begin()); // FALSE
            Node* assigned_arr_to = *std::next(node->children.begin(), 2); // INT:2

            if (inside_arr_brackets->type != "INT"){
                res.push_back(std::make_tuple(node->lineno, "semantic (invalid type of array index)"));
                symtab.error_count++;
            }

            if (assigned_arr_to->type == "expression DOT LENGTH"){
                Node* type_dot_length = assigned_arr_to->children.front(); // identifier:num
                //cout << "value or smthn: " << type_dot_length->value << endl;
                
                Symbol* test = symtab.lookup(type_dot_length->value);
                //if(test) cout << "FOUND IT: \n type" << test->type  << "\nname" << test->name<< endl;
                if (test){
                    if("INT LB RB" != test->type){
                        res.push_back(std::make_tuple(node->lineno, "semantic (member .length is used incorrectly)"));
                        symtab.error_count++;
                    }
                }
                //what type is num ? if its not INT LB RB then its NOT okay.
                //cout << symtab.writeAllSymbols();
                //cout << "dadasdasdasdasdasdasd " << symtab.lookup(type_dot_length->value);
                type_dot_length->value; //num

            }

            // trying to use int as an array.
            if (identifier_arr->type == "identifier"){
                Symbol* found = symtab.lookup(identifier_arr->value);
                if (found->type != "INT LB RB"){
                    res.push_back(std::make_tuple(node->lineno, "semantic (trying to use int as int array)"));
                    symtab.error_count++;
                }
            }
            

        }

        if (node->type == "IF LP expression RP statement ELSE statement") for (auto child : node->children) visit(child);
        if (node->type == "LESS_THAN") for (auto child : node->children) visit(child);
        if (node->type == "expression LEFT_BRACKET expression RIGHT_BRACKET"){
            for (auto child : node->children) visit(child);
            
            Node* check_if_its_a_ident_or_exp_DOT_ident = *std::next(node->children.begin());

            if (check_if_its_a_ident_or_exp_DOT_ident->type == "exp DOT ident LP exp COMMA exp RP"){
                Node* this_or_new = check_if_its_a_ident_or_exp_DOT_ident->children.front(); // THIS
                Node* identifier_for_this = *std::next(check_if_its_a_ident_or_exp_DOT_ident->children.begin()); // identifier:a2
                
                if (this_or_new->type == "THIS"){
                    //find return statement of this (THIS IS VALID IF THIS.a2 IF a2 IS AN INT)
    
                    // for example if (num_aux[this.a2()] < 1) (find return for a2)
                    Node* return_this_method = find_declared_method_type(curr_class_for_returns, identifier_for_this->value);
    
                    // look if the node before is an method then its also ok.
                    if (return_this_method->type != "INT"){
                        res.push_back(std::make_tuple(node->lineno, "semantic (invalid type of array index)"));
                        symtab.error_count++;
                    }
                }
            }

            
        } 
        
        
        if (node->type == "var declarations") for (auto child : node->children) visit(child);

        if (node->type == "var declaration"){
            Node* check_if_type_or_type = node->children.front(); // typechar or INT etc..
            Node* check_name = *std::next(node->children.begin());
            if (check_if_type_or_type->type == "typechar"){
                Node* check_class_name = check_if_type_or_type->children.front();
                //declared_vars.insert(check_class_name->value);
                Symbol* trytofindme = symtab.lookup(check_class_name->value);
                if (!trytofindme){
                    //// @error - semantic ('classthatdoesntExist' is undefined)
                    string error_msg = "semantic ('" + check_class_name->value + "' is undefined)";
                    res.push_back(std::make_tuple(node->lineno, error_msg));
                    symtab.error_count++;
                }
                
            }
            
            declared_vars.insert(check_name->value);
            
        
        }
        if (node->type == "parameters"){
            for (auto child : node->children) visit(child);
        }
        if (node->type == "parameter"){
            Node* name_of_parameter = *std::next(node->children.begin());
            declared_vars.insert(name_of_parameter->value);
        }
        
        
        if (node->type == "functionCall"){ // check SOMETHING ASSIGNED TO SOMETHING.

            Node* func_name = *std::next(node->children.begin());//identifier:a1
            Node* is_this = node->children.front(); // THIS

            Node* argument_lists = *std::next(node->children.begin(), 2); // argument_list

            if (is_this->type == "THIS"){

                // find a3 because its deep down in the tree. And specify which node it belongs to.
                string findA3 = findTypeUsingSymbolReq(node, "exp DOT ident LP exp COMMA exp RP");
                string findSpecial = findTypeUsingSymbolReq(argument_lists, "DOT identifier LP arguments RP");

                
                //find both a1 and a3 in the symbol table, since a1 is already at this subtree we dont need to go deep down.
                Symbol* findA1 = getSymbolForFunction_For_parameters(func_name->value);
                Symbol* findA3sym = getSymbolForFunction_For_parameters(findA3);
                
                
                Symbol* findSpecialSym = getSymbolForFunction_For_parameters(findSpecial);
                if (findSpecialSym){
                    //cout << "DSDASDSAD " << findSpecialSym->type << node->lineno << endl;
                    for (int i=0; i<findA1->param_types.size(); i++){
                        Symbol* find_type_of_arguments = getSymbolForMethod_For_parameters(findA1->type, findA1->name, findA1->param_types[i]);
                        if (find_type_of_arguments->type != findSpecialSym->type){
                            // @error - semantic (invalid method parameter for 'a1')
                            string error_msg = "semantic (invalid method parameter for '" + findA1->name + "')";
                            res.push_back(std::make_tuple(node->lineno, error_msg));
                            symtab.error_count++;
                        }
                    }
                    
                }
                

                


                if (findA3sym){
                    // if (findA1->param_types.size() == findA3sym->param_types.size()){
                    //     res.push_back(std::make_tuple(node->lineno, "semantic (invalid number of parameters)"));
                    //     symtab.error_count++;
                    // }
                    // else if (findA1->param_types.size() != findA3sym->param_types.size()){
                    //     res.push_back(std::make_tuple(node->lineno, "semantic (invalid type of argument)"));
                    //     symtab.error_count++;
                    // }

                    // we now the arguments thats in findA1 but what type are these arguments we dont know.
                    // so whats the type of the arguments in findA1->param_types
                    

                    for (int i=0; i<findA1->param_types.size(); i++){
                        Symbol* find_type_of_arguments = getSymbolForMethod_For_parameters(findA1->type, findA1->name, findA1->param_types[i]);
                        if (find_type_of_arguments->type != findA3sym->type){
                            string error_msg = "semantic (invalid parameter method '" + findA1->name + "')";
                            res.push_back(std::make_tuple(node->lineno, error_msg));
                            symtab.error_count++;
                        }
                        else if (find_type_of_arguments->type != findSpecialSym->type){
                            cout << "FOUND IT " << find_type_of_arguments->type << node->lineno << endl;
                        }
                    }
                }                    
            }
        }
    
        
        if (node->type == "exp DOT ident LP exp COMMA exp RP"){
            Node* func_that_has_the_parameters = *std::next(node->children.begin()); // identifier:a5
            Node* check_argument_list = *std::next(node->children.begin(), 2); // argument_list
            Node* check_this2 = node->children.front(); // THIS
            //Symbol* getA5 = getSymbolForFunction_For_parameters(func_that_has_the_parameters->value);
            //Symbol* getA5 = getSymbolForMethod_For_parameters(curr_class_name, func_that_has_the_parameters->value, "a5");
            vector<string> get_curr_params_types; // a = this.a5(b, a, ia, new InvalidNestedMethodCalls()); current parameters types.
            vector<string> getA5_params_types; // a5 parameters types.
            int counter = 1;

            if (check_argument_list->type == "empty argument"){
                counter = 0;
            }
            
            

            if (check_argument_list->type == "argument_list" || check_argument_list->type == "argument"){
                Node* first_arg = check_argument_list->children.front();
                Symbol *curr_param_for_A5 = symtab.lookup(first_arg->value);
                
                if (curr_param_for_A5){
                    get_curr_params_types.push_back(curr_param_for_A5->type);
                }

                for (auto child : check_argument_list->children){
                    if (child->type == "argument") {
                        counter++;
                        Node* get_next_arg = child->children.front();
                        string var_name;
                        if (get_next_arg->type == "NEW identifier LP RP"){
                            Node* addThis = get_next_arg->children.front();
                            var_name = addThis->value;
                        }
                        else if (get_next_arg->type == "exp DOT ident LP exp COMMA exp RP"){
                            Node* addThis = *std::next(get_next_arg->children.begin());
                            var_name = addThis->value;
                        }
                        else if (get_next_arg->type == "identifier"){
                            var_name = get_next_arg->value;
                        }
                        
                        
                        Symbol* get_next_arg_sym = symtab.lookup(var_name);
                        
                        if (get_next_arg_sym){
                            //cout << get_next_arg_sym->name << " " << get_next_arg_sym->type << endl;
                            if (get_next_arg_sym->type == "identifier"){
                                get_curr_params_types.push_back(get_next_arg_sym->name);
                            }
                            else {
                                get_curr_params_types.push_back(get_next_arg_sym->type);
                            }
                        }
                        
                    }
                }

                /*
                // @error - semantic (invalid number of parameters)
                if (counter != getA5->param_types.size()){
                    string error_msg = "semantic (invalid number of parameters)";
                    res.push_back(std::make_tuple(node->lineno, error_msg));
                    symtab.error_count++;
                }
                else 
                */
            }
            // Symbol* getClassFor = symtab.lookup(check_this2->value);
            // if (getClassFor){
            //     //cout << getClassFor->name << endl;
            //     Scope* class_scope = symtab.get_class_scope(getClassFor->name);
            //     if (class_scope){
            //         Symbol* A5 = class_scope->lookup(func_that_has_the_parameters->value);
            //         if (A5){
            //             cout << "DNWAOIDNAWOIDNAWIODNAWOIDNAWIOD"<<endl;
            //         }
            //     }
            // }
            //cout << "GET CLASS FOR " << getClassFor->name << endl;
            //Scope* class_scope = symtab.get_class_scope(getClassFor->name);


            Symbol* A5 = getSymbolForFunction_For_parameters(func_that_has_the_parameters->value);
            //Symbol* A5 = symtab.lookup(func_that_has_the_parameters->value);
            //Symbol* checkReturn = symtab.lookup(check_this2->value);
            //Symbol* checkReturn2 = getSymbolForFunction_For_parameters(func_that_has_the_parameters->value) ;
            //Scope* class_scope = symtab.get_class_scope(checkReturn->type);

            // if (class_scope){
            //     Symbol* A5 = class_scope->lookup(func_that_has_the_parameters->value);
            //     ///Scope* method_scope = 
            //     for (int i=0; i<A5->param_types.size(); i++){
            //         Symbol* getA5_params = getSymbolForMethod_For_parameters(checkReturn->type, func_that_has_the_parameters->value, A5->param_types[i]);
            //         if (getA5_params){
            //             //cout << A5->param_types[i] << " " << getA5_params->type << endl;
            //             getA5_params_types.push_back(getA5_params->type);
            //         }
                    
            //     }
            //     cout << endl;
            //     // go into checkReturn->type (Element) and find the function that has the parameters.
            //     // func_that_has_the_parameters->value = Init

            //     //Symbol* A5 = getSymbolForMethod_For_parameters(checkReturn->type, func_that_has_the_parameters->value, "a5");
            //     // Scope* class_scope = symtab.get_class_scope(checkReturn->type);
            //     // if (class_scope){
            //     //     //cout << "KKKKKKKKKKKKKKKKKKKKKK"<<endl;
            //     //     A5 = class_scope->lookup(func_that_has_the_parameters->value);
            //     // }
            // }
            if (A5){
                //cout << "FOUND IT " << A5->name << endl;
                for (int i=0; i<A5->param_types.size(); i++){
                    //Symbol* getA5_params = symtab.lookup(A5->param_types[i]);
                    //cout << A5->param_types[i] << endl;
                    //cout << curr_class_name << " " << A5->name << " " << A5->param_types[i] << endl;
                    Symbol* getTypeOfParamater = getSymbolForMethod_For_parameters(curr_class_name, A5->name, A5->param_types[i]);
                    // //cout << "GET TYPE OF PARAMETER " << getTypeOfParamater << endl;
                    if (getTypeOfParamater){
                        //cout << getTypeOfParamater->type << endl;
                        getA5_params_types.push_back(getTypeOfParamater->type);
                    }
                    //getA5_params_types.push_back(A5->param_types[i]);
                }
                
            }
            
            
            // if (counter != getA5_params_types.size()){
            //     cout << "counter " << counter << " getA5_params_types.size() " << getA5_params_types.size() << " "<<node->lineno << endl;
            //     string error_msg = "semantic (invalid number of parameters)";
            //     res.push_back(std::make_tuple(node->lineno, error_msg));
            //     symtab.error_count++;
            // }

            // else 
            if (counter == get_curr_params_types.size()){
                for (int i=0; i<counter; i++){
                    //cout << get_curr_params_types[i] << " " << getA5_params_types[i] << endl;
                    if (get_curr_params_types[i] != getA5_params_types[i]){
                        string error_msg = "semantic (invalid type of argument)";
                        res.push_back(std::make_tuple(node->lineno, error_msg));
                        symtab.error_count++;
                    }
                }
            }
            // for (int i = 0; i<getA5_params_types.size(); i++){
            //     cout << getA5_params_types[i] << " ";
            // }
            // cout << endl;
        
            
        
        }
    }

    std::string visit_for_IR(Node* node) {
        if (!node) return "";

        


        if (node->type == "SOMETHING ASSIGNED = TO SOMETHING") {
            Node* lhs = node->children.front();
            Node* rhs = *std::next(node->children.begin());

            std::string rhs_temp = visit_for_IR(rhs);
            current_block->tacInstruction.push_back(TAC{TACType::ASSIGN, lhs->value, rhs_temp, ""});
            return lhs->value;
        }
        else if (node->type == "AddExpression" || node->type == "SubExpression") {
            Node* left = node->children.front();
            Node* right = *std::next(node->children.begin());
            
            std::string left_temp = visit_for_IR(left);
            std::string right_temp = visit_for_IR(right);
            std::string result_temp = new_temp();
            
            current_block->tacInstruction.push_back(TAC{TACType::BIN_OP, result_temp, left_temp, right_temp, node->value});
            return result_temp;
        }
        else if (node->type == "WHILE LP expression RP statement") {
            BasicBlock* cond_block = create_block();
            BasicBlock* body_block = create_block();
            BasicBlock* exit_block = create_block();

            // Jump to condition block
            current_block->tacInstruction.push_back(TAC{TACType::JUMP, "", "", "", cond_block->label});
            //current_block->next = cond_block;

            // Condition block
            current_block = cond_block;
            std::string cond_temp = visit_for_IR(node->children.front());
            current_block->tacInstruction.push_back(TAC{TACType::COND_JUMP, cond_temp, "", body_block->label, exit_block->label});
            cond_block->next_true = body_block;
            cond_block->next_false = exit_block;

            // Body block
            current_block = body_block;
            visit_for_IR(*std::next(node->children.begin()));
            body_block->tacInstruction.push_back(TAC{TACType::JUMP, "", "", "", cond_block->label});
            //body_block->next = cond_block;

            // Exit block becomes new current
            current_block = exit_block;
            return "";
        }
        else if (node->type == "IF LP expression RP statement ELSE statement") {
            BasicBlock* true_block = create_block();
            BasicBlock* false_block = create_block();
            BasicBlock* merge_block = create_block();

            // Condition evaluation
            std::string cond_temp = visit_for_IR(node->children.front());
            current_block->tacInstruction.push_back(TAC{
                TACType::COND_JUMP, 
                cond_temp, 
                "", 
                true_block->label, 
                false_block->label
            });
            current_block->next_true = true_block;
            current_block->next_false = false_block;

            // True branch
            current_block = true_block;
            visit_for_IR(*std::next(node->children.begin())); 
            true_block->tacInstruction.push_back(TAC{TACType::JUMP, "", "", "", merge_block->label});
            //true_block->next = merge_block;

            // False branch
            current_block = false_block;
            visit_for_IR(*std::next(node->children.begin(), 2));
            false_block->tacInstruction.push_back(TAC{TACType::JUMP, "", "", "", merge_block->label});
            //false_block->next = merge_block;

            // Continue with merge block
            current_block = merge_block;
            return "";
        }
        
        else if (node->type == "RETURN") {
            Node* ret_val = node->children.front();
            std::string ret_temp = visit_for_IR(ret_val);
            current_block->tacInstruction.push_back(TAC{TACType::RETURN, "", ret_temp, ""}); // Dedicated RETURN type
            return "";
        }
        else if (node->type == "exp DOT ident LP exp COMMA exp RP") {
            // Handle method calls with arguments
            Node* obj_node = node->children.front(); // e.g., "THIS"
            Node* method_node = *std::next(node->children.begin()); // e.g., "foo2"
            Node* args_node = *std::next(node->children.begin(), 2); // "argument_list"
        
            std::vector<std::string> arg_temps;
            for (auto arg_child : args_node->children) {
                std::string arg_temp = visit_for_IR(arg_child);
                arg_temps.push_back(arg_temp);
            }
        
            // Generate TAC for method call (e.g., "t0 = CALL foo2, arg1, arg2")
            std::string result_temp = new_temp();
            std::string args_str = "";
            for (const auto& arg : arg_temps) {
                args_str += arg + ", ";
            }
            if (!args_str.empty()) args_str = args_str.substr(0, args_str.size() - 2);
        
            current_block->tacInstruction.push_back(TAC{
                TACType::CALL, 
                result_temp, 
                method_node->value, 
                args_str, 
                ""
            });
            return result_temp;
        }
        else if (node->type == "LESS_THAN") {
            Node* left = node->children.front();
            Node* right = *std::next(node->children.begin());
            std::string left_temp = visit_for_IR(left);
            std::string right_temp = visit_for_IR(right);
            std::string cond_temp = new_temp();
            current_block->tacInstruction.push_back(TAC{TACType::BIN_OP, cond_temp, left_temp, right_temp, "<"});
            return cond_temp;
        }

        else if (node->type == "INT" || node->type == "TRUE" || node->type == "FALSE") {
            std::string temp = new_temp();
            current_block->tacInstruction.push_back(TAC{TACType::ASSIGN, temp, node->value, ""});
            return temp;
        }
        else if (node->type == "SIMPLE PRINT LOL") {
            Node* value_node = node->children.front();
            std::string value_temp = visit_for_IR(value_node);
            current_block->tacInstruction.push_back(TAC{TACType::PRINT, "", value_temp, ""});
            return "";
        }
        else {
            // Default case for other nodes
            for (auto child : node->children) {
                visit_for_IR(child);
            }
            return "";
        }
    }
// if, while, class, methods

private:
    

    

    string new_temp() {
        return "t" + std::to_string(temp_counter++);
    }

    string extractClass(const vector<string>& vec, string toFind) {
        for (const string& s : vec) {
            // Check if the string contains "InInt"
            if (s.find(toFind) != string::npos) {
                // Extract the part before the '.' (i.e., "Element")
                size_t dotPos = s.find('.');
                if (dotPos != string::npos) {
                    return s.substr(0, dotPos);
                }
            }
        }
        return ""; // Return an empty string if "InInt" is not found
    }

    void extract_arguments(Node* args_node, vector<Node*>& args) {
        if (!args_node) return;
        
        if (args_node->type == "exp DOT ident LP exp COMMA exp RP"){
            Node* var_name = *std::next(args_node->children.begin());//identifier:a3
            Node* if_argument_or_argument_list = *std::next(args_node->children.begin(), 2); //argument
            
            if (if_argument_or_argument_list->type == "argument"){
                args.push_back(var_name);
            }
            if (if_argument_or_argument_list->type == "argument_list"){ // send back the third node
                for (auto child : if_argument_or_argument_list->children){
                    extract_arguments(child, args);
                }
            }
            if (args_node->type == "exp DOT ident LP exp COMMA exp RP"){
                for (auto child : args_node->children){
                    extract_arguments(child, args);
                }
                
            }
        }
        
    }

    Node* find_declared_method_type(Node* n, const string& searched){
        //cout << n->type << endl;
        if (n->type == "methodDec"){
            Node* identifier = *std::next(n->children.begin());
            //cout << identifier->value << endl; // a1
            if (identifier->value == searched) { //found it 
                return n->children.front();
            }
            // if (identifier->type != "INT LB RB");
            return nullptr;
        }
       
        for (auto child : n->children){
            Node* result = find_declared_method_type(child, searched);
            if (result) return result; // Propagate found result immediately
        }
        return nullptr;
        
    }
    string findTypeUsingSymbolReq(Node* n, const string& searchingNode){
        if (n->type == searchingNode && searchingNode == "DOT identifier LP arguments RP"){
            Node* identifier = n->children.front();
            return identifier->value;
        }
        if (n->type == searchingNode){
            Node* identifier = *std::next(n->children.begin());
            return identifier->value;
        }
        for (auto child : n->children){
            string result = findTypeUsingSymbolReq(child, searchingNode);
            if (!result.empty()) return result;
        }
        return "";
    }
    
    string getTypeOfVar(const string& searched){
        string found_class = extractClass(method_scope_name, searched);
        Scope* get_class_scope = symtab.get_class_scope(found_class); // FIND a3 (A)
        if (get_class_scope){
            Symbol* get_class_name_for_func = get_class_scope->lookup(searched);
            if (get_class_name_for_func){
                Scope* get_method_for_func = symtab.get_method_scope(get_class_scope->name, searched);
                if (get_method_for_func){
                    Symbol* func_sym = get_method_for_func->lookup(searched);
                    return func_sym->type;
                }
            }
        }
        return "";
    }

    Symbol* getSymbolForFunction_For_parameters(const string& searched){
        string found_class = extractClass(method_scope_name, searched);
        Scope* get_class_scope = symtab.get_class_scope(found_class); // FIND a3 (A)
        if (get_class_scope){
            Symbol* get_class_name_for_func = get_class_scope->lookup(searched);
            if (get_class_name_for_func){
                // get_class_name_for_func = Inint
                // get_class_scope->name = Element
                Scope* get_method_for_func = symtab.get_method_scope(get_class_scope->name, searched);
            
                if (get_method_for_func){
                    Symbol* func_sym = get_method_for_func->lookup(searched);
                    return func_sym;
                }
            }
        }
        return nullptr;
    }

    Symbol* getSymbolForMethod_For_parameters(const string& className, const string& methodName, const string& searched){
        Scope* get_class_scope = symtab.get_parameter_scope(className, methodName, searched); // FIND a3 (A)
        if (get_class_scope){
            Symbol* get_class_name_for_func = get_class_scope->lookup(searched);
            return get_class_name_for_func;
        }
        return nullptr;
    }

    //Symbol* getSymbolFor

    void handle_variable(Node* node){
        // its a normal variable just add it to the symtab
        Node* type_node = node->children.front(); // type of variable (int, string)
        Node* indentifier_var = *std::next(node->children.begin()); //identifier (a, bar)

        //string type_str = (type_node->type == "INT LB RB") ? "INT[]" : type_node->type;
        string type_str;
        if (type_node->type == "typechar") {
            Node *class_name = type_node->children.front();
            type_str = class_name->value; // Use class name "classdata"
        } else {
            type_str = type_node->type;  // Primitive types like "INT", "boolean"
        }

        Symbol var_sym {
            indentifier_var->value,
            VARIABLE,
            type_str,  // Store as "INT[]" for arrays
            node->lineno
        };
        
        symtab.add_symbol(var_sym);
    }

    
    std::string getNodeReturnType(Node* n) {
        if (!n) return "unknown";
    
        // 1) Handle literals
        if (n->type == "INT" || n->type == "expression DOT LENGTH"||
             n->type =="AddExpression" || n->type == "MultExpression") {
            return "INT";
        }
        if (n->type == "TRUE" || n->type == "FALSE" || n->type == "LESS_THAN" ) {
            return "BOOLEAN";
        }
    
        // 2) Handle Identifiers
        if (n->type == "identifier") {
            Symbol* sym = symtab.lookup(n->value);
            if (sym) {
                return sym->type;
            }
            return "unknown";
        }
    
        // 3) Handle Function Calls (`functionCall`)
        if (n->type == "functionCall") {
    
            Node* DotExpNode = *std::next(n->children.begin(),3);
           return getNodeReturnType(DotExpNode);
        }
    
        // 4) Handle `THIS` keyword
        if (n->type == "THIS") {
            return curr_class_name;
        }
    
        // 5) Handle Class Instantiation (`new ClassName()`)
        if (n->type == "NEW identifier LP RP") {
            if (n->children.empty()) {
                return "unknown";
            }
    
            Node* classNameNode = n->children.front();
            if (!classNameNode || classNameNode->value.empty()) {
                return "unknown";
            }
    
            std::string className = classNameNode->value;
            Scope* classScope = symtab.get_class_scope(className);
            if (!classScope) {
                return "unknown";
            }
    
            return className;
        }
    
        // 6) Handle Array Allocation (`new int[expression]`)
        if (n->type == "NEW INT LEFT_BRACKET expression RIGHT_BRACKET") {
            std::cout << "DEBUG: Resolving 'new int[expression]' to 'INT LB RB'" << std::endl;
            return "INT LB RB";  // Represents an `int[]`
        }
    
        // 7) Handle Method Calls (`exp DOT ident LP arguments RP`)
        if (n->type == "exp DOT ident LP arguments RP" || n->type == "exp DOT ident LP exp COMMA exp RP") {
            if (n->children.size() < 2) {
                return "unknown";
            }
    
            Node* objectNode = n->children.front();
            Node* methodNameNode = *std::next(n->children.begin());
    
            if (!methodNameNode || methodNameNode->value.empty()) {
                return "unknown";
            }
    
            std::string objectType = getNodeReturnType(objectNode);
            std::string methodName = methodNameNode->value;
    
            if (objectType == "unknown") {
                return "unknown";
            }
    
            Scope* classScope = symtab.get_class_scope(objectType);
            if (!classScope) {
                return "unknown";
            }
    
            Symbol* methodSym = classScope->lookup(methodName);
            if (!methodSym || methodSym->type.empty()) {
                return "unknown";
            }
    
            return methodSym->type;
        }
    
        // 8) Handle Arithmetic Expressions
        if (n->type == "MultExpression" || n->type == "AddExpression" || n->type == "SubExpression") {
            if (n->children.size() < 2) {
                return "unknown";
            }
    
            Node* leftNode  = n->children.front();
            Node* rightNode = *std::next(n->children.begin());
    
            std::string leftType  = getNodeReturnType(leftNode);
            std::string rightType = getNodeReturnType(rightNode);
    
            if (leftType == "INT" && rightType == "INT") {
                return "INT";
            }
    
            return "unknown";
        }
    
        // 9) Assignment Validation: Prevent `int = int[]`
        if (n->type == "assignment") {
            if (n->children.size() < 2) {
                return "unknown";
            }
    
            Node* leftNode = n->children.front();
            Node* rightNode = *std::next(n->children.begin());
    
            std::string leftType = getNodeReturnType(leftNode);
            std::string rightType = getNodeReturnType(rightNode);
    
            std::cout << "DEBUG: Assignment leftType: " << leftType 
                      << " rightType: " << rightType << std::endl;
    
            if (leftType == "INT" && rightType == "INT LB RB") {
                std::cout << "Error: Cannot assign 'int[]' to 'int'" << std::endl;
                return "unknown";
            }
    
            return leftType;
        }
    
        // 10) Recursive Fallback
        for (Node* child : n->children) {
            std::string childType = getNodeReturnType(child);
            if (childType != "unknown") {
                return childType;
            }
        }
    
        return "unknown";
    }

    
    void handle_array_access(Node* array_access){

        if (array_access->children.size() == 3){ // SOMETHING [ASSIGNED] = TO SOMETHING with expression DOT LENGTH:

            Node* array_node = array_access->children.front();
            Node* index_node = *std::next(array_access->children.begin());
            Node* third_assigned_to_or_length = *std::next(array_access->children.begin(), 2);
            //cout << "wgoreiujnowgn0ieognwoiegnogeniwogwien " << index_node->type << endl;
            string type_ = get_exp_type(index_node); // if its int, unknown, boolean [int, unknown, boolean]
            //cout << "wgoreiujnowgn0ieognwoiegnogeniwogwien " << type_ << endl;
            // num_aux[3] = num.length; // @error - semantic (member .length is used incorrectly)


            //cout << "WAKAKAKAKKAKKAKAKAKAKAKA " << third_assigned_to_or_length->type << endl;

            if (third_assigned_to_or_length->type == "expression DOT LENGTH") {
                string array_type = get_exp_type(array_node);
                //cout << "EHHERHERHERHERH " << array_type << endl;
                Node* type_of_child_length = third_assigned_to_or_length->children.front(); // identifier:num

                string type_of_child_length_type = get_exp_type(type_of_child_length); // int LB RB

                if (type_of_child_length_type != "INT LB RB"){
                    res.push_back(std::make_tuple(array_node->lineno, "semantic (member .length is used incorrectly)"));
                    symtab.error_count++;
                }


                
            }

            
            if (type_ == "int") return; // return directly because 2nd child is an INT
            if (type_ != "INT") { // the type is in INT with capital (its the type of the node)
                // std::cerr << "\n@error Line " << index_node->lineno
                //       << ": semantic (invalid type of array index)" << std::endl;
                      //<<index_type
                      //<< "', expected 'int'\n";
                res.push_back(std::make_tuple(index_node->lineno, "semantic (invalid type of array index)"));
                //cout << "wgoreiujnowgn0ieognwoiegnogeniwogwien";
                symtab.error_count++;
            }
            // handle case with identifiers
            
        }   

    }
    
   
    string get_exp_type(Node* exp_node) {
        if (!exp_node) return "unknown";
        /* handle literals */
        
        if (exp_node->type == "FALSE" || exp_node->type == "TRUE") return "boolean";
        if (exp_node->type == "INT" || exp_node->type == "INT") return "int";
        
        //cout << "yo " << exp_node->type << endl;

        /* handle identifiers */
        if (exp_node->type == "identifier") {
            cout << "THIS SHOULD BE (a2)      " << exp_node->value << endl;
            Symbol* sym = symtab.lookup(exp_node->value);
            if (sym) return sym->type; // if its int or not
            return "unknown"; // Identifier not found
        }   
        // Handle method calls (both direct and through 'this')
        if (exp_node->type == "exp DOT ident LP exp COMMA exp RP") {
            Node* method_name_node = *std::next(exp_node->children.begin());
            Symbol* sym = symtab.lookup(method_name_node->value);
            if (!sym) {
                // Try the mangled key if the plain lookup failed.
                sym = symtab.lookup(method_name_node->value + "$m");
            }
            if (sym) return sym->type;
        }
        // // Handle array access
        // if (exp_node->type == "expression LEFT_BRACKET expression RIGHT_BRACKET") {
        //     Node* array_node = exp_node->children.front();
        //     string base_type = get_exp_type(array_node);
        //     if (base_type.rfind("[]") != string::npos) {
        //         return base_type.substr(0, base_type.length() - 2); // Remove [] from type
        //     }
        // }
        // Handle .length
        
        if (exp_node->type == "expression DOT LENGTH") {
            // Retrieve the expression on which .length is applied.
            Node* lhs = exp_node->children.front();
            string lhs_type = get_exp_type(lhs);
            cout << "DBNWAOIDWNAODNWADINWADOIAW" << endl;
            // Check if the type is an array type (i.e. ends with "[]")
            if (lhs_type != "INT LB RB") {
                // Error: .length called on a non-array
                res.push_back(std::make_tuple(exp_node->lineno,
                    "semantic (member .length is used incorrectly)"));
                symtab.error_count++;
                return "unknown";
            }
            // If it's "INT LB RB", then arr.length is valid:
            return "int";
        }

        // UNSURE:
        /* handle array accesses recursively */
        // Recursive handling for complex expressions
        for (auto child : exp_node->children) {
            string child_type = get_exp_type(child);
            if (child_type != "unknown") return child_type;
        }
        /* TODO: more cases */
        if (exp_node->type == "expression DOT LENGTH") {
            Node* lhs = exp_node->children.front();
            string lhs_type = get_exp_type(lhs);
    
            // Handle array types stored as "INT[]" or "INT LB RB"
            if (lhs_type != "INT[]" && lhs_type != "INT LB RB") {
                res.push_back(std::make_tuple(exp_node->lineno, 
                    "semantic (member .length is used incorrectly)"));
                symtab.error_count++;
                return "unknown";
            }
            return "int";
        }

        return "unknown";
    }
   
public:



    void printCorrect(){
        // print only string not the int, the int is the line number
        // sort based on the int in the tuple
        std::sort(res.begin(), res.end());
        for (auto i : res){
            std::cerr << "\n@error at line " << std::get<0>(i) << ": "<< std::get<1>(i) << std::endl;
        }
    }
};
