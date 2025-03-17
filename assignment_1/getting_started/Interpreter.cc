#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>

std::unordered_map<std::string, int> labelMap;

using namespace std;

void Interpreter::interpret(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            auto instruction = parseInstruction(line);
            if (instruction[0] == "label") {
                labelMap[instruction[1]] = lineNumber; // Map label to line number
            }
            instructions.push_back(instruction);
            lineNumber++;
        }
    }
    inFile.close();

    while (programCounter < instructions.size()) {
        executeInstruction(instructions[programCounter]);
        programCounter++;
    }
}

std::vector<std::string> Interpreter::parseInstruction(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
/*
    E.java
    tests:
    iload
    iadd, isub, imul
    istore
    print
    inot, iand, ior
    ieq, igt, ilt

    A.java
    tests:
    new object

    // Main method
    new Bar           // Create a new Bar object
    istore __t0       // Store the Bar object in temporary variable __t0

    iload __t0        // Load the Bar object reference
    invoke Bar.foo    // Call the foo method on the Bar object
    istore __t1       // Store the result of foo in temporary variable __t1

    iload __t1        // Load the result of foo
    print             // Print the result
    exit              // Exit the program

    // Bar.foo method
    label Bar.foo
    iconst 1          // Load constant 1
    istore aux        // Store it in aux
    iconst 1          // Load constant true (1)
    istore aux2       // Store it in aux2
    iload aux         // Load aux
    invoke Bar.foo2   // Call foo2 with aux as the argument
    istore aux        // Store the result back in aux
    iconst 2          // Load constant 2
    iconst 5          // Load constant 5
    iload aux2        // Load aux2
    invoke Bar.foo3   // Call foo3 with 2, 5, and aux2 as arguments
    istore aux        // Store the result back in aux
    iload aux         // Load aux
    return            // Return aux


    // more simplified:
    invokevirtual Bar.foo   ; Call foo() directly (assuming Bar instance exists)
    istore __t0             ; Store return value of foo()
    iload __t0              ; Load it for printing
    print                   ; Print the result
    stop                    ; End program

    label Bar.foo
    iconst 1                ; int aux = 1;
    istore aux

    iconst 1                ; boolean aux2 = true; (true is iconst 1)
    istore aux2

    iload aux               ; Load aux
    invokevirtual Bar.foo2  ; Call foo2(aux)
    istore aux              ; Store return value in aux

    iconst 2                ; First argument for foo3
    iconst 5                ; Second argument for foo3
    iload aux2              ; Load boolean aux2
    invokevirtual Bar.foo3  ; Call foo3(2, 5, aux2)
    istore aux              ; Store return value in aux

    iload aux               ; Load return value
    ireturn                 ; Return from foo()

    // Bar.foo2 method
    label Bar.foo2
    iload p1            ; Load p1
    iconst 2           ; Load constant 2
    ilt               ; Compare (p1 < 2), push 1 if true, 0 otherwise
    iffalse goto else_foo2  ; If false, jump to else branch

    ; Then branch (p1 < 2)
    iconst 10          ; Push 10
    print             ; Print 10
    goto end_foo2     ; Jump to end of function

    ; Else branch (p1 >= 2)
    label else_foo2
    iconst 0           ; Push 0
    print             ; Print 0

    ; Return statement
    label end_foo2
    iconst 1          ; Return value 1
    ireturn

*/

void Interpreter::executeInstruction(const std::vector<std::string>& instruction) {
    if (instruction.empty()) return;

    const std::string& opcode = instruction[0];

    if (opcode == "iload") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing operand for iload\n";
            return;
        }
        const std::string& operand = instruction[1];

        // Check if the operand is a constant integer
        if (isdigit(operand[0]) || (operand[0] == '-' && isdigit(operand[1]))) {
            dataStack.push(std::stoi(operand)); // Push the constant onto the stack
        } 
        // Check if the operand is a boolean constant (true/false)
        else if (operand == "true" || operand == "True") {
            dataStack.push(1); // Push 1 for true
        } else if (operand == "false" || operand == "False") {
            dataStack.push(0); // Push 0 for false
        } 
        else if (variables.find(operand) != variables.end()) {
            // Operand is a variable
            dataStack.push(variables[operand]);
        } else {
            std::cerr << "Error: Variable " << operand << " used before assignment.\n";
        }
    } 
    else if (opcode == "iadd" || opcode == "isub" || opcode == "imul") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in " << opcode << "\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        if (opcode == "iadd") dataStack.push(a + b);
        else if (opcode == "isub") dataStack.push(a - b);
        else if (opcode == "imul") dataStack.push(a * b);
    }
    else if (opcode == "istore") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing variable or field name for istore\n";
            return;
        }
        if (dataStack.empty()) {
            std::cerr << "Error: Stack underflow in istore\n";
            return;
        }
        const std::string& target = instruction[1];
        int value = dataStack.top();
        dataStack.pop();

        // Check if the target is an object field (e.g., objectName.fieldName)
        size_t dotPos = target.find('.');
        if (dotPos != std::string::npos) {
            std::string objectName = target.substr(0, dotPos);
            std::string fieldName = target.substr(dotPos + 1);
            if (objects.find(objectName) != objects.end()) {
                objects[objectName]->fields[fieldName] = value;
            } else {
                std::cerr << "Error: Object " << objectName << " not found\n";
            }
        } else {
            // Store in a variable
            variables[target] = value;
        }
    }  
    else if (opcode == "print") {
        if (dataStack.empty()) {
            std::cerr << "Error: Stack underflow in print\n";
        } else {
            
            std::cout << dataStack.top() << std::endl;
            dataStack.pop();
        }
    }
    else if (opcode == "inot") {
        if (dataStack.empty()) {
            std::cerr << "Error: Stack underflow in inot\n";
            return;
        }
        int value = dataStack.top();
        dataStack.pop();
        dataStack.push(!value);
    } else if (opcode == "iand") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in iand\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a && b);
    } else if (opcode == "ior") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in ior\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a || b);
    } else if (opcode == "ieq") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in ieq\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a == b);
    } else if (opcode == "igt") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in igt\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a > b);
    } else if (opcode == "ilt") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in ilt\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a < b);
    } else if (opcode == "equal") {
        if (dataStack.size() < 2) {
            std::cerr << "Error: Stack underflow in iequal\n";
            return;
        }
        int b = dataStack.top(); dataStack.pop();
        int a = dataStack.top(); dataStack.pop();
        dataStack.push(a == b);
    }
    else if (opcode == "iconst") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing constant value for iconst\n";
            return;
        }
        int value = std::stoi(instruction[1]); // Convert the constant to an integer
        dataStack.push(value); // Push the constant onto the stack
    }
    else if (opcode == "new") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing class name for new\n";
            return;
        }
        const std::string& className = instruction[1];
        objects[className] = std::make_shared<Object>();
        dataStack.push(1); // Push a reference value onto the stack
    }
    else if (opcode == "invokevirtual") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing method label for invoke\n";
            return;
        }
        const std::string& methodLabel = instruction[1];
        callStack.push(programCounter); // Save the current program counter
        auto it = labelMap.find(methodLabel);
        if (it != labelMap.end()) {
            programCounter = it->second - 1; // Jump to the method label
        } else {
            std::cerr << "Error: Method label " << methodLabel << " not found\n";
        }
    }
    else if (opcode == "dup") {
        if (dataStack.empty()) {
            std::cerr << "Error: Stack underflow in dup\n";
            return;
        }
        int topValue = dataStack.top(); // Get the top value of the stack
        dataStack.push(topValue);      // Push the duplicate value onto the stack
    }
    else if (opcode == "aload"){
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing variable or field name for aload\n";
            return;
        }
        const std::string& target = instruction[1];
        if (variables.find(target) != variables.end()) {
            dataStack.push(variables[target]);
        } else {
            std::cerr << "Error: Variable " << target << " used before assignment.\n";
        }
    }

    else if (opcode == "label") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing method name for ENTRY\n";
            return;
        }
        const std::string& methodName = instruction[1];
        // Acknowledge the method entry (no action needed)
        return;
    }
    else if (opcode == "exit") {
        exit(0);
    }   else if (opcode == "return") {
        if (dataStack.empty()) {
            std::cerr << "Error: Stack underflow in return\n";
            return;
        }
        int returnValue = dataStack.top();
        dataStack.pop();

        if (callStack.empty()) {
            std::cerr << "Error: Call stack underflow in return\n";
            return;
        }
        programCounter = callStack.top(); // Restore the return address
        callStack.pop();

        // Push the return value onto the stack for the caller
        dataStack.push(returnValue);
    } else if (opcode == "class") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing class name for class\n";
            return;
        }
        const std::string& className = instruction[1];
        // Acknowledge the class definition (no action needed)
    } else if (opcode == "iffalse") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing label for iffalse\n";
            return;
        }
        if (dataStack.empty()) {
            std::cerr << "Error: Stack underflow in iffalse\n";
            return;
        }
        int value = dataStack.top();
        dataStack.pop();
        if (value == 0) {
            const std::string& label = instruction[1];
            auto it = labelMap.find(label);
            if (it != labelMap.end()) {
                programCounter = it->second - 1; // Jump to the label
            } else {
                std::cerr << "Error: Label " << label << " not found\n";
                return;
            }
        }
    } else if (opcode == "goto") {
        if (instruction.size() < 2) {
            std::cerr << "Error: Missing label for goto\n";
            return;
        }
        const std::string& label = instruction[1];
        auto it = labelMap.find(label);
        if (it != labelMap.end()) {
            programCounter = it->second - 1; // Jump to the label
        } else {
            std::cerr << "Error: Label " << label << " not found\n";
            return;
        }
    }  else {
        std::cerr << "Unknown opcode: " << opcode << std::endl;
    }
}