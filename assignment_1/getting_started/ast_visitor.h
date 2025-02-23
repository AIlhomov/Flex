# include "Node.h"
# include "symtab.h"
# include <algorithm>


 /*
 My own lexicon of errors:  
 Node* id_node = *std::next(var_node->children.begin(), 2000); gives segmentation fault if the child/node does not exist.
 
 */
class ASTVisitor {
private:
    SymbolTable &symtab;
    vector<tuple<int, string>> res; // line number, error message
public:
    ASTVisitor(SymbolTable &st) : symtab(st) {}

    void visit(Node* node){ /* VISIT ALL THE NODES IN THE AST (pdf file or smthn)*/
        if (!node) return;

        bool handled = false;
        // right:
        if (node->type == "var declaration"){ handle_variable(node); }

        else if (node->type == "METHODDECLARATION VARDECLARATION") {
            handle_method(node);
            handled = true;
        }
        else if (node->type == "classDeclaration") {
            handle_class(node);
            handled = true;
        }
        else if (node->type == "MAIN CLASS") {
            handle_main_class(node);
            handled = true;
        }
        else if (node->type == "SOMETHING [ASSIGNED] = TO SOMETHING") { handle_array_access(node); } // num_aux[false] = 2;
        else if (node->type == "expression LEFT_BRACKET expression RIGHT_BRACKET") { handle_array_access(node); }

        //else if (node->type == "expression LEFT_BRACKET expression RIGHT_BRACKET") { handle_expr_lb_expr_rb(node); }

        if (!handled) for (auto child : node->children) visit(child);
    }

private:
    void handle_main_class(Node* main_class) {
        Node* class_name_node = main_class->children.front(); 
        Symbol class_sym{
            class_name_node->value,
            CLASS,
            "class",
            class_name_node->lineno
        };

        if (!symtab.add_symbol(class_sym)) {
            // std::cerr << "semantic @error  line " << class_name_node->lineno
            //         << ": Duplicate class '" << class_sym.name << "'\n";
            // return;
        }

        // Enter class scope
        symtab.enter_scope(class_sym.name);

        // Process main class body
        for (auto it = std::next(main_class->children.begin()); 
            it != main_class->children.end(); ++it) {
            visit(*it);
        }

        symtab.exit_scope();
    }
    
    void handle_variable(Node* var_node) {

        if (var_node->children.size() < 2) { /* we know for a FACT that the pdf file (tree) generated a variable 
            declaration with 2 children under */
            std::cerr << "Malformed variable declaration at line " << var_node->lineno << std::endl;
            return;
        }

        Node* type_node = var_node->children.front(); // yo. WORKS.
        Node* id_node = *std::next(var_node->children.begin()); // WORKS.

        

        Symbol var_sym{
            id_node->value,  // Variable name (should be to the right child) 
            VARIABLE,       // kind
            type_node->type,  // Type (should be to the left child) (do this !)
            var_node->lineno // line_no
        };
        // if (type_node->children.size() > 0) {
        //     var_sym.dimension = count_array_dimensions(type_node);
        // }

        symtab.add_symbol(var_sym);
    }

    void handle_parameter(Node* param_node, Symbol& method_sym) {
        if (param_node->children.size() < 2) return;
    
        Node* type_node = param_node->children.front();
        Node* id_node = *std::next(param_node->children.begin());
    
        /* spot if there is 3 children if so solve it recursevely */

        Symbol param_sym{
            id_node->value,
            PARAMETER,
            type_node->type,
            id_node->lineno  // Parameter's actual line number
        };
        symtab.add_symbol(param_sym);
    }
    
    void handle_method(Node* method_node) {
        if (method_node->children.size() < 3) return;
    
        Node* return_type_node = method_node->children.front();
        Node* method_name_node = *std::next(method_node->children.begin());
        Node* params_node = *std::next(method_node->children.begin(), 2);
        //
        //
        Node * method_body = *std::next(method_node->children.begin(), 3);
        //Node * returnedNode = *std::next(method_node->children.begin(), 4);


    
        // Add method to CLASS scope
        Symbol method_sym{
            method_name_node->value,
            METHOD,
            return_type_node->type,
            method_name_node->lineno
        };
        
        // Instead of calling symtab.add_symbol directly, check for a duplicate.
        // If a symbol with the same name already exists (e.g. a field), store
        // the method symbol under a mangled key.
        // Check if a symbol with the same name already exists in the current class scope.
        auto it = symtab.current_scope->symbols.find(method_name_node->value);
        if (it != symtab.current_scope->symbols.end()) {
            // If the existing symbol is a method, then this is a duplicate function declaration.
            if (it->second.kind == METHOD) {
                // std::cerr << "\n@error at line " << method_name_node->lineno
                //         << ": Duplicate function '" << method_name_node->value 
                //         << "' in class " << symtab.current_scope->get_name() << "\n";
                res.push_back(std::make_tuple(method_name_node->lineno, "Duplicate function '" + method_name_node->value + "' in class " + symtab.current_scope->get_name()));
                symtab.error_count++;
                return; // Do not add the duplicate function.
            }
            else {
                // If the existing symbol is a field (or something else), mangle the method key.
                symtab.current_scope->symbols[method_name_node->value + "$m"] = method_sym;
            }
        }
        else {
            // No conflict; add the method normally.
            symtab.current_scope->symbols[method_name_node->value] = method_sym;
        }
    
        // Enter METHOD scope
        symtab.enter_scope(method_sym.name);
    
        // Process parameters
        if (params_node->type == "parameters" || params_node->type == "empty parameters") {
            
            for (auto param_child : params_node->children) {

                if (param_child->type == "parameter_list COMMA type identifier") {
                    handle_parameter_list(param_child, method_sym);
                }
                else if (param_child->type == "type identifier") {
                    handle_parameter(param_child, method_sym);
                }
            }
        }
    
        // Process method body (variables)
        if (method_node->children.size() > 3) {
            Node* body_node = *std::next(method_node->children.begin(), 3);
            visit(body_node);
        }

        // // Check return type
        // if (method_node->children.size() > 3) {
        //     Node* return_node = find_return_statement(method_node);
        //     if (return_node) {
        //         string actual_type = get_exp_type(return_node);
        //         if (actual_type != method_sym.type) {
        //             std::cerr << "semantic @error  line " << return_node->lineno
        //                     << ": Return type mismatch. Expected '" 
        //                     << method_sym.type << "', got '" 
        //                     << actual_type << "'\n";
        //             symtab.error_count++;
        //         }
        //     }
        // }
        
        
        if(find_return_statement(method_node) != nullptr){
            Node* returnedNode = find_return_statement(method_node)->children.front();
            //Node* return_node = find_return_statement(method_node);
            
            //std::cout << "return_type_node->type: " << return_type_node->type << std::endl;
            //std::cout << "returnedNode->value: " << returnedNode->value << std::endl;
            
            if(symtab.lookup(returnedNode->value) != nullptr){
                //std::cout << "symtab.lookup(returnedNode->value)->type: " << symtab.lookup(returnedNode->value)->type << std::endl;
                std::string returned_type = symtab.lookup(returnedNode->value)->type;

                if (return_type_node->type != returned_type) {
                    // std::cerr << "\n@error Line " << return_type_node->lineno
                    //     << ": semantic (type mismatch)"  << std::endl;
                        //<< return_type_node->type << "', got '" 
                        //<< returned_type << "'\n";
                    res.push_back(std::make_tuple(return_type_node->lineno, "semantic (type mismatch)"));
                        symtab.error_count++;
                    }
                    
            }
        }
                
        symtab.exit_scope();
    }

    void handle_class(Node* class_node) {
        if (class_node->children.empty()) return;

        Node* class_name_node = class_node->children.front();
        Symbol class_sym{
            class_name_node->value,
            CLASS,
            "class",
            class_name_node->lineno
        };


        // Add class to global scope
        if (!symtab.add_symbol(class_sym)) {
            // std::cerr << "semantic @error  line " << class_name_node->lineno
            //         << ": Duplicate class '" << class_sym.name << "'\n";
        }

        symtab.enter_scope(class_sym.name);
        
        // // Process parameters
        // if (params_node->type == "parameters" || params_node->type == "empty parameters") {
        //     for (auto param_child : params_node->children) {
        //         if (param_child->type == "parameter_list") {
        //             handle_parameter_list(param_child, method_sym);
        //         }
        //         else if (param_child->type == "type identifier") {
        //             handle_parameter(param_child, method_sym);
        //         }
        //     }
        // }

        // Explicitly process class body nodes
        for (auto it = std::next(class_node->children.begin()); 
            it != class_node->children.end(); ++it) {
            visit(*it); // Process fields/methods within class scope
        }
        
        symtab.exit_scope();
    }

    void handle_parameter_list(Node* param_list, Symbol& method_sym) {




        if (!param_list) return;


        // Base case: type identifier
        if (param_list->children.size() == 2) {
            Node* type_node = param_list->children.front();
            Node* id_node = *std::next(param_list->children.begin());
            add_parameter(id_node, type_node, method_sym);
        }
        // Recursive case: parameter_list COMMA type identifier
        else if (param_list->children.size() >= 3) {
            handle_parameter_list(param_list->children.front(), method_sym);
            Node* type_node = *std::next(param_list->children.begin(), 1);
            Node* id_node = *std::next(param_list->children.begin(), 2);
            
            add_parameter(id_node, type_node, method_sym);
        }
    }
    /*
     public int Pen(int param, int param) {// @error - semantic (Already Declared parameter: 'param')
        return 1;
    }
    */

    void add_parameter(Node* id_node, Node* type_node, Symbol& method_sym) {
        Symbol param_sym{
            id_node->value,
            PARAMETER,
            type_node->type,
            id_node->lineno
        };
        if (!symtab.add_symbol(param_sym)) {
            // std::cerr << "semantic @error  line " << id_node->lineno
            //           << ": Duplicate parameter '" << id_node->value
            //           << "' in method '" << method_sym.name << "'\n";
        }
    }

    /* InvalidArrayInteger.java (handle arrays access) */
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

        if (array_access->children.size() < 2) return;

        Node* array_node = array_access->children.front();
        Node* index_node = *std::next(array_access->children.begin());
        //Node* third_assigned_to_or_length = *std::next(array_access->children.begin(), 2);
        //cout << index_node->type << endl;
        string array_type = get_exp_type(array_node);

        // if (array_type.rfind("[]") != array_type.size() - 2) { /* similiar to ends_with("[]")*/
        //     std::cerr << "semantic @error  line " << index_node->lineno
        //           << ": Invalid array access on non-array type '" << array_type << "'\n";
        //     symtab.error_count++;
        // }

        string index_type = get_exp_type(index_node); // exp DOT ident LP exp COMMA exp RP
        cout << index_type << endl;
        
        
        // if (third_assigned_to_or_length->type == "") CONTINUE HERE
         

        /* if: int a;    and a[0] = 2*/
        Symbol* symlookup = symtab.lookup(array_node->value);
        
        /**
         *    std::cout <<"symLoopUp" << std::endl;
        std::cout<< symlookup->name << std::endl;
        std::cout<< symlookup->type << std::endl;
        std::cout<< symlookup->kind << std::endl;
        std::cout<< symlookup->dimension << std::endl;
        std::cout <<"symLoopUp END" << std::endl;
     

         * 
         */
     
      
        
     

    }
    

    string get_exp_type(Node* exp_node) {
        if (!exp_node) return "unknown";
        /* handle literals */
        
        if (exp_node->type == "FALSE" || exp_node->type == "TRUE") return "boolean";
        if (exp_node->type == "Int" || exp_node->type == "INT") return "int";
        
        //cout << "yo " << exp_node->type << endl;

        /* handle identifiers */
        if (exp_node->type == "identifier") {
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
        // Handle array access
        if (exp_node->type == "expression LEFT_BRACKET expression RIGHT_BRACKET") {
            Node* array_node = exp_node->children.front();
            string base_type = get_exp_type(array_node);
            if (base_type.rfind("[]") != string::npos) {
                return base_type.substr(0, base_type.length() - 2); // Remove [] from type
            }
        }
        // Handle .length
        
        if (exp_node->type == "expression DOT LENGTH") {
            // Retrieve the expression on which .length is applied.
            Node* lhs = exp_node->children.front();
            string lhs_type = get_exp_type(lhs);
            cout << "DBNWAOIDWNAODNWADINWADOIAW" << endl;
            // Check if the type is an array type (i.e. ends with "[]")
            if (lhs_type.size() < 2 || lhs_type.substr(lhs_type.size()-2) != "[]") {
                //  std::cerr << "\n@error Line " << exp_node->lineno
                //            << ": semantic (member .length is used incorrectly)" << std::endl;
                           //<<"on non-array type '" 
                           //<< lhs_type << "', expected an array type\n";
                //symtab.error_count++;

                 res.push_back(std::make_tuple(exp_node->lineno, "semantic (member .length is used incorrectly)"));
                 symtab.error_count++;
                 return "unknown";
            }
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

        return "unknown";
    }
    Node* find_return_statement(Node* method_node) {
        for (auto child : method_node->children) {
            if (child->type == "RETURN") return child;
            Node* found = find_return_statement(child);
            if (found) return found;
        }
        return nullptr;
    }

    void handle_expr_lb_expr_rb(Node* node){

        Node* array_node = node->children.front(); // identifier:num_aux
        Node* index_node = *std::next(node->children.begin()); // exp DOT ident LP exp COMMA exp RP: (ROOT)

        Node* index_node2 = *std::next(index_node->children.begin()); // identifier:a2

        //symtab.lookup(array_node->value);
        //Symbol* symlookup = symtab.lookup(index_node2->value);

        Symbol* array_sym = symtab.lookup(array_node->value);
        if (array_sym) {
            if (array_sym->type.find("[]") == string::npos) {
                // std::cerr << "\n@error Line " << array_node->lineno
                //       << ": semantic Attempting to use array access on non-array variable '" 
                //       << array_node->value << "'\n";
                res.push_back(std::make_tuple(array_node->lineno, "semantic Attempting to use array access on non-array variable '" + array_node->value + "'"));
                symtab.error_count++;
                return;
            }
        }

        string index_type = get_exp_type(index_node);
        
        if (index_node->type == "exp DOT ident LP exp COMMA exp RP") {
            Node* method_name_node = *std::next(index_node->children.begin()); // Get method name node
            Symbol* method_sym = symtab.lookup(method_name_node->value);
            
            if (method_sym) {
                // Check if method return type is not int
                if (method_sym->type != "int") {
                    // std::cerr << "\n@error Line " << method_name_node->lineno
                    //       << ": semantic Invalid array index type '" << method_sym->type
                    //       << "', expected 'int'\n";
                    res.push_back(std::make_tuple(method_name_node->lineno, "semantic Invalid array index type '" + method_sym->type + "', expected 'int'"));
                    symtab.error_count++;
                }
            }
        } 
        else if (index_type != "int") {
            // std::cerr << "\n@error Line " << index_node->lineno
            //       << ": semantic Invalid array index type '" << index_type 
            //       << "', expected 'int'\n";

            res.push_back(std::make_tuple(index_node->lineno, "semantic Invalid array index type '" + index_type + "', expected 'int'"));

            symtab.error_count++;
        }

        // if (symlookup == nullptr) {
        //     std::cerr << "semantic @error  line " << index_node2->lineno
        //           << ": Invalid array index type '" << index_node2->value 
        //           << "', expected 'int'\n";
        //     symtab.error_count++;
        // }


        //cout << symlookup->name << endl; segmentation fault if symlookup is null
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