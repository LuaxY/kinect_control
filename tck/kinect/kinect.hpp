#ifndef KINECT_HPP
#define KINECT_HPP

#include <windows.h>
#include <d2d1.h>
#include <NuiApi.h>

class kinect
{
public:
    kinect();
    ~kinect();
    void start();
    static DWORD WINAPI kinect_main(LPVOID lpParam);
    static HANDLE next_kinect_event;

protected:
    DWORD tId;

    // Kinect
    //INuiSensor* nui_sensor;
    INuiSensor* nui_sensor;
    int cout_update;

    // Direct2D
    ID2D1Factory* D2DFactory;

    // Fonctions Kinect
    HRESULT kinect_init();
    void kinect_start();
    void kinect_stop();
    void kinect_update();
};

#endif // KINECT_HPP