#include "core/Application.h"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    Application app;

    if (!app.initialize()) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }

    std::cout << "Ball Bouncing Simulator" << std::endl;
    std::cout << "Press ESC to quit" << std::endl;

    app.run();
    app.cleanup();

    return 0;
}
