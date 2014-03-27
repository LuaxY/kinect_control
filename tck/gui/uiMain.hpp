#ifndef UI_MAIN_HPP
#define UI_MAIN_HPP

#include "tck/kinect/NuiViewer.hpp"
#include "tck/kinect/NuiSkeletonStream.hpp"

#include <windows.h>
#include <string>
#include <sstream>

class uiMain : public NuiViewer
{
public:

    uiMain();

    ~uiMain();

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
    /// Retourn l'ID de la boîte de dialoge
    /// </summary>
    UINT GetDlgId()
    {
        return IDD_MAIN;
    }

    /// <summary>
    /// Enumération et initialisation des capteurs
    /// </summary>
    void InitializeResource();

    /// <summary>
    /// Traitement des commandes windows
    /// </summary>
    void HandleCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Modifie le message d'état
    /// <param name="message">Message</param>
    /// </summary>
    void SetStatusMessage(char* message);

    /* TEST ZONE */
    HANDLE                  m_hStopStreamEventThread;
    INuiSensor*             m_pNuiSensor;
    NuiSkeletonStream*      m_pSkeletonStream;
    NuiStreamViewer*        m_pPrimaryView;

    void GetSensor();
    static void WINAPI StreamEventThread(uiMain*);
    void MessageLoop();
};

#endif // UI_MAIN_HPP