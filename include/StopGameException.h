#include <iostream>

using namespace std;

class StopGameException : public runtime_error {
public:
    StopGameException();
};