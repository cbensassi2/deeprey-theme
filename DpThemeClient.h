#pragma once

#include "DpThemes.h"
#include <wx/string.h>
#include <wx/event.h>
#include <functional>

// Événement personnalisé pour le changement de thème
wxDECLARE_EVENT(EVT_DPTHEME_CHANGED, wxCommandEvent);

/**
 * @brief Classe de base pour les clients de thème
 * 
 * Cette classe peut être utilisée par n'importe quel plugin pour
 * recevoir et appliquer les thèmes de manière synchronisée
 */
class DpThemeClient : public wxEvtHandler {
public:
    // Type de callback pour les changements de thème
    using ThemeChangeCallback = std::function<void()>;
    
    static DpThemeClient& Instance();
    
    // Initialisation (à appeler dans Init() du plugin)
    void Init(const wxString& pluginName);
    
    // Demande le thème actuel au plugin principal
    void RequestCurrentTheme();
    
    // Récupère une couleur
    wxColour GetColor(DpColorRole role) const;
    
    // Getters
    wxString GetCurrentThemeName() const { return m_currentTheme; }
    DpThemeMode GetMode() const { return m_mode; }
    bool IsInitialized() const { return m_initialized; }
    
    // Gestion des messages JSON
    void HandleThemeMessage(const wxString& message_body);
    
    // Enregistrer un callback pour les changements
    void RegisterCallback(ThemeChangeCallback callback);
    
    // Forcer un refresh (utile pour les widgets)
    void ForceRefresh();
    
protected:
    DpThemeClient() = default;
    virtual ~DpThemeClient() = default;
    
private:
    wxString m_pluginName;
    wxString m_currentTheme = "Ocean";  // Défaut
    DpThemeMode m_mode = DpThemeMode::Day;
    bool m_initialized = false;
    
    // Cache local des couleurs actuelles
    DpThemeProfile m_cachedProfile;
    
    // Callbacks enregistrés
    std::vector<ThemeChangeCallback> m_callbacks;
    
    void ApplyTheme(const wxString& themeName, DpThemeMode mode);
    void NotifyThemeChange();
    void LoadFromConfig();
    void SaveToConfig();
};