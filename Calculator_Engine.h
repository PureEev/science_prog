#pragma once
#include "Plugin.h"
#include <string>
#include <vector>
#include <map>

class CalculatorEngine {
private:
    template<typename Number>
    static std::map<std::string, std::function<Number(Number, Number)>> functions = {
            { "+",   [](Number x, Number y) { return x + y; } },
            { "-",   [](Number x, Number y) { return x - y; } },
            { "*",   [](Number x, Number y) { return x * y; } },
            { "/",   [](Number x, Number y) { return (y == 0) ? NAN : (x / y); } },
            { "^",   [](Number x, Number y) { return pow(x,y); } },

    };
public:

    template<typename Number>
    void CalculateFuncWithOneArgument(std::vector<Number>& numbers, std::vector<std::string>& actions, std::vector<Plugin>& plugins) {
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
    }

    template<typename Number>
    void CalculateOperationsFirstPriority(std::vector<Number>& numbers, std::vector<std::string>& actions) {
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
    }

    template<typename Number>
    void CalculateOperationsSecondPriority(std::vector<Number>& numbers, std::vector<std::string>& actions) {
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
    }

    template<typename Number>
    Number CalculateExpression(std::vector<Number>& numbers, std::vector<std::string>& actions, std::vector<Plugin>& plugins) {

        CalculateFuncWithOneArgument(numbers, actions, plugins);
        CalculateOperationsFirstPriority(numbers, actions);
        CalculateOperationsSecondPriority(numbers, actions);

        if (numbers.empty()) {
            std::cerr << "Error: No numbers left in the vector to return." << std::endl;
            throw std::out_of_range("Vector 'numbers' is empty.");
        }

        return numbers[0];
    }
};