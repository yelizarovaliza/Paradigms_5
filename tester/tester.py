import subprocess

def run_cpp_program(input_data):
    process = subprocess.Popen(
        [r'C:\Users\User\Documents\GitHub\Paradigms_5\ConsoleApplication5\Debug\ConsoleApplication5.exe'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    stdout, stderr = process.communicate(input_data)
    
    output_lines = stdout.strip().split("\n")
    filtered_output_lines = [line for line in output_lines if not line.startswith("Enter expression (or type 'exit' to quit):")]
    filtered_output = "\n".join(filtered_output_lines).strip()
    
    if "Error" in filtered_output:
        stderr = filtered_output
    
    return filtered_output, stderr.strip()

def test_cpp_program():
    tests = [
        {
            "input": "var a = 5\na + 10\n",
            "expected_output": "Result: 15"
        },
        {
            "input": "def myfunc(a, b) { min(a, b) + max(a, b) }\nmyfunc(3, 4)\nexit\n",
            "expected_output": "Result: 7"
        },
    ]
    for test in tests:
        input_data = test["input"]
        expected_output = test["expected_output"]
        output, error = run_cpp_program(input_data)
        if error:
            print(f"Test failed for input:\n{input_data}")
            print(f"Error:\n{error}")
        elif output.strip() != expected_output:
            print(f"Test failed for input:\n{input_data}")
            print(f"Expected:\n{expected_output}")
            print(f"Got:\n{output}")
        else:
            print(f"Test passed for input:\n{input_data}")

if __name__ == "__main__":
    test_cpp_program()
