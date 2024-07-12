#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <stack>
#include <queue>

using namespace std;

// Tokenizer
class Tokenizer {
public:
    static vector<string> Tokenization() {
        vector<string> result;
        string expression;

        // Input the expression
        cout << "Enter expression: ";
        getline(cin, expression);

        // Buffer for constructing tokens
        string buffer;
        bool lastWasOperator = true;

        for (char c : expression) {
            if (isdigit(c) || c == '.') {
                buffer.push_back(c);
                lastWasOperator = false;
            }
            else {
                if (!buffer.empty()) {
                    result.push_back(buffer);
                    buffer.clear();
                }
                if (c == '-' && (lastWasOperator || result.empty())) {
                    // Handle negative numbers
                    buffer.push_back(c);
                }
                else {
                    if (!isspace(c)) {
                        result.push_back(string(1, c));
                    }
                    lastWasOperator = (c != ')');
                }
            }
        }
        if (!buffer.empty()) {
            result.push_back(buffer);
        }

        return result;
    }
    static bool IsFunction(string func) {
        transform(func.begin(), func.end(), func.begin(), ::tolower);
        return func == "pow" || func == "abs" || func == "min" || func == "max" || func == "var";
    }
};


// ShuntingYard
class ShuntingYard {
public:
    static void Main() {
        string postfixExpression = ConvertToPostfix();
        cout << "Postfix: " << postfixExpression << endl;

        double result = EvaluatePostfix(postfixExpression);
        cout << "Result: " << result << endl;
    }

private:
    static string ConvertToPostfix() {
        unordered_map<string, int> precedence = {
            {"pow", 3}, {"abs", 3}, {"max", 3}, {"min", 3},
            {"*", 2}, {"/", 2},
            {"+", 1}, {"-", 1},
            {"(", 0}
        };

        stack<string> operatorStack;
        queue<string> outputQueue;

        vector<string> tokens = Tokenizer::Tokenization();

        cout << "Tokenization: ";
        for (const auto& token : tokens) {
            cout << token << ", ";
        }
        cout << endl;

        for (const auto& token : tokens) {
            string func;
            if (isalpha(token[0])) {
                func = token;
                while (!operatorStack.empty() && operatorStack.top() == "(") {
                    func += operatorStack.top();
                    operatorStack.pop();
                }
                if (Tokenizer::IsFunction(func)) {
                    operatorStack.push(func);
                }
                else {
                    throw runtime_error("Invalid function: " + func);
                }
            }
            else if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-' && isdigit(token[1]))) {
                outputQueue.push(token);
            }
            else if (token == "(") {
                operatorStack.push(token);
            }
            else if (token == ")") {
                while (!operatorStack.empty() && operatorStack.top() != "(") {
                    outputQueue.push(operatorStack.top());
                    operatorStack.pop();
                }
                operatorStack.pop();
            }
            else {
                while (!operatorStack.empty() && precedence[token] <= precedence[operatorStack.top()]) {
                    outputQueue.push(operatorStack.top());
                    operatorStack.pop();
                }
                operatorStack.push(token);
            }
        }

        while (!operatorStack.empty()) {
            if (operatorStack.top() == "(" || operatorStack.top() == ")") {
                throw runtime_error("Mismatched parentheses in the expression");
            }
            outputQueue.push(operatorStack.top());
            operatorStack.pop();
        }

        string result;
        while (!outputQueue.empty()) {
            result += outputQueue.front() + " ";
            outputQueue.pop();
        }
        return result;
    }

    static double EvaluatePostfix(const string& postfixExpression) {
        stack<double> operandStack;

        istringstream iss(postfixExpression);
        string token;

        while (iss >> token) {
            if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-' && isdigit(token[1]))) {
                operandStack.push(stod(token));
            }
            else if (Tokenizer::IsFunction(token)) {
                double operand = operandStack.top();
                operandStack.pop();
                double result = EvaluateFunction(token, operand);
                operandStack.push(result);
            }
            else {
                double operand2 = operandStack.top();
                operandStack.pop();
                double operand1 = operandStack.top();
                operandStack.pop();

                if (token == "+") operandStack.push(operand1 + operand2);
                else if (token == "-") operandStack.push(operand1 - operand2);
                else if (token == "*") operandStack.push(operand1 * operand2);
                else if (token == "/") operandStack.push(operand1 / operand2);
                else if (token == "pow") operandStack.push(pow(operand1, operand2));
                else if (token == "abs") operandStack.push(abs(operand1));
                else if (token == "min") operandStack.push(min(operand1, operand2));
                else if (token == "max") operandStack.push(max(operand1, operand2));
                else throw runtime_error("Invalid operator or function: " + token);
            }
        }
        

        return operandStack.top();
    }
    static double EvaluateFunction(const string & functionName, double operand) {
        string lowerFunctionName = functionName;
        transform(lowerFunctionName.begin(), lowerFunctionName.end(), lowerFunctionName.begin(), ::tolower);

        if (lowerFunctionName == "abs") return abs(operand);
        else throw runtime_error("Invalid function: " + functionName);
    }
};

int main() {
    try {
        ShuntingYard::Main();
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    return 0;
}
