#include "tck/gui/GuiManager.hpp"
#include "tck/utils.hpp"

HICON GuiManager::SensorIcon;

GuiManager::GuiManager(const GuiManager* pParent):
    m_hWnd(nullptr),
    m_pParent(pParent)
{
    EnsureIconLoaded(SensorIcon, IDI_ICON);
}

bool GuiManager::CreateView()
{
    if(!m_hWnd)
    {
        HWND hWndParent = nullptr;

        if(m_pParent)
        {
            hWndParent = m_pParent->GetWindow();
        }

        WNDCLASSEX  wc = { 0 };
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

        if(!RegisterClassEx(&wc))
        {
            return FALSE;
        }

        m_hWnd = CreateDialogParam(
            m_hInstance,
            MAKEINTRESOURCE(GetDlgId()),
            hWndParent,
            (DLGPROC)MessageRouter,
            reinterpret_cast<LPARAM>(this));
    }

    return (nullptr != m_hWnd);
}

LRESULT GuiManager::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    GuiManager* pThis = nullptr;

    if(WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<GuiManager*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<GuiManager*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if(pThis)
    {
        return (pThis->DialogProc(hWnd, uMsg, wParam, lParam));
    }

    return (LRESULT)FALSE;
}

void GuiManager::ShowView()
{
    if(m_hWnd)
    {
        ShowWindow(m_hWnd, SW_SHOW);
        UpdateWindow(m_hWnd);
    }
}

void GuiManager::HideView()
{
    if(m_hWnd)
    {
        ShowWindow(m_hWnd, SW_HIDE);
    }
}

HWND GuiManager::GetWindow() const
{
    return m_hWnd;
}

bool GuiManager::MoveView(const RECT& rect)
{
    if(m_hWnd)
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

void GuiManager::SetIcon(HWND hWnd)
{
    if(hWnd != nullptr)
    {
        SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)SensorIcon);
        SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)SensorIcon);
    }
}

void GuiManager::SetMenuIcon(int MenuId, int SubmenuId, int IconId)
{
    if(m_hWnd)
    {
        HMENU menubar = GetMenu(m_hWnd);

        HMENU menu = GetSubMenu(menubar, MenuId);
        UINT submenu = GetMenuItemID(menu, SubmenuId);

        HBITMAP icon = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IconId), IMAGE_BITMAP, 0, 0, 0);

        SetMenuItemBitmaps(menubar, submenu, 0, icon, icon);
    }
}