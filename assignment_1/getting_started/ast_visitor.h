# include "Node.h"
# include "symtab.h"


 /*
 My own lexicon of errors:  
 Node* id_node = *std::next(var_node->children.begin(), 2000); gives segmentation fault if the child/node does not exist.
 
 */
class ASTVisitor {
private:
    SymbolTable &symtab;
    
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
            std::cerr << "Semantic error @ line " << class_name_node->lineno
                    << ": Duplicate class '" << class_sym.name << "'\n";
            return;
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
    
        // Add method to CLASS scope
        Symbol method_sym{
            method_name_node->value,
            METHOD,
            return_type_node->type,
            method_name_node->lineno
        };
        
        if (!symtab.add_symbol(method_sym)) {
            std::cerr << "Semantic error @ line " << method_name_node->lineno
                      << ": Duplicate method '" << method_sym.name
                      << "' in class '" << symtab.current_scope->get_name() << "'\n";
            return;
        }
    
        // Enter METHOD scope
        symtab.enter_scope(method_sym.name);
    
        // Process parameters
        if (params_node->type == "parameters" || params_node->type == "empty parameters") {
            for (auto param_child : params_node->children) {
                if (param_child->type == "parameter_list") {
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
            std::cerr << "Semantic error @ line " << class_name_node->lineno
                    << ": Duplicate class '" << class_sym.name << "'\n";
            return; // Skip processing body for duplicates
        }

        symtab.enter_scope(class_sym.name);
        
        // Explicitly process class body nodes
        for (auto it = std::next(class_node->children.begin()); 
            it != class_node->children.end(); ++it) {
            visit(*it); // Process fields/methods within class scope
        }
        
        symtab.exit_scope();
    }

    void handle_parameter_list(Node* param_list, Symbol& method_sym) {
        if (!param_list || param_list->type != "parameter_list") return;
    
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

    void add_parameter(Node* id_node, Node* type_node, Symbol& method_sym) {
        Symbol param_sym{
            id_node->value,
            PARAMETER,
            type_node->type,
            id_node->lineno
        };
        if (!symtab.add_symbol(param_sym)) {
            std::cerr << "Semantic error @ line " << id_node->lineno
                      << ": Duplicate parameter '" << id_node->value
                      << "' in method '" << method_sym.name << "'\n";
        }
    }

};

