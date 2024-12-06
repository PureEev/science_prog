#include <string>
#include <unordered_map>
#include <map>
#include <stdexcept>
#include "Wrapper.h"

class Engine {
private:
    std::map<std::string, const Wrapper*> commands;

public:
    void register_command(const Wrapper* wrapper, const std::string& command_name) {
        commands[command_name] = wrapper;
    }

    void execute(const std::string& name, Wrapper::ArgsMap args) const {
        auto it = commands.find(name);
        if (it == commands.end()) {
            throw std::invalid_argument("Command not found: " + name);
        }
        it->second->execute(args);
    }

};
