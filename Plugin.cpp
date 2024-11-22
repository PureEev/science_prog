#include "Plugin.h"

void Plugins::loadPlugins(const std::string& directory) {

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

void Plugins::unloadPlugins() {
    for (auto& plugin : plugins) {
        FreeLibrary(plugin.library);
    }
}