#include"const.h"
#include"Window.h"
int main()
{
    Window app;
    Render renderer;
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

