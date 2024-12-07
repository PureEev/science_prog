#include "Engine.h"

void Engine::execute(const std::string& name, ArgsMap args) {
    auto it = commands.find(name);
    if (it == commands.end()) {
        throw std::invalid_argument("Command not found: " + name);
    }
    it->second->execute(args);
}