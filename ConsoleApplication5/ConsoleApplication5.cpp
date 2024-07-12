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

        for (size_t i = 0; i < expression.size(); ++i) {
            char c = expression[i];
            if (isdigit(c) || c == '.') {
                buffer.push_back(c);
                lastWasOperator = false;
            }
            else if (isalpha(c)) {
                buffer.push_back(c);
                while (i + 1 < expression.size() && isalpha(expression[i + 1])) {
                    buffer.push_back(expression[++i]);
                }
                result.push_back(buffer);
                buffer.clear();
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

    static bool IsFunction(const string& func) {
        string lowerFunc = func;
        transform(lowerFunc.begin(), lowerFunc.end(), lowerFunc.begin(), ::tolower);
        return lowerFunc == "pow" || lowerFunc == "abs" || lowerFunc == "min" || lowerFunc == "max";
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
            {"pow", 4}, {"abs", 4}, {"max", 4}, {"min", 4},
            {"*", 3}, {"/", 3},
            {"+", 2}, {"-", 2},
            {"(", 1}
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
            if (isalpha(token[0])) {
                if (Tokenizer::IsFunction(token)) {
                    operatorStack.push(token);
                }
                else {
                    throw runtime_error("Invalid function: " + token);
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
                if (!operatorStack.empty() && operatorStack.top() == "(") {
                    operatorStack.pop();
                }
                if (!operatorStack.empty() && Tokenizer::IsFunction(operatorStack.top())) {
                    outputQueue.push(operatorStack.top());
                    operatorStack.pop();
                }
            }
            else if (token == ",") {
                while (!operatorStack.empty() && operatorStack.top() != "(") {
                    outputQueue.push(operatorStack.top());
                    operatorStack.pop();
                }
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
                double operand1 = operandStack.top();
                operandStack.pop();

                if (token == "abs") {
                    operandStack.push(abs(operand1));
                }
                else {
                    double operand2 = operandStack.top();
                    operandStack.pop();

                    if (token == "min") operandStack.push(min(operand1, operand2));
                    else if (token == "max") operandStack.push(max(operand1, operand2));
                    else if (token == "pow") operandStack.push(pow(operand2, operand1));
                }
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
                else throw runtime_error("Invalid operator: " + token);
            }
        }

        return operandStack.top();
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
