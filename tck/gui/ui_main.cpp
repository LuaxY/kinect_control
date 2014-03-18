#include "ui_main.hpp"
#include "tck/gui/res_main/resource.h"

#include <iostream>

/**
 * Constructeur fen�tre principale.
 **/
tck::gui::ui_main::ui_main() :
    nui_sensor(NULL),
    D2DFactory(NULL),
    next_kinect_event(INVALID_HANDLE_VALUE),
    cout_update(0)
{
}

/**
 * D�structeur fen�tre principale.
 **/
tck::gui::ui_main::~ui_main()
{
    kinect_stop();

    DiscardDirect2DResources();
    safe_release(D2DFactory);
}

/**
 * Fonction de lancement de la fen�tre principale.
 **/
int tck::gui::ui_main::run(HINSTANCE _hInstance, int nCmdShow)
{
    // Console de debug
    AllocConsole() ;
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);

    MSG       msg = {0};
    WNDCLASS  wc  = {0};
    hInstance = _hInstance;

    // Param�trage de la fenetre
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, (LPCSTR)IDC_POINTER);
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = "ui_main";

    if (!RegisterClass(&wc))
    {
        return 0;
    }

    // Cr�ation de la fenetre principale
    HWND hWndApp = CreateDialogParam(
        hInstance,
        MAKEINTRESOURCE(IDD_MAIN),
        NULL,
        (DLGPROC)tck::gui::ui_main::event_router,
        reinterpret_cast<LPARAM>(this));

    // Afficher la fenetre
    ShowWindow(hWndApp, nCmdShow);

    const int event_count = 1;
    HANDLE events[event_count];

    // Boucle principale
    while (WM_QUIT != msg.message)
    {
        events[0] = next_kinect_event;

        // V�rifier s'il y a de nouveaux messages (QS_ALLINPUT)
        // ou des events Kinect (events)
        // La fonction update() v�rifira les events invalide, dans o� plus d'un est signaler
        MsgWaitForMultipleObjects(event_count, events, FALSE, INFINITE, QS_ALLINPUT);

        // Explicitly check the Kinect frame event since MsgWaitForMultipleObjects
        // can return for other reasons even though it is signaled.

        // Mise � jour du traitement de la Kinect
        kinect_update();

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if ((hWndApp != NULL) && IsDialogMessageW(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

/**
 * Fonction de routage des �v�nement Win32 et Kinect.
 **/
LRESULT CALLBACK tck::gui::ui_main::event_router(HWND _hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ui_main* pThis = NULL;

    if (uMsg == WM_INITDIALOG)
    {
        pThis = reinterpret_cast<ui_main*>(lParam);
        SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)); 
    }
    else
    {
        pThis = reinterpret_cast<ui_main*>(GetWindowLongPtr(_hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->event_handle(_hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

/**
 * Fonction de traitement des �v�nement Win32 et Kinect.
 **/
LRESULT CALLBACK tck::gui::ui_main::event_handle(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_INITDIALOG:
            hWnd = _hWnd;

            kinect_init();
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DFactory);

            // Chargement des icones des menus
            load_menu_icon(hWnd, 0, 0, IDB_FILE_EXIT);
            load_menu_icon(hWnd, 1, 0, IDB_KINECT_START);
            load_menu_icon(hWnd, 1, 1, IDB_KINECT_STOP);
            break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_COMMAND:
            command_handle(hWnd, message, wParam, lParam);
            break;
    }

    return FALSE;
}

/**
 * Fonction d'initalisation de la Kinect.
 **/
HRESULT tck::gui::ui_main::kinect_init()
{
    INuiSensor* pNuiSensor;

    int sensor_count = 0;
    HRESULT hr = NuiGetSensorCount(&sensor_count);

    if (FAILED(hr))
        return hr;

    // Verification de tout les capteurs de la Kinect
    for (int i = 0; i < sensor_count; ++i)
    {
        // Cr�ation des capteur pour v�rifier leur �tat
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
            continue;

        // R�cup�ration du status du capteur, si connect� -> initalisation
        hr = pNuiSensor->NuiStatus();
        if (hr == S_OK)
        {
            nui_sensor = pNuiSensor;
            break;
        }

        // Capteur non fonctionnel, d�tachement
        pNuiSensor->Release();
    }

    if (nui_sensor != NULL)
    {
        // Initialisation du capteur du squelette
        hr = nui_sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
    }

    if (nui_sensor == NULL || FAILED(hr))
    {
        set_status_message("Aucune Kinect trouv�e");
        return E_FAIL;
    }

    set_status_message("Kinect initialis�e");
}

/**
* Fonction de d�marrage de la Kinect.
**/
void tck::gui::ui_main::kinect_start()
{
    if (nui_sensor != NULL)
    {
        /*if (SUCCEEDED(hr))
        {*/
            // Cr�ation d'un �venement pour signaler de nouvelles donn�es du squette
            next_kinect_event = CreateEvent(NULL, TRUE, FALSE, NULL);

            // Activation du flux de donn�es du squelette
            /*hr =*/ nui_sensor->NuiSkeletonTrackingEnable(next_kinect_event, 0);

            set_status_message("Kinect d�marr�e");
        //}
    }
}

/**
* Fonction d'arret de la Kinect.
**/
void tck::gui::ui_main::kinect_stop()
{
    if (nui_sensor != NULL)
    {
        ResetEvent(next_kinect_event);

        nui_sensor->NuiSkeletonTrackingDisable();
        //nui_sensor->NuiShutdown(); // Eteint la Kinect, useless :)
        //safe_release(nui_sensor);
        cout_update = 0;
        std::cout << "\rx0               ";

        set_status_message("Kinect arr�t�e");
    }
}

/**
 * Fonction de traitement des donn�es Kinect.
 **/
void tck::gui::ui_main::kinect_update()
{
    if (nui_sensor == NULL)
        return;

    // Attente de traitement des donn�es du squelette
    if (WaitForSingleObject(next_kinect_event, 0) == WAIT_OBJECT_0)
    {
        cout_update++;
        std::cout << "\rx" << cout_update;
        //ProcessSkeleton();
    }
}

/**
 * Fonction d'ajout d'icones sur les menus.
 **/
void tck::gui::ui_main::load_menu_icon(HWND _hWnd, int menu_id, int submenu_id, int icon_id)
{
    HMENU menubar = GetMenu(_hWnd);

    HMENU menu = GetSubMenu(menubar, menu_id);
    UINT submenu = GetMenuItemID(menu, submenu_id);

    HBITMAP icon = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(icon_id), IMAGE_BITMAP, 0, 0, 0);

    SetMenuItemBitmaps(menubar, submenu, 0, icon, icon);
}

/**
* Fonction de traitement des commandes.
**/
void tck::gui::ui_main::command_handle(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ID_KINECT_START == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        //kinect_init();
        kinect_start();
    }

    if (ID_KINECT_STOP == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
        kinect_stop();
    if (ID_FICHIER_QUITTER == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
        DestroyWindow(_hWnd);
}

/**
* Fonction de gestion du message de status.
**/
void tck::gui::ui_main::set_status_message(std::string message)
{
    std::ostringstream msg;
    msg << " " << message;
    SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)msg.str().c_str());
}

void tck::gui::ui_main::DiscardDirect2DResources()
{
    /*safe_release(m_pRenderTarget);

    safe_release(m_pBrushJointTracked);
    safe_release(m_pBrushJointInferred);
    safe_release(m_pBrushBoneTracked);
    safe_release(m_pBrushBoneInferred);*/
}