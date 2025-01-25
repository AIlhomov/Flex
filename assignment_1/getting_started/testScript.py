import glob
import os
import re
import subprocess
import sys
import filecmp

# Global file ID counter
global_file_id = 1

# ANSI escape codes for colored output
class Colors:
    RED = '\033[91m'
    GREEN = '\033[92m'
    DARK_GREEN = '\033[32m'  # Dark Green
    ORANGE = '\033[93m'      # Orange
    END = '\033[0m'

def colored(text, color):
    return f"{color}{text}{Colors.END}"

def run_compiler(file_path):
    process = subprocess.Popen(['./compiler', file_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    return stdout.decode(), stderr.decode()

def run_interpreter(file_path = ""):
    if file_path == "":
        process = subprocess.Popen(['./interpreter'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    else:
        process = subprocess.Popen(['java', file_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    stdout, stderr = process.communicate()
    return stdout.decode(), stderr.decode()

def extract_expected_errors(file_path):
    expected_errors = {}
    error_pattern = re.compile(r'//\s*@error - (.*)')

    with open(file_path, 'r') as file:
        for line_number, line in enumerate(file, 1):
            match = error_pattern.search(line)
            if match:
                expected_errors[line_number] = match.group(1).strip()

    return expected_errors

def parse_compiler_errors(compiler_output):
    error_pattern = re.compile(r'@error at line (\d+). (.*)')
    errors = {}
    for match in error_pattern.finditer(compiler_output):
        line_number = int(match.group(1))
        message = match.group(2).strip()
        errors[line_number] = message
    return errors

def print_test_summary(id, file_name, success):
    status = colored("Success", Colors.DARK_GREEN) if success else colored("Fail", Colors.RED)
    print(f"    {id}: {file_name} - {status}")

def display_summary(file_details):
    print(colored("\nTest Summary:", Colors.GREEN))

    # Group files by test type
    grouped_files = {}
    for file_id, details in file_details.items():
        test_type = details['test_type']
        if test_type not in grouped_files:
            grouped_files[test_type] = []
        grouped_files[test_type].append((file_id, details))

    # Display summary for each group
    for test_type, files in grouped_files.items():
        print(colored(f"\n{test_type.title()} Test Results:", Colors.GREEN))
        for file_id, details in files:
            expected_lines = set(details['expected_errors'].keys())
            compiler_error_lines = set(details['compiler_errors'].keys())
            success = expected_lines == compiler_error_lines
            print_test_summary(file_id, details['file_name'], success)

def run_test_files(folder_path, test_type, file_details, global_id):
    print(colored(f"\nRunning {test_type} test classes...", Colors.GREEN))
    for file in os.listdir(folder_path):
        if file.endswith('.java'):
            file_path = os.path.join(folder_path, file)
            expected_errors = extract_expected_errors(file_path)
            stdout, stderr = run_compiler(file_path)
            compiler_errors = parse_compiler_errors(stderr)

            # Check if all expected errors match the compiler-reported errors
            expected_lines = set(expected_errors.keys())
            compiler_error_lines = set(compiler_errors.keys())
            success = expected_lines == compiler_error_lines
            
            # Store file details including the test type
            file_details[global_id] = {
                'file_name': file, 
                'test_type': test_type,  # Store the test type here
                'stdout': stdout, 
                'stderr': stderr, 
                'expected_errors': expected_errors, 
                'compiler_errors': compiler_errors
            }
            print_test_summary(global_id, file, success)
            global_id += 1

    return global_id


def display_file_details(file_id, file_details, output_type=None):
    if file_id in file_details:
        details = file_details[file_id]
        print(colored(f"\nDetails for {details['file_name']}:", Colors.GREEN))
        
        if output_type == "raw" or (output_type is None and ('expected_errors' in details and  not details['expected_errors']) and ('compiler_errors' in details and not details['compiler_errors'])):
            print("Standard Output:\n" + details['stdout'])
            if(details['stderr']):
                print("Standard Error:\n" + details['stderr'])

        expected_errors_exist = bool('expected_errors' in details and details['expected_errors'])
        unexpected_errors_exist = 'compiler_errors' in details and any(line not in details['expected_errors'] for line in details['compiler_errors'])

        if output_type == "expected" or (expected_errors_exist and not output_type):
            if expected_errors_exist:
                print("\nExpected Errors (Lines indicated with //@error in the test files):")
                for line_number, error_message in details['expected_errors'].items():
                    print(colored(f"    Line {line_number}: {error_message}", Colors.RED))
            else: 
                if 'expected' not in details:
                    print(colored("\n No Expected Errors Exist!", Colors.DARK_GREEN))

        if output_type == "unexpected" or (unexpected_errors_exist and not output_type):
            if unexpected_errors_exist:
                print("\nUnexpected Compiler Errors (errors not annotated with //@error):")
                for line_number, error_message in details['compiler_errors'].items():
                    if line_number not in details['expected_errors']:
                        print(colored(f"    Line {line_number}: {error_message}", Colors.ORANGE))
            else: 
                print(colored("\n No Unexpected Compiler Errors Exist!", Colors.DARK_GREEN))
        
        # Check if the expected result matches the provided one
        # print("hello")
        # print(details['expected'])
        if 'expected' in details and (details['expected'] in details['stdout'] or details['stdout'] in details['expected'] or details['stdout'] == details['expected']):
            print(colored(details['expected'], Colors.DARK_GREEN))
        elif 'expected' in details:
            print(colored(details['expected'], Colors.RED))
    else:
        print(colored("Invalid file ID.", Colors.RED))


def help_message():
    print("Commands:")
    print("  <id> [output_type] - Display detailed results for the specified file ID. Optional output types: raw, expected, unexpected.")
    print("  help - Display this help message.")
    print("  exit - Exit the script.")

def run_interpreter_tests(folder_path, test_type, file_details, global_id):

    #check if interpreter exists
    if not os.path.exists('./interpreter'):
        print(colored("Interpreter executable not found. Please compile the interpreter first.", Colors.RED))
        sys.exit(1)

    print(colored(f"\nRunning {test_type} test classes...", Colors.GREEN))
    # Get all files in the assignment3_valid folder
    for file in os.listdir(folder_path):
        if file.endswith('.java'):
            file_path = os.path.join(folder_path, file)
            
            # Run the compiler on the file
            subprocess.run(['./compiler', file_path], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            
            # Run the interpreter on the output.bc file and capture the output
            stdout, stderr = run_interpreter()
            # interpreter_result = subprocess.run(['./interpreter'], capture_output=True, text=True)
            
            # Run the Java compiler on the file
            # subprocess.run(['java', file])
            
            # Run the Java program and capture the output
            # java_program = file.replace('.java', '')
            java_result, compiler_errors = run_interpreter(file_path)
            
            success = stdout == java_result or java_result in stdout or stdout in java_result
            # print(java_result, compiler_errors)
            # Compare the results
            # Store file details including the test type
            file_details[global_id] = {
                'file_name': file, 
                'test_type': test_type,  # Store the test type here
                'stdout': stdout, 
                'stderr': stderr, 
                'expected': java_result, 
            }
            print_test_summary(global_id, file, success)
            global_id += 1

    return global_id

def main():
    if len(sys.argv) < 2:
        print("Usage: python testScript.py [options]")
        print("Options:")
        print("  -lexical       Run tests in the 'test_files/lexical_errors' directory to check for lexical errors.")
        print("  -syntax        Run tests in the 'test_files/syntax_errors' directory to detect syntactic errors.")
        print("  -semantic      Run tests in the 'test_files/semantic_errors' directory to validate semantic correctness.")
        print("  -valid         Run tests in the 'test_files/valid' directory to ensure valid files are processed correctly.")
        print("  -interpreter   Run tests in the 'test_files/assignment3_valid' directory for interpreter-related functionality.")
        print("You can specify multiple options at once to run tests across different categories.")
        sys.exit(1)
    
    # check if file compiler exists
    if not os.path.exists('./compiler'):
        print(colored("Compiler executable not found. Please compile the compiler first.", Colors.RED))
        sys.exit(1)
    
    
    test_types = sys.argv[1:]
    valid_types = {"-lexical": "test_files/lexical_errors", "-syntax": "test_files/syntax_errors", "-semantic": "test_files/semantic_errors", "-valid": "test_files/valid", "-interpreter": "test_files/assignment3_valid"}
    
    global global_file_id
    file_details = {}   
    summary_generated = False

    for test_type in test_types:
        if test_type in valid_types:
            if test_type == "-interpreter":
                global_file_id = run_interpreter_tests(valid_types[test_type], test_type[1:], file_details, global_file_id)
            else:
                global_file_id = run_test_files(valid_types[test_type], test_type[1:], file_details, global_file_id)
            
            summary_generated = True
        else:
            print(colored(f"Invalid test type: {test_type}. Please choose from -lexical, -syntax, -semantic, or -valid.", Colors.RED))

    # Interactive part
    print(colored("\nInteractive mode. Type 'help' for options or 'exit' to quit.", Colors.GREEN))
    while True:
        command = input("\nEnter command: ").strip().split()
        if not command:
            continue
        if command[0] == 'exit':
            break
        elif command[0] == 'help':
            help_message()
        elif command[0] == 'summary' and summary_generated:
            display_summary(file_details)
        elif command[0].isdigit():
            file_id = int(command[0])
            output_type = command[1] if len(command) > 1 else None
            display_file_details(file_id, file_details, output_type)
        else:
            print(colored("Invalid command. Type 'help' for options.", Colors.RED))


if __name__ == "__main__":
    main()
