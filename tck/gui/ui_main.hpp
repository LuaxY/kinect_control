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
    /// <param name="hWnd">Propriétaire du message</param>
    /// <param name="uMsg">Message</param>
    /// <param name="wParam">Données du message</param>
    /// <param name="lParam">Données additionnel du message</param>
    /// <returns>resultat du traitement du message</returns>
    LRESULT DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Création de la fenetre principale et débuter le traitement
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