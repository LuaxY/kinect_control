#include "tck/gui/NuiViewer.hpp"
#include "tck/utils.hpp"

HICON NuiViewer::SensorIcon;

NuiViewer::NuiViewer(const NuiViewer* pParent):
    m_hWnd(nullptr),
    m_pParent(pParent)
{
    EnsureIconLoaded(SensorIcon, IDI_ICON);
}

bool NuiViewer::CreateView()
{
    if (!m_hWnd)
    {
        HWND hWndParent = nullptr;
        if (m_pParent)
        {
            hWndParent = m_pParent->GetWindow();
        }

        m_hWnd = CreateDialogParam(
            nullptr,
            MAKEINTRESOURCE(GetDlgId()),
            hWndParent,
            (DLGPROC)MessageRouter,
            reinterpret_cast<LPARAM>(this));
    }

    return (nullptr != m_hWnd);
}

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
        pThis = reinterpret_cast<NuiViewer*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return (pThis->DialogProc(hWnd, uMsg, wParam, lParam));
    }

    return (LRESULT)FALSE;
}

void NuiViewer::ShowView()
{
    if (m_hWnd)
    {
        ShowWindow(m_hWnd, SW_SHOW);
        UpdateWindow(m_hWnd);
    }
}

void NuiViewer::HideView()
{
    if (m_hWnd)
    {
        ShowWindow(m_hWnd, SW_HIDE);
    }
}

HWND NuiViewer::GetWindow() const
{
    return m_hWnd;
}

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

void NuiViewer::SetIcon(HWND hWnd)
{
    if (nullptr != hWnd)
    {
        SendMessageW(hWnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)SensorIcon);
        SendMessageW(hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)SensorIcon);
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