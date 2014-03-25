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
    /// Retourn l'ID de la bo�te de dialoge
    /// </summary>
    UINT GetDlgId()
    {
        return IDD_MAIN;
    }

    /// <summary>
    /// Enum�ration et initialisation des capteurs
    /// </summary>
    void InitializeResource();

    /// <summary>
    /// Traitement des commandes windows
    /// </summary>
    void HandleCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Modifie le message d'�tat
    /// <param name="message">Message</param>
    /// </summary>
    void SetStatusMessage(std::string message);

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