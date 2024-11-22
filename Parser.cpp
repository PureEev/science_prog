#include "Parser.h"



void Parser::HandleAction(const std::string& FunctionStr, std::vector<std::string>& StackOfActions) {
    for (const auto& action : Actions) {
        if (action == FunctionStr) {
            StackOfActions.push_back(action);
            return;
        }
    }
}

