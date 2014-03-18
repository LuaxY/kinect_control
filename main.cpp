#include "tck/gui/ui_main.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    tck::gui::ui_main application;
    application.run(hInstance, nCmdShow);
}