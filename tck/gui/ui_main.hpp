#ifndef UI_MAIN_HPP
#define UI_MAIN_HPP

#include <windows.h>
#include <d2d1.h>
#include <NuiApi.h>
#include <string>
#include <sstream>

namespace tck {
namespace gui {

class ui_main
{
public:
    ui_main();
    ~ui_main();
    int run(HINSTANCE hInstance, int nCmdShow);
    static LRESULT CALLBACK event_router(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    /** Variables **/
    HINSTANCE hInstance;
    HWND hWnd;

    // Kinect
    //INuiSensor* nui_sensor;
    INuiSensor* nui_sensor;
    HANDLE next_kinect_event;
    int cout_update;

    // Direct2D
    ID2D1Factory* D2DFactory;

    /** Fonctions **/
    void command_handle(UINT message, WPARAM wParam, LPARAM lParam);
    void set_status_message(std::string message);
    void DiscardDirect2DResources();

    // Fonctions Events
    LRESULT CALLBACK event_handle(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // Fonctions Kinect
    HRESULT kinect_init();
    void kinect_start();
    void kinect_stop();
    void kinect_update();

    void load_menu_icon(int menu_id, int submenu_id, int icon_id);
};

} // namespace gui
} // namespace tck

template<class T>
inline void safe_release(T *& ptr)
{
    if (ptr != NULL)
    {
        ptr->Release();
        ptr = NULL;
    }
}

#endif // UI_MAIN_HPP