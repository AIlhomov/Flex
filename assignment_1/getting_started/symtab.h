#ifndef SYMTAB_H
#define SYMTAB_H

#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <unordered_map> // hash table with NO SORTING (faster) (better)


using namespace std;

enum SymbolKind { CLASS, METHOD, VARIABLE, PARAMETER }; 

struct Symbol {
    string name;
    SymbolKind kind;
    string type; /* Return type for methods, data type for variables */
    int line_no; /* line of declaration */



    //Symbol* parent_scope; /* scope hierarchy */
    vector<string> param_types; /* for methods (parameters) */
    //int dimension = 0; /* 0 for primitive/scalar, 1 for 1D arrays and so on.. */
    /* list<int> line_of_usage; store a linked list of line numbers where the symbol is used (line_of_usage) scopes? */
    
    /* string adress; */
};


class Scope {
public: /* Public makes "parent" accessible */
    string name; /* "Global", "Classname", "MethodName" */
    unordered_map<string, Symbol> symbols; /* all symbols in current scope */
    Scope* parent; 
    vector<Scope*> children;

public:
    Scope() : name("Global"), parent(nullptr) {}
    Scope(const string& name, Scope* p = nullptr) : name(name), parent(p) {
        if (parent)
        parent->children.push_back(this);
    }

    bool add_symbol(const Symbol& sym){
        // if (symbols.find(sym.name) != symbols.end()) return false; /* duplicate */
        // symbols[sym.name] = sym;
        // return true;
        // Check for duplicates: same name AND same kind in current scope
        for (const auto& entry : symbols) {
            if (entry.second.name == sym.name && entry.second.kind == sym.kind) {
                return false; // Duplicate name+kind
            }
            if ((entry.second.kind == VARIABLE && sym.kind == PARAMETER && entry.second.name == sym.name) ||
                (entry.second.kind == PARAMETER && sym.kind == VARIABLE && entry.second.name == sym.name)) {
                return false; // Variable and parameter with same name
            }
        }
        
        symbols[sym.name] = sym; // Allow same name if kinds differ
        return true;
    }

    

    Symbol *lookup(const string& name){
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return &it->second;}
        if (parent) {
                        
            return parent->lookup(name);
        }
        return nullptr;
    }


    const string& get_name() const { return name; }

};

class SymbolTable {
private:
    Scope* global_scope;
public:
    int error_count = 0;
    Scope* current_scope;
public:
    SymbolTable() {
        global_scope = new Scope("Global");
        current_scope = global_scope;
    }
    
    void enter_scope(const string& name){
        for (Scope* child : current_scope->children) {
            if (child->name == name) {
                current_scope = child; // Reuse existing scope
                return;
            }
        }
        
        // If the scope does not exist, create a new one
        Scope* new_scope = new Scope(name, current_scope);
        current_scope = new_scope;
        }

    void exit_scope(){
        if (current_scope == global_scope) {    return;}
        //Scope* temp = current_scope;
        current_scope = current_scope->parent;
        //delete temp;
    }

    bool add_symbol(const Symbol& sym){
        if (!current_scope->add_symbol(sym)){

            // cerr << "@error at line " << sym.line_no << ": semantic (Already Declared " << sym.kind
            // << ": \'" << sym.name << "\')\n";

            //DEBUG:
            cerr << "\n@error at line " << sym.line_no << ": Duplicate symbol " << sym.name 
            << " in scope " << current_scope->get_name() << endl 
            << "EXTRA: type: " << sym.type << " kind: "<< sym.kind 
            << " " << endl;
            
            error_count++;
            return false;
        }
        return true;
    }

 
    Symbol* lookup(const string& name){
        return current_scope->lookup(name);
    }

    int get_error_count() const { return error_count; }



    
    void writeAllSymbolsHelper(Scope* scope, string indent, string& res) {
        if (!scope) return;
        
        res += indent + "Scope: " + scope->name + " -> ";
        
        for (const auto& entry : scope->symbols) {
            res += entry.first + " ";
        }
        res += "\n";
        
        for (Scope* child : scope->children) {
            writeAllSymbolsHelper(child, indent + "  ", res);
        }
    }
    
    string writeAllSymbols() {
        string res;
        writeAllSymbolsHelper(global_scope, "", res);
        return res;
    }
    Scope* get_class_scope(const string& name) {
        for (Scope* child : global_scope->children) {
            if (child->name == name) {
                return child;
            }
        }
        return nullptr;
    }
    Scope* get_method_scope(const string& class_name, const string& method_name) {
        Scope* class_scope = get_class_scope(class_name); // Get class scope
        if (class_scope) {
            for (Scope* child : class_scope->children) {
                if (child->name == method_name) {
                    return child; // Return method scope
                }
            }
        }
        return nullptr;
    }
    
    Scope* get_parameter_scope(const string& class_name, const string& method_name, const string& param_name) {
        Scope* method_scope = get_method_scope(class_name, method_name); // Get method scope
        
        if (method_scope) {
            
            Symbol* param_sym = method_scope->lookup(param_name);
            if (param_sym && param_sym->kind == PARAMETER) {
                return method_scope; // Or return param_sym->type directly
            }
        }
        return nullptr;
    }
   
};


/* functions to be used: 

    always:
    insert()
    lookup()

    for scopes: 
    set()
    reset()
*/

    

#endif // SYMTAB_H