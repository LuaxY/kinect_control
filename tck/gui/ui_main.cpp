#include "tck/gui/ui_main.hpp"
#include "tck/kinect/kinect.hpp"

#include <iostream>

ui_main::ui_main():
    NuiViewer(nullptr)
{
}

ui_main::~ui_main()
{
}

int ui_main::Run()
{
    // Console de debug
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);

    CreateView();
    ShowView();

    MSG msg = { 0 };

    while (msg.message != WM_QUIT)
    {
        if (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam);

    /*// Console de debug
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);

    MSG       msg = {0};
    WNDCLASSEX  wc  = {0};
    HICON icon = LoadIcon(_hInstance, MAKEINTRESOURCE(IDI_ICON));

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
        (DLGPROC)ui_main::event_router,
        reinterpret_cast<LPARAM>(this));

    // Afficher la fenetre
    ShowWindow(hWndApp, nCmdShow);

    SendMessage(hWndApp, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessage(hWndApp, WM_SETICON, ICON_BIG, (LPARAM)icon);

    // Boucle principale
    while(WM_QUIT != msg.message)
    {
        

        // Vérifier s'il y a de nouveaux messages (QS_ALLINPUT)
        // ou des events Kinect (events)
        // La fonction update() vérifira les events invalide, dans où plus d'un est signaler
        

        // Explicitly check the Kinect frame event since MsgWaitForMultipleObjects
        // can return for other reasons even though it is signaled.

        // Mise à jour du traitement de la Kinect
        //kinect_update();

        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if((hWndApp != NULL) && IsDialogMessage(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam);*/
}

LRESULT ui_main::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;

            SetMenuIcon(0, 0, IDB_FILE_EXIT);
            SetMenuIcon(1, 0, IDB_KINECT_START);
            SetMenuIcon(1, 1, IDB_KINECT_STOP);

            // Set device status callback to monitor all sensor changes
            //NuiSetDeviceStatusCallback(StatusChangeCallback, reinterpret_cast<void*>(hWnd));

            //InitializeResource();
        }
        break;

        case WM_COMMAND:
            std::cout << std::hex << uMsg << std::endl;
            break;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORDLG:
            return (LRESULT)GetStockObject(WHITE_BRUSH);

        /*case WM_NOTIFY:
            switch (LOWORD(wParam))
            {
                case IDC_MOREINFOLINK:
                    OnClickMoreInfoLink(lParam);
                    break;

                case IDC_KINECTSENSORLIST:
                    return m_pSensorListControl->HandleNotifyMessage(hWnd, (LPNMLISTVIEW)lParam);

                case IDC_STATUSLOGLIST:
                    return m_pStatusLogListControl->HandleNotifyMessage(hWnd, (LPNMLISTVIEW)lParam);
            }
            break;*/

        // If the titlebar X is clicked, destroy the app
        case WM_CLOSE:
        {
            /*m_pKinectWindowMgr->CloseAllKinectWindows();*/
            DestroyWindow(hWnd);
        }
        break;

        // Quit the main message pump
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        // Handle the Kinect sensor status change case
        /*case WM_UPDATEMAINWINDOW:
        {
            UpdateMainWindow((PCWCHAR)wParam, (HRESULT)lParam);
            UpdateLayoutAndShowStatus();
        }
        break;*/

        // If the kinect window is closed, notify Kinect window manager
        /*case WM_CLOSEKINECTWINDOW:
            m_pKinectWindowMgr->ResetKinectWindow((PCWSTR)wParam);
            break;*/

        /*case WM_SIZE:
            Resize();
            break;*/

        /*case WM_PAINT:
            DrawBreakLine();
            break;*/

        /*case WM_GETMINMAXINFO:
        {
            POINT minTrackSize = { m_minTrackWidth, 0 };

            auto pMinMaxInfo = (PMINMAXINFO)lParam;
            pMinMaxInfo->ptMinTrackSize = minTrackSize;
        }
        break;*/
    }

    return FALSE;
}

void ui_main::InitializeResource()
{
    NuiViewer::SetIcon(m_hWnd);
}

/**
 * Fonction de routage des évènement Win32 et Kinect.
 **/
/*LRESULT CALLBACK ui_main::event_router(HWND _hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
}*/

/**
 * Fonction de traitement des évènement Win32 et Kinect.
 **/
/*LRESULT CALLBACK ui_main::event_handle(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_INITDIALOG:
            hWnd = _hWnd;

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
}*/

/*void ui_main::start()
{
    kinect te;
    te.start();
}*/

/**
* Fonction de traitement des commandes.
**/
/*void ui_main::command_handle(UINT message, WPARAM wParam, LPARAM lParam)
{
    if(ID_KINECT_START == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
        start();
    if (ID_KINECT_STOP == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
        return;
        //stop();
    if(ID_FICHIER_QUITTER == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
        DestroyWindow(hWnd);
}*/

/**
* Fonction de gestion du message de status.
**/
/*void ui_main::set_status_message(std::string message)
{
    std::ostringstream msg;
    msg << " " << message;
    SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)msg.str().c_str());
}*/

/*void ui_main::DiscardDirect2DResources()
{
    safe_release(m_pRenderTarget);
    safe_release(m_pBrushJointTracked);
    safe_release(m_pBrushJointInferred);
    safe_release(m_pBrushBoneTracked);
    safe_release(m_pBrushBoneInferred);
}*/