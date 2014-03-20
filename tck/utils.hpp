#ifndef UTILS_HPP
#define UTILS_HPP

#include <windows.h>

#include "tck/gui/res_main/resource.h"

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

template<class T>
inline void safe_release(T *& ptr)
{
    if (ptr != NULL)
    {
        ptr->Release();
        ptr = NULL;
    }
}

#endif // UTILS_HPP