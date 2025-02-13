# include "Node.h"
# include "symtab.h"

class ASTVisitor {
private:
    SymbolTable &symtab;

public:
    ASTVisitor(SymbolTable &st) : symtab(st) {}

    void visit(Node* node){
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
        Symbol var_sym{
            var_node->value,  // Variable name
            VARIABLE,
            var_node->children.front()->value,  // Type
            var_node->lineno
        };
        
        symtab.add_symbol(var_sym);
    }
    
};

