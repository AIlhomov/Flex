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
        // right:
        if (node->type == "var declaration") handle_variable(node);

        // unsure:
        if (node->type == "METHODDECLARATION VARDECLARATION") handle_method(node);
        if (node->type == "classDeclaration") handle_class(node);

        for (auto child : node->children) visit(child);
    }

private:

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
        if (method_node->children.size() < 3) {
            std::cerr << "Malformed method declaration at line " << method_node->lineno << std::endl;
            return;
        }
    
        // Extract method components from AST structure
        Node* return_type_node = method_node->children.front();
        Node* method_name_node = *std::next(method_node->children.begin());
        Node* params_node = *std::next(method_node->children.begin(), 2);
        
        
        // Create method symbol with proper line number
        Symbol method_sym{
            method_name_node->value,  // Method name from identifier node
            METHOD,
            return_type_node->type,   // Return type from type node
            method_name_node->lineno  // Use identifier's line number
        };
    
        // Process parameters
        if (params_node->type == "parameters") {
            for (auto param : params_node->children) {
                if (param->type == "parameter_list") {
                    for (auto param_child : param->children) {
                        handle_parameter(param_child, method_sym);
                    }
                }
            }
        }
    
        // Add method to symbol table
        if (!symtab.add_symbol(method_sym)) {
            std::cerr << "Duplicate method '" << method_sym.name 
                      << "' declaration at line " << method_name_node->lineno << std::endl;
            return;
        }
    
        // Enter method scope
        symtab.enter_scope(method_sym.name);
    
        // Process parameters in method scope
        if (params_node->type == "parameters") {
            for (auto param : params_node->children) {
                if (param->type == "parameter_list") {
                    for (auto param_child : param->children) {
                        handle_parameter(param_child, method_sym);
                    }
                }
            }
        }
    
        // Process method body
        // ...
    
        symtab.exit_scope();
    }

    void handle_class(Node* class_node){
        if (class_node->children.empty()) return;
    
        Node* class_name_node = class_node->children.front();
        Symbol class_sym{
            class_name_node->value,
            CLASS,
            "class",
            class_name_node->lineno  // Use identifier's line number
        };
        

        //remove ?
        if (!symtab.add_symbol(class_sym)) {
            std::cerr << "Semantic error @ line " << class_name_node->lineno 
                    << ": Duplicate class '" << class_sym.name << "'\n";
            return;
        }
        
        symtab.enter_scope(class_sym.name);

        // Process class body
        // ...

    }
};

