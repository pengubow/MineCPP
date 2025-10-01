#include <unordered_map>
#include <string>
#include <cstdint>

using namespace std;

class Textures {
private:
    static unordered_map<string, int32_t>* idMap;
public:
    static int32_t loadTexture(string resourceName, int32_t mode);
};