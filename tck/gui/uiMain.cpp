#include "tck/gui/uiMain.hpp"
#include "tck/utils.hpp"
#include "tck/logger.hpp"

#include <iostream>

uiMain::uiMain():
    NuiViewer(nullptr)
{
}

uiMain::~uiMain()
{
}

int uiMain::Run()
{
    // Console de debug
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);

    CreateView();
    ShowView();

    /* ----------------------------- */
    m_pPrimaryView = new NuiStreamViewer(this);

    m_pNuiSensor = nullptr;
    GetSensor();

    if (m_pNuiSensor != nullptr)
    {
        m_pSkeletonStream = new NuiSkeletonStream(m_pNuiSensor);
        m_pSkeletonStream->SetStreamViewer(m_pPrimaryView);
    }
    /* ----------------------------- */

    MSG msg = { 0 };

    while(msg.message != WM_QUIT)
    {
        if(GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

LRESULT uiMain::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            m_hWnd = hWnd;

            SetMenuIcon(0, 0, IDB_FILE_EXIT);
            SetMenuIcon(1, 0, IDB_KINECT_START);
            SetMenuIcon(1, 1, IDB_KINECT_STOP);

            tck::logger(GetWindow()) << "Programme démarrée";

            // Set device status callback to monitor all sensor changes
            //NuiSetDeviceStatusCallback(StatusChangeCallback, reinterpret_cast<void*>(hWnd));

            InitializeResource();
        }
        break;

        case WM_COMMAND:
            HandleCommand(uMsg, wParam, lParam);
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

        case WM_STREAMEVENT:
            //UpdateStreams();
            m_pSkeletonStream->ProcessStreamFrame();
            std::cout << "stream" << std::endl;
            break;

        case WM_STATUSEVENT:
            SetStatusMessage(reinterpret_cast<char*>(wParam));
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

void uiMain::InitializeResource()
{
    NuiViewer::SetIcon(m_hWnd);
}

void uiMain::HandleCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ID_KINECT_START == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        MessageLoop();
        m_pSkeletonStream->StartStream();
        //KinectStream test;
        return;
    }
    if (ID_KINECT_STOP == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        SetEvent(m_hStopStreamEventThread);
        return;
    }
    if(ID_FICHIER_QUITTER == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
    {
        DestroyWindow(m_hWnd);
        return;
    }

    std::cout << "commande non traitee" << std::endl;
}

void uiMain::SetStatusMessage(char* message)
{
    std::ostringstream msg;
    msg << " " << message;
    SendDlgItemMessage(m_hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)msg.str().c_str());
}

void uiMain::MessageLoop()
{
    m_hStopStreamEventThread = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    HANDLE hEventThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)uiMain::StreamEventThread, reinterpret_cast<void*>(this), 0, nullptr);
}

void uiMain::StreamEventThread(uiMain* pThis)
{
    HANDLE events[] = { pThis->m_hStopStreamEventThread,
                        pThis->m_pSkeletonStream->GetFrameReadyEvent() };

    while(true)
    {
        DWORD ret = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);

        if (WAIT_OBJECT_0 == ret)
            break;

        if (WAIT_OBJECT_0 + 1 == ret)
        {
            SendMessage(pThis->GetWindow(), WM_STREAMEVENT, 0, 0);
        }
    }
}

void uiMain::GetSensor()
{
    INuiSensor* pNuiSensor = nullptr;

    if (SUCCEEDED(NuiCreateSensorByIndex(0, &pNuiSensor)))
    {
        if (pNuiSensor->NuiStatus() == S_OK)
        {
            tck::logger(GetWindow()) << "device connection ID: " << pNuiSensor->NuiDeviceConnectionId();

            if (SUCCEEDED(pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON)))
            {
                m_pNuiSensor = pNuiSensor;
                return;
            }
            else
                return;
        }
        else
            return;
    }
    else
    {
        return;
    }
}