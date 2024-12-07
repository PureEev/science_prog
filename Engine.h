#include <unordered_map>
#include <stdexcept>
#include <memory> 
#include "Wrapper.h"

using ArgsMap = std::unordered_map<std::string, std::string>;

class Engine {
private:

    std::unordered_map<std::string, std::unique_ptr<Wrapper>> commands;

public:

    void register_command(Wrapper wrapper, const std::string& name) {
        commands[name] = std::make_unique<Wrapper>(std::move(wrapper));
    }

    void execute(const std::string& name, ArgsMap args);
};
