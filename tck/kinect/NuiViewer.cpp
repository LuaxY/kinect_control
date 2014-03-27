#include "tck/kinect/NuiViewer.hpp"
//#include "tck/kinect/KinectWindow.h"
#include "tck/gui/res_main/resource.h"
#include "tck/utils.hpp"

HICON NuiViewer::SensorIcon;

/// <summary>
/// Constructor
/// </summary>
/// <param name="pParent">The pointer to parent window</param>
NuiViewer::NuiViewer(const NuiViewer* pParent)
    : m_hWnd(nullptr)
    , m_pParent(pParent)
{
    EnsureIconLoaded(SensorIcon, IDI_ICON);
}

/// <summary>
/// Create window of viewer
/// </sumarry>
/// <returns>Indicates success or failure</returns>
bool NuiViewer::CreateView()
{
    if (!m_hWnd)
    {
        HWND hWndParent = nullptr;
        if (m_pParent)
        {
            hWndParent = m_pParent->GetWindow();
        }

        /*WNDCLASSEX  wc = { 0 };
        m_hInstance = GetModuleHandle(nullptr);
        HICON icon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON));

        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.cbWndExtra = DLGWINDOWEXTRA;
        wc.hInstance = m_hInstance;
        wc.hCursor = LoadCursor(nullptr, (LPCSTR)IDC_POINTER);
        wc.hIcon = icon;
        wc.hIconSm = (HICON)LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
        wc.lpfnWndProc = DefDlgProc;
        wc.lpszClassName = "ui";

        if (!RegisterClassEx(&wc))
        {
            return FALSE;
        }*/

        m_hWnd = CreateDialogParam(nullptr,
                                    MAKEINTRESOURCE(GetDlgId()),
                                    hWndParent,
                                    (DLGPROC)MessageRouter,
                                    reinterpret_cast<LPARAM>(this));
    }

    return (nullptr != m_hWnd);
}

/// <summary>
/// Dispatch the message to the window object that it belongs to
/// </summary>
/// <param name="hWnd">The handle to the window which receives the message</param>
/// <param name="uMsg">Message type</param>
/// <param name="wParam">Message data</param>
/// <param name="lParam">Additional message data</param>
/// <returns>Result from message process</returns>
LRESULT NuiViewer::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    NuiViewer* pThis = nullptr;

    if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<NuiViewer*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<NuiViewer*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return (pThis->DialogProc(hWnd, uMsg, wParam, lParam));
    }

    return (LRESULT)FALSE;
}

/// <summary>
/// Shows up the window of the viewer
/// </summary>
void NuiViewer::ShowView()
{
    if (m_hWnd)
    {
        ShowWindow(m_hWnd, SW_SHOW);
        UpdateWindow(m_hWnd);
    }
}

/// <summary>
/// Hide the window of the viewer
/// </summary>
void NuiViewer::HideView()
{
    if (m_hWnd)
    {
        ShowWindow(m_hWnd, SW_HIDE);
    }
}

/// <summary>
/// Returns the window handle
/// </summary>
/// <returns>Handle to the window</returns>
HWND NuiViewer::GetWindow() const
{
    return m_hWnd;
}

/// <summary>
/// Set window with position and size
/// </summary>
/// <param name="rect">New position and size</param>
bool NuiViewer::MoveView(const RECT& rect)
{
    if (m_hWnd)
    {
        return FALSE != MoveWindow(m_hWnd,
                                   rect.left,
                                   rect.top,
                                   rect.right - rect.left,
                                   rect.bottom - rect.top,
                                   TRUE);
    }

    return false;
}

/// <summary>
/// Set the icon of the given window
/// </summary>
/// <param name="hWnd">Handle to the window</param>
void NuiViewer::SetIcon(HWND hWnd)
{
    if (nullptr != hWnd)
    {
        SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)SensorIcon);
        SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)SensorIcon);
    }
}

void NuiViewer::SetMenuIcon(int MenuId, int SubmenuId, int IconId)
{
    if (m_hWnd)
    {
        HMENU menubar = GetMenu(m_hWnd);

        HMENU menu = GetSubMenu(menubar, MenuId);
        UINT submenu = GetMenuItemID(menu, SubmenuId);

        HBITMAP icon = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IconId), IMAGE_BITMAP, 0, 0, 0);

        SetMenuItemBitmaps(menubar, submenu, 0, icon, icon);
    }
}