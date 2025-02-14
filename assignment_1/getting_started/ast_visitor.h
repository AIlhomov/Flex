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

    void handle_parameter(Node* param_node, Symbol& method_sym){
        if (param_node->children.size() < 2) return; // malformed parameter

        // 2 children: type and id
        Node* type_node = param_node->children.front(); // left
        Node* id_node = *std::next(param_node->children.begin()); // right

        method_sym.param_types.push_back(type_node->type);

        Symbol param_sym{
            id_node->value,
            PARAMETER,
            type_node->type,
            param_node->lineno
        };

        symtab.add_symbol(param_sym);
    }
    
    void handle_method(Node* method_node){
        // if method_node has children: [ReturnType, Parameters, Body]

        Symbol method_sym{
            method_node->value, // Method name
            METHOD,
            method_node->children.front()->type, // Return type
            method_node->lineno
        };

        if (method_node->children.size() > 1) {
            auto it = std::next(method_node->children.begin(), 1);
            Node* params_node = (it != method_node->children.end()) ? *it : nullptr;
            for (auto param : params_node->children){
                method_sym.param_types.push_back(param->children.front()->type);
                //handle_parameter(param, method_sym);
            }
        }


        if (!symtab.add_symbol(method_sym)){
            std::cerr << "Duplicate method declaration at line " << method_node->lineno << std::endl;
            return;
        }
        symtab.enter_scope(method_node->value);

        // wild guess:
        for (auto param_node : method_node->children){
            handle_parameter(param_node, method_sym);
        }

        symtab.add_symbol(method_sym);
    }
};

