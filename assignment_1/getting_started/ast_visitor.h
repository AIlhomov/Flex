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

        if (node->type == "var declaration") handle_variable(node);

        for (auto child : node->children) visit(child);
    }

private:
    // void handle_class(Node* class_node){
    //     Symbol cls_sym{
    //         class_node->value, /* class name */
    //         SymbolKind::CLASS,
    //         "class",
    //         class_node->lineno
    //     };

    //     if (!symtab.add_symbol(cls_sym)){
    //         return; // error: class already defined
    //     }

    //     symtab.enter_scope(class_node->value);

    //     // Process class body (fields and methods)
    //     for (auto child : class_node->children){
    //         if (child->type == "methodDeclaration"){
    //             handle_method(child);
    //         }
    //         else if (child->type == "variableDeclaration"){
    //             handle_variable(child);
    //         }
    //     }   

    //     symtab.exit_scope();
    // }

    void handle_variable(Node* var_node) {

        if (var_node->children.size() < 2) { /* we know for a FACT that the pdf file (tree) generated a variable 
            declaration with 2 children under */
            std::cerr << "Malformed variable declaration at line " << var_node->lineno << std::endl;
            return;
        }

        Node* type_node = var_node->children.front(); // nah.
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
    
};

