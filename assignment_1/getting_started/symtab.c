#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <string>

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

/* functions to be used: 

    always:
    insert()
    lookup()

    for scopes: 
    set()
    reset()
*/

