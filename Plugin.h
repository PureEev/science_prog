#pragma once
#include <iostream>
#include <sstream>
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

    void loadPlugins(const std::string& directory);

    void unloadPlugins();
};