#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <string>

using namespace std;

struct symbol {
    string name;
    string type;
    int size; /* int = 2 bytes ? char = 1 byte ?*/
    int dimension = 0; /* 0 for primitive/scalar, 1 for 1D arrays and so on.. */
    int line_no; /* line of declaration */

    /* list<int> line_of_usage; store a linked list of line numbers where the symbol is used (line_of_usage) */
    
    string adress;
};

/* functions to be used: 

    always:
    insert()
    lookup()

    for scopes: 
    set()
    reset()
*/

