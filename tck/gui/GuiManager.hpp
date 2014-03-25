#ifndef GUI_MANAGER_HPP
#define GUI_MANAGER_HPP

#include "tck/gui/res_main/resource.h"

#include <windows.h>
#include <memory>

class GuiManager
{
public:
    /// <summary>
    /// Constructeur
    /// </summary>
    /// <param name="pParent">Pointeur sur la fenètre parent</param>
    GuiManager(const GuiManager* pParent);

    /// <summary>
    /// Création de la fenetre
    /// </sumarry>
    /// <returns>Indique le succès ou l'échec</returns>
    virtual bool CreateView();

    /// <summary>
    /// Affiche le fenètre
    /// </summary>
    virtual void ShowView();

    /// <summary>
    /// Cache la fenètre
    /// </summary>
    virtual void HideView();

    /// <summary>
    /// Retour l'handle de la fenètre
    /// </summary>
    /// <returns>Handle de la fenètre</returns>
    virtual HWND GetWindow() const;

    /// <summary>
    /// Modifie la position et taille de la fenètre
    /// </summary>
    /// <param name="rect">Nouvelle positions et taille</param>
    /// <returns>Indique le succès ou l'échec</returns>
    virtual bool MoveView(const RECT& rect);

protected:
    HWND               m_hWnd;
    const   GuiManager* m_pParent;
    static  HICON      SensorIcon;
    HINSTANCE          m_hInstance;

    /// <summary>
    /// Dispatche les messages aux fenètre correspondant
    /// </summary>
    /// <param name="hWnd">Handle de la fenètre recevant le message</param>
    /// <param name="uMsg">Type de message</param>
    /// <param name="wParam">Données du message</param>
    /// <param name="lParam">Données additionnel du message</param>
    /// <returns>Resultat du traitement du message</returns>
    static LRESULT MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Dispatch window message to message handlers.
    /// </summary>
    /// <param name="hWnd">Handle to window</param>
    /// <param name="uMsg">Message type</param>
    /// <param name="wParam">Extra message parameter</param>
    /// <param name="lParam">Extra message parameter</param>
    /// <returns>
    /// If message is handled, non-zero is returned. Otherwise FALSE is returned and message is passed to default dialog procedure
    /// </returns>
    virtual LRESULT DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    /// <summary>
    /// Returns the ID of the dialog
    /// </summary>
    /// <returns>ID of dialog</returns>
    virtual UINT GetDlgId() = 0;

    /// <summary>
    /// Modifie l'icon de la fenètre
    /// </summary>
    /// <param name="hWnd">Handle de la fenètre</param>
    static void SetIcon(HWND hWnd);

    /// <summary>
    /// Modifie l'icon d'un sous-menu
    /// </summary>
    /// <param name="MenuId">ID du menu</param>
    /// <param name="SubmenuId">ID du sous-menu</param>
    /// <param name="IconId">ID de l'icon</param>
    void SetMenuIcon(int MenuId, int SubmenuId, int IconId);
};

#endif // GUI_MANAGER_HPP