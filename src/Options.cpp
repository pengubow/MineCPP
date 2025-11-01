#include <fstream>
#include "Options.h"
#include "Minecraft.h"

const vector<string> Options::RENDER_DISTANCES = {"FAR", "NORMAL", "SHORT", "TINY"};

Options::Options(shared_ptr<Minecraft>& minecraft, fs::path optionsDir) {
    string optionsFilePath = optionsDir.string() + "options.txt";
    optionsFile = optionsDir / "options.txt";
    loadOptions();
}

string Options::getKeyBinding(int32_t binding) {
    string keyName = Util::getKeyName(keyBindings[binding]->key);
    return keyBindings[binding]->name + ": " + keyName;
}

void Options::setKeyBinding(int32_t binding, int32_t key) {
    keyBindings[binding]->key = key;
    saveOptions();
}

void Options::setOption(int32_t var1, int32_t var2) {
    if (var1 == 0) {
        music = !music;
    }

    if (var1 == 1) {
        sound = !sound;
    }

    if (var1 == 2) {
        invertMouse = !invertMouse;
    }

    if (var1 == 3) {
        showFPS = !showFPS;
    }

    if (var1 == 4) {
        renderDistance = (renderDistance + var2) & 3;
    }

    saveOptions();
}

string Options::getOption(int32_t var1) {
    if (var1 == 0) {
        return "Music: " + string(music ? "ON" : "OFF");
    }

    if (var1 == 1) {
        return "Sound: " + string(sound ? "ON" : "OFF");
    }

    if (var1 == 2) {
        return "Invert mouse: " + string(invertMouse ? "ON" : "OFF");
    }

    if (var1 == 3) {
        return "Show FPS: " + string(showFPS ? "ON" : "OFF");
    }

    if (var1 == 4) {
        return "Render distance: " + RENDER_DISTANCES[renderDistance];
    }

    return "";
}

void Options::loadOptions() {
    try {
        if (fs::exists(optionsFile)) {
            ifstream file(optionsFile);
            if (!file) {
                throw runtime_error("Failed to open options file");
            }

            string line;
            while (getline(file, line)) {
                if (line.empty()) {
                    continue;
                }

                size_t colonPos = line.find(":");
                if (colonPos == string::npos) {
                    continue;
                }

                string key = line.substr(0, colonPos);
                string value = line.substr(colonPos + 1);

                if (key == "music") {
                    music = (value == "true");
                }

                if (key == "sound") {
                    sound = (value == "true");
                }

                if (key == "invertYMouse") {
                    invertMouse = (value == "true");
                }

                if (key == "showFrameRate") {
                    showFPS = (value == "true");
                }

                if (key == "viewDistance") {
                    renderDistance = stoi(value);
                }

                for (int i = 0; i < keyBindings.size(); i++) {
                    if (key == "key_" + keyBindings[i]->name) {
                        keyBindings[i]->key = stoi(value);
                    }
                }
            }
        }
    } catch (const exception& e) {
        cerr << "Failed to load options" << endl;
        cerr << e.what() << endl;
    }
}

void Options::saveOptions() {
    try {
        ofstream file(optionsFile);
        if (!file) {
            throw runtime_error("Failed to open options");
        }

        file << "music:" << (music ? "true" : "false") << "\n";
        file << "sound:" << (sound ? "true" : "false") << "\n";
        file << "invertYMouse:" << (invertMouse ? "true" : "false") << "\n";
        file << "showFrameRate:" << (showFPS ? "true" : "false") << "\n";
        file << "viewDistance:" << renderDistance << "\n";

        for (int i = 0; i < keyBindings.size(); i++) {
            file << "key_" << keyBindings[i]->name << ":" << keyBindings[i]->key << "\n";
        }

        file.close();
    } catch (const exception& e) {
        cerr << "Failed to save options" << endl;
        cerr << e.what() << endl;
    }
}