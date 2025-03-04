#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

//enum class OpCode { ADD, SUB, MOV, LOAD, STORE };

class Tac {
public:
    string op;
    vector<string> lhs, rhs, result;
    

    void printAll(){
        printf("%s := %s %s %s\n",result , lhs, op, rhs);
    }

    void addToTac(){

    }

    Tac(string o, string l, string r, string res){

    }

};

class Block : public Tac{
public:
    string name; // UNIQUE
    vector<Tac> tacInstruction;
    
    Tac condition;
    Block *trueExit, *falseExit;

    //Block() : trueExit(NULL), falseExit(NULL) {}


};
