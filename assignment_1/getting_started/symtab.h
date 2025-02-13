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

enum SymbolKind { CLASS, METHOD, VARIABLE }; 

struct Symbol {
    string name;
    SymbolKind kind;
    string type; /* Return type for methods, data type for variables */

    int dimension = 0; /* 0 for primitive/scalar, 1 for 1D arrays and so on.. */
    int line_no; /* line of declaration */
    vector<string> param_types; /* for methods (parameters) */

    Symbol* parent_scope; /* scope hierarchy */

    /* list<int> line_of_usage; store a linked list of line numbers where the symbol is used (line_of_usage) scopes? */
    
    /* string adress; */
};

class Scope {
public:
    string name; /* "Global", "Classname", "MethodName" */
    unordered_map<string, Symbol> symbols;
    Scope* parent;

public:
    Scope() : name("Global"), parent(nullptr) {}
    Scope(const string& name, Scope* p = nullptr) : name(name), parent(p) {}

    bool add_symbol(const Symbol& sym);
    Symbol* lookup(const string& name);
    const string& get_name() const;
};

class SymbolTable {
private:
    Scope* current_scope;
    Scope* global_scope;
    int error_count = 0;

public:
    SymbolTable();
    void enter_scope(const string& name);
    void exit_scope();
    bool add_symbol(const Symbol& sym);
    Symbol* lookup(const string& name);
    int get_error_count() const;
};

#endif // SYMTAB_H