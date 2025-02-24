# include "Node.h"
# include "symtab.h"
# include <algorithm>


 /*
 My own lexicon of errors:  
 Node* id_node = *std::next(var_node->children.begin(), 2000); gives segmentation fault if the child/node does not exist.
 
 Use the identifier node's line number for DECLARATIONS (classes/methods/variables).
 and Use the node's own line number for OTHER CONSTRUCTS:(statements/expressions)

 for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child); RUN THIS TO SEE ALL CHILDS
 */
class ASTVisitor {
private:
    SymbolTable &symtab;
    vector<tuple<int, string>> res; // line number, error message
public:
    ASTVisitor(SymbolTable &st) : symtab(st) {}

    void visit_THE_WHOLE_AST_FOR_THE_SYMTAB(Node* node){
        if (!node) return;

        //bool handled = false;
        // if (node->type == "var declaration"){ //REMOVE THIS LATER JUST PUT HANDLE_VARIABLE IN BOTH MAIN CLASS AND CLASS DEC
        //     handle_variable(node);
        // }
        //cout << "AAAA " << node->type << " name: " << node->value<< endl;
        if (node->type == "goal" || node->type == "classDeclarations"){
            
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);
        }
        if (node->type == "classDeclaration"){
            Node* class_name_node = node->children.front(); // identifier:DuplicateIdentifiers
            
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
        }
        
        if (node->type == "reqVarDeclaration"){
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);  
        }

        if (node->type == "var declaration"){
            handle_variable(node);
        }

        if (node->type == "reqMethodDeclaration methodDeclaration"){
            // look for all childs until it sees something in style with "METHODDECLARATION VARDECLARATION:"
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);
            // THEN IT WILL GO TO AN IF STATEMENT (DOWN BELOW)
        }

        if (node->type == "METHODDECLARATION VARDECLARATION"){
            //Node* method_type = node->children.front(); // INT RETURN TYPE (HERERERERE) open it up if it needs usage
            
            Node* indentifier_method = *std::next(node->children.begin()); // identifier:func

            Symbol method_sym {
                indentifier_method->value,
                METHOD,
                indentifier_method->type,
                indentifier_method->lineno
            };
            //symtab.exit_scope();
            symtab.add_symbol(method_sym);
            symtab.enter_scope(method_sym.name);

            
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);  
            symtab.exit_scope();

        }


        if (node->type == "parameters"){
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);  
        }
        if (node->type == "parameter"){
            Node* indentifier_parameter = *std::next(node->children.begin()); //identifier:param

            Symbol param_sym {
                indentifier_parameter->value,
                PARAMETER,
                indentifier_parameter->type,
                node->lineno
            };

            symtab.add_symbol(param_sym); // ADD parameter or parameters to the symbol table.

        }

        if (node->type == "reqVarOrStmt varDeclaration"){
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child); //int param; // @error - semantic (Already Declared parameter: 'param')
        }

        if (node->type == "MAIN CLASS"){
            Node* main_class_name_node = node->children.front();

            Symbol main_sym {
                main_class_name_node->value,
                MAIN,
                main_class_name_node->type,
                node->lineno
            };

            symtab.add_symbol(main_sym);
            symtab.enter_scope(main_sym.name);
            for (auto child : node->children) visit_THE_WHOLE_AST_FOR_THE_SYMTAB(child);  
            symtab.exit_scope();
        }

        
        
    }

    void visit(Node* node){ /* VISIT ALL THE NODES IN THE AST (pdf file or smthn)*/
        if (!node) return;

        
        // if (node->type == "SOMETHING [ASSIGNED] = TO SOMETHING") { 
        //     handle_array_access(node); 
        // } // num_aux[false] = 2;
        
        // else if (node->type == "expression LEFT_BRACKET expression RIGHT_BRACKET") { 
        //     handle_array_this_access(node); 
        // }

        
        for (auto child : node->children) visit(child);
    }

private:
    //helper function:
    //handle requsiverly
    Node* rec_classDeclaration_all_childs(Node* child_of_method_dec){
        //used to find METHODDECLARATION VARDECLARATION
        // because it can be anywhere in the AST so we do it recursively until we find it. 
        //Mark please. When found.
        
        if (child_of_method_dec->type == "reqMethodDeclaration methodDeclaration"){
            for (auto i : child_of_method_dec->children){
                rec_classDeclaration_all_childs(i);
            }
            //rec_classDeclaration_all_childs(child_of_method_dec->children);
        }
        
        else if (child_of_method_dec->type == "METHODDECLARATION VARDECLARATION"){
            Node* children_of_METHOD_DECLARATION  = child_of_method_dec;

            for (auto i : children_of_METHOD_DECLARATION->children){
                if (i->type == "identifier"){
                    //add it to the sym tab
                    cout << "I ACTUALLY GOT HERE";
                }
            }
        }

    }

    void handle_variable(Node* node){
        // its a normal variable just add it to the symtab
        Node* type_var = node->children.front(); // type of variable (int, string)
        Node* indentifier_var = *std::next(node->children.begin()); //identifier (a, bar)

        Symbol var_sym {
            indentifier_var->value,
            VARIABLE,
            type_var->type,
            node->lineno
        };

        symtab.add_symbol(var_sym);
    }

    // void handle_parameter_list(Node* param_list, Symbol& method_sym) {

    //     if (!param_list) return;

    //     // Base case: type identifier
    //     if (param_list->children.size() == 2) {
    //         Node* type_node = param_list->children.front();
    //         Node* id_node = *std::next(param_list->children.begin());
    //         add_parameter(id_node, type_node, method_sym);
    //     }
    //     // Recursive case: parameter_list COMMA type identifier
    //     else if (param_list->children.size() >= 3) {
    //         handle_parameter_list(param_list->children.front(), method_sym);
    //         Node* type_node = *std::next(param_list->children.begin(), 1);
    //         Node* id_node = *std::next(param_list->children.begin(), 2);
            
    //         add_parameter(id_node, type_node, method_sym);
    //     }
        
    // }
    /*
     public int Pen(int param, int param) {// @error - semantic (Already Declared parameter: 'param')
        return 1;
    }
    */

    /* InvalidArrayInteger.java (handle arrays access) */
    void handle_array_this_access(Node* array_this_access){

        
        Node* array_node = array_this_access->children.front(); // identifier:num_aux
        Node* index_node = *std::next(array_this_access->children.begin()); // exp DOT ident LP exp COMMA exp RP:

        if (index_node->type == "exp DOT ident LP exp COMMA exp RP"){
            Node* type_index_node = *std::next(index_node->children.begin()); // identifier:a2

            string type_index = get_exp_type(type_index_node);
            // run writeAllSymbols
            cout << symtab.writeAllSymbols() << endl;
            cout << "EHHEHEHEHEHEH " << type_index << endl;
        }


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
        if (exp_node->type == "Int" || exp_node->type == "INT") return "int";
        
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
    // Node* find_return_statement(Node* method_node) {
    //     for (auto child : method_node->children) {
    //         if (child->type == "RETURN") return child;
    //         Node* found = find_return_statement(child);
    //         if (found) return found;
    //     }
    //     return nullptr;
    // }

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
