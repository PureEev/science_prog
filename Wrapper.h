#pragma once
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <string>
#include <tuple>

class Wrapper {
public:
    using ArgsMap = std::unordered_map<std::string, std::string>;

    Wrapper() = default;

    template <typename InstanceType, typename MethodType>
    Wrapper(InstanceType* instance, MethodType method, const ArgsMap& defaultArgs)
        : invoker_([=](ArgsMap& args) -> void {
        ArgsMap mergedArgs = defaultArgs;
        for (const auto& [key, value] : args) {
            mergedArgs[key] = value;
        }
        call_method(instance, method, mergedArgs);
            }) {}

    void execute(ArgsMap& args) const {
        if (!invoker_) {
            throw std::runtime_error("Method invoker not initialized");
        }
        invoker_(args);
    }

private:
    using Invoker = std::function<void(ArgsMap&)>;
    Invoker invoker_;

    template <typename T, typename... Args>
    void vector_to_tuple(const std::vector<T>& vec, std::tuple<Args...>& t) {
        if constexpr (sizeof...(Args) > 0) {
            std::get<sizeof...(Args) - sizeof...(Args) - 1>(t) = vec[sizeof...(Args) - sizeof...(Args) - 1];
        }
    }

    template <typename... Args>
    void call_function(std::function<void(Args...)> func, std::vector<std::string> args) {

        auto tuple_args = std::make_tuple(args[0], args[1]);

        std::apply(func, tuple_args);
    }

    template <typename InstanceType, typename MethodType>
    static void call_method(InstanceType* instance, MethodType method, ArgsMap& args) {
        std::vector<std::string> Args;
        for (const auto& pair : args) {
            Args.push_back(pair.second);
        }


        call_function(instance->*method, Args);

    }
};







