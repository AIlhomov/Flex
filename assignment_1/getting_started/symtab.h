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
    //vector<string> param_types; /* for methods (parameters) */
    //int dimension = 0; /* 0 for primitive/scalar, 1 for 1D arrays and so on.. */
    /* list<int> line_of_usage; store a linked list of line numbers where the symbol is used (line_of_usage) scopes? */
    
    /* string adress; */
};


class Scope {
public: /* Public makes "parent" accessible */
    string name; /* "Global", "Classname", "MethodName" */
    unordered_map<string, Symbol> symbols; /* all symbols in current scope */
    Scope* parent; 

public:
    Scope() : name("Global"), parent(nullptr) {}
    Scope(const string& name, Scope* p = nullptr) : name(name), parent(p) {}

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
        if (it != symbols.end()) return &it->second;
        if (parent) return parent->lookup(name);
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
        current_scope = new Scope(name, current_scope);
    }
    void exit_scope(){
        if (current_scope == global_scope) return;
        Scope* temp = current_scope;
        current_scope = current_scope->parent;
        delete temp;
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

    string writeAllSymbols(){
        string res;
        Scope* scope = current_scope;
        while (scope) {
            for (const auto& entry : scope->symbols) {
                res += entry.first + " ";
            }
            scope = scope->parent;
        }
        return res;
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