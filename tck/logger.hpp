#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <sstream>
#include <windows.h>

#include "tck/utils.hpp"

namespace tck {

class logger
{
public:
    logger(HWND hWnd) :
        m_hWnd(hWnd)
    {
    }

    ~logger()
    {
        SendMessage(m_hWnd, WM_STATUSEVENT, (WPARAM)m_message.str().c_str(), 0);
    }

    template<class T>
    logger &operator<<(const T &x)
    {
        m_message << x;
        return *this;
    }

private:
    std::ostringstream  m_message;
    HWND                m_hWnd;
};

} // namespace tck

#endif // LOGGER_HPP