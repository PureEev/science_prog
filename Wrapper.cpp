#include "Wrapper.h"

void Wrapper::execute(ArgsMap& inputArgs) const {
    std::vector<std::any> args;
    for (const auto& pair : inputArgs) {
        args.push_back(pair.second);
    }
    callable(args);
}