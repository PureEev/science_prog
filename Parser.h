#pragma once
#include "Calculator_Engine.h"

class Parser {
private:
    std::vector<std::string> Actions = { "sin", "cos", "tg", "ctg", "log", "ln", "+", "-", "/", "*", "^" };

public:
    CalculatorEngine engine;

    template<typename Number>
    void HandleNumber(std::string& NumberStr, std::vector<Number>& StackOfNumbers, bool& isNegative) {
        if (!NumberStr.empty()) {
            Number number = static_cast<Number>(std::stod(NumberStr));
            if (isNegative) {
                number = -number;
                isNegative = false;
            }
            StackOfNumbers.push_back(number);
            NumberStr.clear();
        }
    }

    void HandleAction(const std::string& FunctionStr, std::vector<std::string>& StackOfActions);

    template<typename Number>
    Number ParserExpression(std::string& expression, std::vector<Plugin>& plugins) {
        std::vector<Number> StackOfNumbers;
        std::vector<std::string> StackOfActions;
        std::string NumberStr;
        std::string FunctionStr;
        bool isNegative = false;

        for (size_t i = 0; i < expression.size(); ++i) {
            char CurrentChar = expression[i];

            if (isdigit(CurrentChar) || CurrentChar == '.' || CurrentChar == ',') {
                NumberStr += CurrentChar;
            }
            else if (CurrentChar == '-') {
                if (i == 0 || (i > 0 && (expression[i - 1] == '(' || expression[i - 1] == '*' || expression[i - 1] == '/' || expression[i - 1] == '^'))) {
                    isNegative = true;
                }
                else {
                    HandleNumber<Number>(NumberStr, StackOfNumbers, isNegative);
                    FunctionStr = "-";
                    HandleAction(FunctionStr, StackOfActions);
                }
            }
            else if (CurrentChar != ' ') {
                HandleNumber<Number>(NumberStr, StackOfNumbers, isNegative);
                FunctionStr += CurrentChar;
                HandleAction(FunctionStr, StackOfActions);
                FunctionStr.clear();
            }
        }

        HandleNumber<Number>(NumberStr, StackOfNumbers, isNegative);
        return engine.CalculateExpression(StackOfNumbers, StackOfActions, plugins);
    }

    template<typename Number>
    void ProcessSubExpression(std::string& expression, std::vector<int>& LeftBracketIndex, int i, std::vector<Plugin>& plugins) {
        int CurrentLeftIndex = LeftBracketIndex.back();
        LeftBracketIndex.pop_back();

        std::string subExpression = expression.substr(CurrentLeftIndex + 1, i - CurrentLeftIndex - 1);
        Number CurrentExpression = ParserExpression<Number>(subExpression, plugins);

        expression.erase(CurrentLeftIndex, i - CurrentLeftIndex + 1);
        expression.insert(CurrentLeftIndex, std::to_string(CurrentExpression));
    }

    template<typename Number>
    Number ParserString(std::string& expression, std::vector<Plugin>& plugins) {
        std::vector<int> LeftBracketIndex;

        for (int i = 0; i < expression.size(); ++i) {
            if (expression[i] == '(') {
                LeftBracketIndex.push_back(i);
            }
            else if (expression[i] == ')') {
                ProcessSubExpression<Number>(expression, LeftBracketIndex, i, plugins);
                i = -1;
            }
        }

        return ParserExpression<Number>(expression, plugins);
    }
};