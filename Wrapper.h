#pragma once
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <string>
#include <tuple>
#include <any>
#include <vector>

class Wrapper {
public:
    using ArgsMap = std::unordered_map<std::string, std::string>;

    template <typename Ret, typename InstanceType, typename... Args>
    Wrapper(InstanceType* instance, Ret(InstanceType::* method)(Args...), const ArgsMap& defaultArgs)
        : args(defaultArgs) {
        callable = [instance, method](const std::vector<std::any>& args) -> std::any {
            if (args.size() != sizeof...(Args)) {
                throw std::invalid_argument("Invalid number of arguments.");
            }

            auto tuple_args = [&]<std::size_t... I>(std::index_sequence<I...>) {
                return std::make_tuple(std::any_cast<std::remove_reference_t<Args>>(args[I])...);
            }(std::index_sequence_for<Args...>{});

            if constexpr (std::is_void_v<Ret>) {
                std::apply([&](auto&&... unpacked_args) {
                    (instance->*method)(std::forward<decltype(unpacked_args)>(unpacked_args)...);
                    }, tuple_args);
                return std::any(); 
            }
            else {
                return std::apply([&](auto&&... unpacked_args) {
                    return (instance->*method)(std::forward<decltype(unpacked_args)>(unpacked_args)...);
                    }, tuple_args);
            }
            };
    }

    void execute(ArgsMap& inputArgs) const;

private:

    std::function<std::any(const std::vector<std::any>&)> callable; 
    ArgsMap args;
};
