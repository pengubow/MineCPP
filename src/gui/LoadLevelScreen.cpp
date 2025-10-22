#include "gui/LoadLevelScreen.h"
#include "Minecraft.h"
#include "Util/httplib.h"

LoadLevelScreen::LoadLevelScreen(shared_ptr<Screen> parent) 
    : parent(parent) {}

void LoadLevelScreen::run() {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }
    try {
        status = "Getting level list..";
        httplib::Client cli(minecraft->minecraftUri);
        cli.set_connection_timeout(5, 0);
        cli.set_follow_location(true);
        
        string path = "/listmaps.jsp?user=" + minecraft->user->name;
        auto res = cli.Get(path);
        
        if (!res) {
            status = "Failed to connect";
            finished = true;
            return;
        }

        if (res->status != 200 || res->body.empty()) {
            status = "Failed to load levels";
            finished = true;
            return;
        }
        
        istringstream stream(res->body);
        string line;
        if (!getline(stream, line)) {
            status = "Failed to load levels";
            finished = true;
            return;
        }
        
        vector<string> levels;
        size_t start = 0;
        size_t end;
        while ((end = line.find(';', start)) != string::npos) {
            levels.push_back(line.substr(start, end - start));
            start = end + 1;
        }
        if (start < line.size()) {
            levels.push_back(line.substr(start));
        }
        
        if (levels.size() >= 5) {
            setLevels(levels);
            loaded = true;
            return;
        }

        status = levels[0];
        finished = true;
        
    } catch (const exception& e) {
        cerr << e.what() << endl;
        status = "Failed to load levels";
        finished = true;
    }
}

void LoadLevelScreen::setLevels(vector<string>& var1) {
    for(int32_t var2 = 0; var2 < 5; ++var2) {
        buttons[var2]->enabled = var1[var2] != "-";
        buttons[var2]->msg = var1[var2];
        buttons[var2]->visible = true;
    }
}

void LoadLevelScreen::init() {
    thread t(&LoadLevelScreen::run, this);
    t.detach();
    for(int32_t var1 = 0; var1 < 5; ++var1) {
        buttons.push_back(make_shared<Button>(var1, width / 2 - 100, height / 4 + var1 * 24, 200, 20, "---"));
        buttons[var1]->visible = false;
    }

    buttons.push_back(make_shared<Button>(5, width / 2 - 100, height / 4 + 144, 200, 20, "Cancel"));
}

void LoadLevelScreen::buttonClicked(shared_ptr<Button>& button) {
    if(button->enabled) {
        if (loaded && button->id < 5) {
            loadLevel(button->id);
        }

        if (finished || loaded && button->id == 5) {
            shared_ptr<Minecraft> minecraft = this->minecraft.lock();
            if (!minecraft) {
                return;
            }
            minecraft->setScreen(parent);
        }

    }
}

void LoadLevelScreen::loadLevel(int32_t var1) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (!minecraft) {
        return;
    }
    minecraft->loadLevel(minecraft->user->name, var1);
    minecraft->setScreen(nullptr);
    minecraft->grabMouse();
}

void LoadLevelScreen::render(int32_t var1, int32_t var2) {
    fillGradient(0, 0, width, height, 1610941696, -1607454624);
    drawCenteredString(title, width / 2, 40, 16777215);
    if(!loaded) {
        drawCenteredString(status, width / 2, height / 2 - 4, 16777215);
    }

    Screen::render(var1, var2);
}