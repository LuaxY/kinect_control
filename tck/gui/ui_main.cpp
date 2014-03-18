#include "ui_main.hpp"
#include "tck/gui/res_main/resource.h"

#include <iostream>

/**
 * Constructeur fenètre principale.
 **/
tck::gui::ui_main::ui_main() :
    nui_sensor(NULL),
    D2DFactory(NULL),
    next_kinect_event(INVALID_HANDLE_VALUE),
    cout_update(0)
{
}

/**
 * Déstructeur fenètre principale.
 **/
tck::gui::ui_main::~ui_main()
{
    kinect_stop();
    //nui_sensor->NuiShutdown(); // possible deadlock -> Dev Microsoft = Je code avec le cul
    safe_release(nui_sensor);
    DiscardDirect2DResources();
    safe_release(D2DFactory);
}

/**
 * Fonction de lancement de la fenètre principale.
 **/
int tck::gui::ui_main::run(HINSTANCE _hInstance, int nCmdShow)
{
    // Console de debug
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);

    MSG       msg = {0};
    WNDCLASSEX  wc  = {0};
    HICON icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));

    hInstance = _hInstance;

    // Paramétrage de la fenetre
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, (LPCSTR)IDC_POINTER);
    wc.hIcon         = icon;
    wc.hIconSm       = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = "ui_main";

    if(!RegisterClassEx(&wc))
    {
        return -1;
    }

    // Création de la fenetre principale
    HWND hWndApp = CreateDialogParam(
        hInstance,
        MAKEINTRESOURCE(IDD_MAIN),
        NULL,
        (DLGPROC)tck::gui::ui_main::event_router,
        reinterpret_cast<LPARAM>(this));

    // Afficher la fenetre
    ShowWindow(hWndApp, nCmdShow);

    SendMessage(hWndApp, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessage(hWndApp, WM_SETICON, ICON_BIG, (LPARAM)icon);

    const int event_count = 1;
    HANDLE events[event_count];

    // Boucle principale
    while(WM_QUIT != msg.message)
    {
        events[0] = next_kinect_event;

        // Vérifier s'il y a de nouveaux messages (QS_ALLINPUT)
        // ou des events Kinect (events)
        // La fonction update() vérifira les events invalide, dans où plus d'un est signaler
        MsgWaitForMultipleObjects(event_count, events, FALSE, INFINITE, QS_ALLINPUT);

        // Explicitly check the Kinect frame event since MsgWaitForMultipleObjects
        // can return for other reasons even though it is signaled.

        // Mise à jour du traitement de la Kinect
        kinect_update();

        while(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if((hWndApp != NULL) && IsDialogMessageW(hWndApp, &msg))
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
 * Fonction de routage des évènement Win32 et Kinect.
 **/
LRESULT CALLBACK tck::gui::ui_main::event_router(HWND _hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ui_main* pThis = NULL;

    if(uMsg == WM_INITDIALOG)
    {
        pThis = reinterpret_cast<ui_main*>(lParam);
        SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)); 
    }
    else
    {
        pThis = reinterpret_cast<ui_main*>(GetWindowLongPtr(_hWnd, GWLP_USERDATA));
    }

    if(pThis)
    {
        return pThis->event_handle(_hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

/**
 * Fonction de traitement des évènement Win32 et Kinect.
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
            load_menu_icon(0, 0, IDB_FILE_EXIT);
            load_menu_icon(1, 0, IDB_KINECT_START);
            load_menu_icon(1, 1, IDB_KINECT_STOP);
            break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_COMMAND:
            command_handle(message, wParam, lParam);
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

    if(FAILED(hr))
        return hr;

    // Verification de tout les capteurs de la Kinect
    for(int i = 0; i < sensor_count; ++i)
    {
        // Création des capteur pour vérifier leur état
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if(FAILED(hr))
            continue;

        // Récupération du status du capteur, si connecté -> initalisation
        hr = pNuiSensor->NuiStatus();
        if(hr == S_OK)
        {
            nui_sensor = pNuiSensor;
            break;
        }

        // Capteur non fonctionnel, détachement
        pNuiSensor->Release();
    }

    if(nui_sensor != NULL)
    {
        // Initialisation du capteur du squelette
        hr = nui_sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);

        if(SUCCEEDED(hr))
        {
            set_status_message("Kinect initialisée");
            return hr;
        }
    }

    if(nui_sensor == NULL || FAILED(hr))
    {
        set_status_message("Aucune Kinect trouvée");
        return E_FAIL;
    }
}

/**
* Fonction de démarrage de la Kinect.
**/
void tck::gui::ui_main::kinect_start()
{
    if(nui_sensor != NULL)
    {
        // Création d'un évenement pour signaler de nouvelles données du squette
        next_kinect_event = CreateEvent(NULL, TRUE, FALSE, NULL);

        // Activation du flux de données du squelette
        /*hr =*/ nui_sensor->NuiSkeletonTrackingEnable(next_kinect_event, 0);

        set_status_message("Kinect démarrée");
    }
}

/**
* Fonction d'arret de la Kinect.
**/
void tck::gui::ui_main::kinect_stop()
{
    if(nui_sensor != NULL)
    {
        ResetEvent(next_kinect_event);

        nui_sensor->NuiSkeletonTrackingDisable();
        cout_update = 0;
        std::cout << "\rx0               ";

        set_status_message("Kinect arrêtée");
    }
}

/**
 * Fonction de traitement des données Kinect.
 **/
void tck::gui::ui_main::kinect_update()
{
    if(nui_sensor == NULL)
        return;

    // Attente de traitement des données du squelette
    if(WaitForSingleObject(next_kinect_event, 0) == WAIT_OBJECT_0)
    {
        cout_update++;
        std::cout << "\rx" << cout_update;
        //ProcessSkeleton();
    }
}

/**
 * Fonction d'ajout d'icones sur les menus.
 **/
void tck::gui::ui_main::load_menu_icon(int menu_id, int submenu_id, int icon_id)
{
    HMENU menubar = GetMenu(hWnd);

    HMENU menu = GetSubMenu(menubar, menu_id);
    UINT submenu = GetMenuItemID(menu, submenu_id);

    HBITMAP icon = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(icon_id), IMAGE_BITMAP, 0, 0, 0);

    SetMenuItemBitmaps(menubar, submenu, 0, icon, icon);
}

/**
* Fonction de traitement des commandes.
**/
void tck::gui::ui_main::command_handle(UINT message, WPARAM wParam, LPARAM lParam)
{
    if(ID_KINECT_START == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
        kinect_start();
    if(ID_KINECT_STOP == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
        kinect_stop();
    if(ID_FICHIER_QUITTER == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
        DestroyWindow(hWnd);
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