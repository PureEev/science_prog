#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cmath>
#include <filesystem>
#include <windows.h>
#include <stdexcept>
#include <cctype>
#include <functional>

using PluginFunction = std::pair<bool, double>(*)(const char*, double);

struct Plugin {
    HMODULE library;
    PluginFunction function;
};

class Plugins {
public:
    std::vector<Plugin> plugins;

    void loadPlugins(const std::string& directory) {

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.path().extension() == ".dll") {
                HMODULE lib = LoadLibraryA(entry.path().string().c_str());
                if (lib) {
                    PluginFunction func = (PluginFunction)GetProcAddress(lib, "pluginFunc");
                    if (func) {
                        plugins.push_back({ lib, func });
                    }
                    else {
                        std::cerr << "Error function not in " << entry.path().string() << std::endl;
                        FreeLibrary(lib);
                    }
                }
                else {
                    std::cerr << "Error of downloading DLL: " << entry.path().string() << std::endl;
                }
            }
        }
    }

    void unloadPlugins() {
        for (auto& plugin : plugins) {
            FreeLibrary(plugin.library);
        }
    }
};

class CalculatorEngine {
public:
    template<typename Number>
    Number CalculateExpression(std::vector<Number>& numbers, std::vector<std::string>& actions, std::vector<Plugin>& plugins) {

        std::map<std::string, std::function<Number(Number, Number)>> functions = {
            { "+",   [](Number x, Number y) { return x + y; } },
            { "-",   [](Number x, Number y) { return x - y; } },
            { "*",   [](Number x, Number y) { return x * y; } },
            { "/",   [](Number x, Number y) { return (y == 0) ? NAN : (x / y); } },
            { "^",   [](Number x, Number y) { return pow(x,y); } },

        };

        for (size_t i = 0; i < actions.size(); ++i) {
            std::string funcName = actions[i];

            if (funcName == "sin" or funcName == "cos" or funcName == "tg" or funcName == "ctg" or funcName == "log" or funcName == "ln") {
                for (const auto& plugin : plugins) {
                    auto [success, res] = plugin.function(funcName.c_str(), numbers[i]);
                    if (success) {
                        numbers[i] = res;
                        actions.erase(actions.begin() + i);
                        --i;
                        if (std::isnan(res)) {
                            std::cerr << "Function argument: " << funcName << " is in invalid values" << std::endl;
                            exit(1);
                        }
                        break;
                    }
                }
            }

        }

        for (size_t i = 0; i < actions.size(); ++i) {
            if (actions[i] == "*" or actions[i] == "/" or actions[i] == "^") {
                auto funcIt = functions.find(actions[i]);
                if (funcIt != functions.end()) {
                    const auto& func = funcIt->second;
                    numbers[i] = func(numbers[i], numbers[i + 1]); 
                    numbers.erase(numbers.begin() + i + 1);
                    actions.erase(actions.begin() + i);
                    --i;
                }
            }
        }

        for (size_t i = 0; i < actions.size(); ++i) {
            if (actions[i] == "+" or actions[i] == "-") {
                auto funcIt = functions.find(actions[i]);
                if (funcIt != functions.end()) {
                    const auto& func = funcIt->second;
                    numbers[i] = func(numbers[i], numbers[i + 1]);
                    numbers.erase(numbers.begin() + i + 1);
                    actions.erase(actions.begin() + i);
                    --i;
                }
            }
        }

        if (numbers.empty()) {
            std::cerr << "Error: No numbers left in the vector to return." << std::endl;
            throw std::out_of_range("Vector 'numbers' is empty.");
        }

        return numbers[0];
    }
};

class Parser {
public:
    CalculatorEngine engine;

    template<typename Number>
    Number ParserExpression(std::string& expression, std::vector<Plugin>& plugins) {
        std::vector<Number> StackOfNumbers;
        std::vector<std::string> StackOfActions;
        std::vector<std::string> Actions = { "sin", "cos", "tg", "ctg", "log","ln", "+", "-", "/", "*","^" };

        std::string NumberStr;
        std::string FunctionStr;
        bool isNegative = false;

        for (size_t i = 0; i < expression.size(); ++i) {
            char CurrentChar = expression[i];

            if (isdigit(CurrentChar) or CurrentChar == '.' or CurrentChar == ',') {
                NumberStr += CurrentChar;
            }
            else if (CurrentChar == '-') {
                if (i == 0 or (i > 0 && (expression[i - 1] == '(' or expression[i - 1] == '*' or expression[i - 1] == '/' or expression[i - 1] == '^'))) {
                    isNegative = true;
                }
                else {
                    FunctionStr = CurrentChar;
                    for (const auto& action : Actions) {

                        if (action == FunctionStr) {
                            StackOfActions.push_back(action);
                            FunctionStr.clear();
                            break;
                        }
                    }

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
            }
            else {
                if (CurrentChar == ' ') continue;
                FunctionStr += CurrentChar;

                if (!NumberStr.empty()) {
                    Number number = static_cast<Number>(std::stod(NumberStr));
                    if (isNegative) {
                        number = -number;
                        isNegative = false;
                    }
                    StackOfNumbers.push_back(number);
                    NumberStr.clear();
                }

                for (const auto& action : Actions) {
                    if (action == FunctionStr) {
                        StackOfActions.push_back(action);
                        FunctionStr.clear();
                        break;
                    }
                }
            }
        }

        if (!NumberStr.empty()) {
            Number number = static_cast<Number>(std::stod(NumberStr));
            if (isNegative) {
                number = -number;
            }
            StackOfNumbers.push_back(number);
        }

        return engine.CalculateExpression(StackOfNumbers, StackOfActions, plugins);
    }

    template<typename Number>
    Number ParserString(std::string& expression, std::vector<Plugin>& plugins) {
        std::vector<int> LeftBracketIndex;
        int LastIndex = expression.size() - 1;

        for (int i = 0; i < expression.size(); ++i) {
            if (expression[i] == '(') {
                LeftBracketIndex.push_back(i);
            }
            else if (expression[i] == ')') {
                int CurrentLeftIndex = LeftBracketIndex.back();
                LeftBracketIndex.pop_back();

                std::string subExpression = expression.substr(CurrentLeftIndex + 1, i - CurrentLeftIndex - 1);
                Number CurrentExpression = ParserExpression<Number>(subExpression, plugins);

                expression.erase(CurrentLeftIndex, i - CurrentLeftIndex + 1);
                expression.insert(CurrentLeftIndex, std::to_string(CurrentExpression));

                i = CurrentLeftIndex + std::to_string(CurrentExpression).length() - 1;
                LastIndex = expression.size() - 1;
            }
        }

        return ParserExpression<Number>(expression, plugins);
    }

};

int main() {
    Plugins plugins = Plugins();
    plugins.loadPlugins("./plugins");

    while (true) {
        Parser parser = Parser();
        std::string inputExpression;
        std::cout << "Write expression (can use: sin, cos, tg, ctg, log, ln, +, -, *, /, ^ ): ";
        std::getline(std::cin, inputExpression);

        try {
            double result = parser.ParserString<double>(inputExpression, plugins.plugins);
            std::cout << "Result: " << result << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Mistake: " << e.what() << std::endl;
        }
    }
    plugins.unloadPlugins();
    return 0;
}
