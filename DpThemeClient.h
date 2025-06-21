#pragma once

#include "DpThemes.h"
#include <wx/string.h>
#include <wx/event.h>
#include <functional>

// Forward declaration
class wxFileConfig;

// Événement personnalisé pour le changement de thème
wxDECLARE_EVENT(EVT_DPTHEME_CHANGED, wxCommandEvent);

/**
 * @brief Interface pour les fonctions OpenCPN
 */
struct DpThemeClientCallbacks {
    std::function<void(const wxString&, const wxString&)> sendMessage;
    std::function<wxFileConfig*()> getConfig;
};

/**
 * @brief Classe de base pour les clients de thème
 */
class DpThemeClient : public wxEvtHandler {
public:
    // Type de callback pour les changements de thème
    using ThemeChangeCallback = std::function<void()>;
    
    static DpThemeClient& Instance();
    
    // Initialisation avec les callbacks OpenCPN
    void Init(const wxString& pluginName, const DpThemeClientCallbacks& callbacks);
    
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
    
    // Forcer un refresh
    void ForceRefresh();
    
protected:
    DpThemeClient() = default;
    virtual ~DpThemeClient() = default;
    
private:
    wxString m_pluginName;
    wxString m_currentTheme = "Ocean";
    DpThemeMode m_mode = DpThemeMode::Day;
    bool m_initialized = false;
    
    // Callbacks vers OpenCPN
    DpThemeClientCallbacks m_callbacks;
    
    // Cache local des couleurs actuelles
    DpThemeProfile m_cachedProfile;
    
    // Callbacks enregistrés pour les changements
    std::vector<ThemeChangeCallback> m_changeCallbacks;
    
    void ApplyTheme(const wxString& themeName, DpThemeMode mode);
    void NotifyThemeChange();
    void LoadFromConfig();
    void SaveToConfig();
};