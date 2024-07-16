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

unordered_map<string, double> variableMap;
unordered_map<string, pair<vector<string>, string>> functionMap;

// Tokenizer
class Tokenizer {
public:
    static vector<string> Tokenization(const string& expression) {
        vector<string> result;
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
                // Handle negative numbers
                if (c == '-' && (lastWasOperator || result.empty())) {
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
        return lowerFunc == "pow" || lowerFunc == "abs" || lowerFunc == "min" || lowerFunc == "max" || functionMap.find(lowerFunc) != functionMap.end();
    }
};

// ShuntingYard
class ShuntingYard {
public:
    static string ConvertToPostfix(const vector<string>& tokens) {
        unordered_map<string, int> precedence = {
            {"pow", 4}, {"abs", 4}, {"max", 4}, {"min", 4},
            {"*", 3}, {"/", 3},
            {"+", 2}, {"-", 2},
            {"(", 1}
        };

        stack<string> operatorStack;
        queue<string> outputQueue;

        /*cout << "Tokenization: ";
        for (const auto& token : tokens) {
            cout << token << ", ";
        }
        cout << endl;*/

        for (const auto& token : tokens) {
            if (isalpha(token[0])) {
                if (Tokenizer::IsFunction(token)) {
                    operatorStack.push(token);
                }
                else {
                    if (variableMap.find(token) == variableMap.end()) {
                        throw runtime_error("Variable not found: " + token);
                    }
                    else {
                        outputQueue.push(to_string(variableMap[token]));
                    }
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
                if (functionMap.find(token) != functionMap.end()) {
                    auto func = functionMap[token];
                    vector<double> args;
                    for (int i = func.first.size() - 1; i >= 0; --i) {
                        if (operandStack.empty()) {
                            throw runtime_error("Insufficient operands for function " + token);
                        }
                        args.push_back(operandStack.top());
                        operandStack.pop();
                    }
                    unordered_map<string, double> localVariables = variableMap;
                    for (size_t i = 0; i < func.first.size(); ++i) {
                        localVariables[func.first[i]] = args[i];
                    }
                    operandStack.push(EvaluateExpressionWithVariables(func.second, localVariables));
                }
                else {
                    if (operandStack.empty()) {
                        throw runtime_error("Insufficient operands for function " + token);
                    }
                    double operand1 = operandStack.top();
                    operandStack.pop();

                    if (token == "abs") {
                        operandStack.push(abs(operand1));
                    }
                    else {
                        if (operandStack.empty()) {
                            throw runtime_error("Insufficient operands for function " + token);
                        }
                        double operand2 = operandStack.top();
                        operandStack.pop();

                        if (token == "min") operandStack.push(min(operand1, operand2));
                        else if (token == "max") operandStack.push(max(operand1, operand2));
                        else if (token == "pow") operandStack.push(pow(operand2, operand1));
                    }
                }
            }
            else {
                if (operandStack.size() < 2) {
                    throw runtime_error("Insufficient operands for operator " + token);
                }
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

        if (operandStack.empty()) {
            throw runtime_error("No result on the operand stack");
        }

        return operandStack.top();
    }

    static double EvaluateExpressionWithVariables(const string& expression, unordered_map<string, double>& localVariables) {
        auto oldVariableMap = variableMap;
        variableMap = localVariables;

        vector<string> tokens = Tokenizer::Tokenization(expression);
        string postfixExpression = ConvertToPostfix(tokens);
        double result = EvaluatePostfix(postfixExpression);

        variableMap = oldVariableMap;

        return result;
    }
};

// variable declarations
void HandleVariableDeclaration(const string& expression) {
    size_t equalSignPos = expression.find('=');
    if (equalSignPos != string::npos) {
        string varName = expression.substr(0, equalSignPos);
        varName.erase(remove_if(varName.begin(), varName.end(), ::isspace), varName.end());
        string varExpression = expression.substr(equalSignPos + 1);
        vector<string> tokens = Tokenizer::Tokenization(varExpression);
        string postfixExpression = ShuntingYard::ConvertToPostfix(tokens);
        double value = ShuntingYard::EvaluatePostfix(postfixExpression);
        variableMap[varName] = value;
        //cout << "Variable " << varName << " = " << value << endl;
    }
    else {
        throw runtime_error("Invalid variable declaration");
    }
}

// custom function definitions
void HandleFunctionDefinition(const string& expression) {
    size_t defPos = expression.find("def ");
    if (defPos != string::npos) {
        string funcDeclaration = expression.substr(defPos + 4);
        size_t openParenPos = funcDeclaration.find('(');
        size_t closeParenPos = funcDeclaration.find(')');
        size_t openBracePos = funcDeclaration.find('{');
        size_t closeBracePos = funcDeclaration.find('}');

        if (openParenPos == string::npos || closeParenPos == string::npos || openBracePos == string::npos || closeBracePos == string::npos) {
            throw runtime_error("Invalid function definition syntax");
        }

        string funcName = funcDeclaration.substr(0, openParenPos);
        funcName.erase(remove_if(funcName.begin(), funcName.end(), ::isspace), funcName.end());

        string paramList = funcDeclaration.substr(openParenPos + 1, closeParenPos - openParenPos - 1);
        string funcBody = funcDeclaration.substr(openBracePos + 1, closeBracePos - openBracePos - 1);

        vector<string> params;
        stringstream paramStream(paramList);
        string param;
        while (getline(paramStream, param, ',')) {
            param.erase(remove_if(param.begin(), param.end(), ::isspace), param.end());
            if (!param.empty()) {
                params.push_back(param);
            }
        }

        functionMap[funcName] = make_pair(params, funcBody);
        //cout << "Function " << funcName << " defined" << endl;
    }
    else {
        throw runtime_error("Invalid function definition");
    }
}

int main() {
    try {
        string input;
        while (true) {
            string input;
            // cout << "Enter expression (or type 'exit' to quit): ";
            getline(cin, input);
            if (input == "exit") {
                break;
            }
            if (input.find("var ") == 0) {
                HandleVariableDeclaration(input.substr(4));
            }
            else if (input.find("def ") == 0) {
                HandleFunctionDefinition(input);
            }
            else {
                vector<string> tokens = Tokenizer::Tokenization(input);
                string postfixExpression = ShuntingYard::ConvertToPostfix(tokens);
                double result = ShuntingYard::EvaluatePostfix(postfixExpression);
                cout << "Result: " << result << endl;
            }
        }
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    return 0;
}
