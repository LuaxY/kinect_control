#ifndef UI_MAIN_HPP
#define UI_MAIN_HPP

#include "tck/gui/NuiViewer.hpp"

#include <windows.h>
#include <string>
#include <sstream>

class ui_main : public NuiViewer
{
public:

    ui_main();

    ~ui_main();

    /// <summary>
    /// Traitement des message
    /// </summary>
    /// <param name="hWnd">Propri�taire du message</param>
    /// <param name="uMsg">Message</param>
    /// <param name="wParam">Donn�es du message</param>
    /// <param name="lParam">Donn�es additionnel du message</param>
    /// <returns>resultat du traitement du message</returns>
    LRESULT DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Cr�ation de la fenetre principale et d�buter le traitement
    /// </summary>
    int Run();

private:
    /// <summary>
    /// Returns the ID of the dialog
    /// </summary>
    UINT GetDlgId()
    {
        return IDD_MAIN;
    }

    /// <summary>
    /// This method will initialize all the members and enumerate all the sensors
    /// </summary>
    void InitializeResource();
};

#endif // UI_MAIN_HPP