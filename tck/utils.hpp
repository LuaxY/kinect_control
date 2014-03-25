#ifndef UTILS_HPP
#define UTILS_HPP

#include <windows.h>

#include "tck/gui/res_main/resource.h"

#define WM_STREAMEVENT  WM_USER + 1
#define WM_TIMEREVENT   WM_USER + 2

#define CLOSING_FROM_STATUSCHANGED    1
#define READING_TEXT_BUFFER_SIZE    128

static const UINT MaxStringChars = 256;

/// <summary>
/// Ensure the font object has been created correctly
/// </summary>
inline void EnsureFontCreated(HFONT& hFont, int fontSize, int fontWeight)
{
    if (nullptr == hFont)
    {
        hFont = CreateFont(fontSize, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    }
}

/// <summary>
/// Ensure the image object has been loaded correctly
/// </summary>
inline void EnsureImageLoaded(HBITMAP& hBitmap, UINT resourceId)
{
    if (nullptr == hBitmap)
    {
        hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId));
    }
}

/// <summary>
/// Ensure the icon object has been loaded correctly
/// </summary>
inline void EnsureIconLoaded(HICON& hIcon, UINT resourceId)
{
    if (nullptr == hIcon)
    {
        hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId));
    }
}

inline SIZE GetWindowSize(HWND hWnd)
{
    RECT rect;
    GetWindowRect(hWnd, &rect);

    SIZE size = { rect.right - rect.left, rect.bottom - rect.top };
    return size;
}

inline SIZE GetClientSize(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);

    SIZE size = { rect.right, rect.bottom };
    return size;
}

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface*& pInterfaceToRelease)
{
    if (pInterfaceToRelease)
    {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = nullptr;
    }
}

// Safe delete pointers
template<class T>
inline void SafeDelete(T*& ptr)
{
    if (ptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

// Safe delete array
template<class T>
inline void SafeDeleteArray(T*& pArray)
{
    if (pArray)
    {
        delete[] pArray;
        pArray = nullptr;
    }
}

// Compare the reading, format and update to static control if it changed
template<class T>
void CompareUpdateValue(const T newValue, T& storedValue, HWND hWnd, UINT controlID, LPCWSTR format)
{
    if (storedValue != newValue)
    {
        storedValue = newValue;

        WCHAR buffer[READING_TEXT_BUFFER_SIZE];
        swprintf_s(buffer, READING_TEXT_BUFFER_SIZE, format, newValue);
        SetDlgItemText(hWnd, controlID, buffer);
    }
}

#endif // UTILS_HPP