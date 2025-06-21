#pragma once

#include <wx/wx.h>
#include <wx/jsonval.h>
#include <wx/jsonreader.h>
#include <wx/jsonwriter.h>
#include <functional>
#include <unordered_map>
#include <vector>
#include <cstdint>

// ============================================================================
// PARTIE 1 : Définitions communes (IDENTIQUES dans GUI et tous les plugins)
// ============================================================================

enum class DpColorRole {
    // Texte
    TextPrimary,
    TextPrimary_Selected,
    TextSecondary,
    TextDisabled,
    
    // Fonds
    Background_1,
    Background_2,
    Background_3,
    Background_4,
    Background_rail,
    
    // Border
    Border_1,
    Border_2,
    Border_3,
    Border_4,
    
    // Éléments actifs
    HighlightPrimary,
    HighlightSecondary,
    HighlightDisabled
};

enum class DpThemeMode { 
    Day, 
    Night 
};

// Interface minimale
class IDpThemeProvider {
public:
    virtual ~IDpThemeProvider() = default;
    virtual wxColour Get(DpColorRole role) const = 0;
    virtual DpThemeMode GetMode() const = 0;
    virtual wxString GetCurrentThemeName() const = 0;
    virtual std::vector<wxString> GetThemeNames() const = 0;
};

// ============================================================================
// PARTIE 2 : Côté PLUGIN GUI (DeepreyGui_pi)
// ============================================================================

#ifdef DP_GUI_PLUGIN

#include "DpThemeManager.h"

class DpThemeAPI : public IDpThemeProvider {
public:
    static DpThemeAPI& Instance() {
        static DpThemeAPI instance;
        return instance;
    }
    
    // Implémentation de l'interface
    wxColour Get(DpColorRole role) const override {
        return DpThemeManager::Instance().Get(role);
    }
    
    DpThemeMode GetMode() const override {
        return DpThemeManager::Instance().GetMode();
    }
    
    wxString GetCurrentThemeName() const override {
        return DpThemeManager::Instance().GetCurrentThemeName();
    }
    
    std::vector<wxString> GetThemeNames() const override {
        return DpThemeManager::Instance().GetThemeNames();
    }
    
    // Initialiser et annoncer la disponibilité
    static void Initialize() {
        // S'annoncer immédiatement
        SendThemeAPIPointer();
        
        // Écouter les demandes des autres plugins
        // (sera géré dans SetPluginMessage)
    }
    
    // Envoyer le pointeur API aux autres plugins
    static void SendThemeAPIPointer() {
        wxJSONValue root;
        root["type"] = "theme_api_available";
        root["api"] = (wxUint64)&Instance();
        root["theme"] = Instance().GetCurrentThemeName();
        root["mode"] = (Instance().GetMode() == DpThemeMode::Night) ? "night" : "day";
        
        wxJSONWriter writer;
        wxString message;
        writer.Write(root, message);
        
        SendPluginMessage("DP_THEME_BROADCAST", message);
        wxLogMessage("DpThemeAPI: Broadcasting theme API availability");
    }
    
    // Répondre à une demande spécifique
    static void RespondToPing() {
        SendThemeAPIPointer();
    }
    
    // Notifier un changement de thème
    static void NotifyThemeChanged() {
        wxJSONValue root;
        root["type"] = "theme_changed";
        root["theme"] = Instance().GetCurrentThemeName();
        root["mode"] = (Instance().GetMode() == DpThemeMode::Night) ? "night" : "day";
        
        wxJSONWriter writer;
        wxString message;
        writer.Write(root, message);
        
        SendPluginMessage("DP_THEME_BROADCAST", message);
        wxLogMessage("DpThemeAPI: Broadcasting theme change");
    }
    
private:
    DpThemeAPI() = default;
};

#endif // DP_GUI_PLUGIN

// ============================================================================
// PARTIE 3 : Côté AUTRES PLUGINS (Radar, etc.)
// ============================================================================

#ifndef DP_GUI_PLUGIN

// Global pointer accessible depuis le plugin
extern IDpThemeProvider* g_dpThemeAPI;

class DpThemesBridge {
public:
    DpThemesBridge(std::function<void()> onThemeApiAvailable = nullptr) 
        : m_onThemeApiAvailable(onThemeApiAvailable) {
        // Initialiser avec des couleurs par défaut
        InitDefaultColors();
        
        // Demander l'API immédiatement
        RequestThemeAPI();
        
        // Planifier des demandes périodiques jusqu'à connexion
        StartConnectionTimer();
    }
    
    ~DpThemesBridge() {
        StopConnectionTimer();
    }
    
    // Traiter les messages
    bool ProcessMessage(const wxString& message_id, const wxString& message_body) {
        if (message_id == "DP_THEME_BROADCAST") {
            return ProcessThemeBroadcast(message_body);
        }
        return false;
    }
    
    // Obtenir la couleur (utilise l'API si disponible, sinon les défauts)
    wxColour Get(DpColorRole role) const {
        if (g_dpThemeAPI) {
            return g_dpThemeAPI->Get(role);
        }
        auto it = m_defaultColors.find(role);
        return (it != m_defaultColors.end()) ? it->second : wxColour(128, 128, 128);
    }
    
    // Obtenir le mode actuel
    DpThemeMode GetMode() const {
        if (g_dpThemeAPI) {
            return g_dpThemeAPI->GetMode();
        }
        return m_currentMode;
    }
    
    // Vérifier si connecté au GUI
    bool IsConnected() const {
        return g_dpThemeAPI != nullptr;
    }
    
    // Ajouter un callback pour les changements de thème
    uint64_t AddThemeChangeCallback(std::function<void()> callback) {
        uint64_t id = m_nextCallbackId++;
        m_callbacks[id] = callback;
        return id;
    }
    
    // Retirer un callback
    void RemoveThemeChangeCallback(uint64_t callbackId) {
        m_callbacks.erase(callbackId);
    }
    
    // Forcer une nouvelle demande d'API
    void RetryConnection() {
        if (!IsConnected()) {
            RequestThemeAPI();
        }
    }
    
private:
    void RequestThemeAPI() {
        wxJSONValue root;
        root["type"] = "request_theme_api";
        root["plugin"] = "client"; // Peut être personnalisé avec le nom du plugin
        
        wxJSONWriter writer;
        wxString message;
        writer.Write(root, message);
        
        SendPluginMessage("DP_THEME_BROADCAST", message);
        wxLogMessage("DpThemesBridge: Requesting theme API");
    }
    
    void StartConnectionTimer() {
        m_connectionTimer.Bind(wxEVT_TIMER, &DpThemesBridge::OnConnectionTimer, this);
        m_connectionTimer.Start(2000); // Réessayer toutes les 2 secondes
    }
    
    void StopConnectionTimer() {
        if (m_connectionTimer.IsRunning()) {
            m_connectionTimer.Stop();
        }
    }
    
    void OnConnectionTimer(wxTimerEvent& event) {
        if (!IsConnected()) {
            RequestThemeAPI();
        } else {
            // Connecté, arrêter le timer
            StopConnectionTimer();
        }
    }
    
    void InitDefaultColors() {
        // Couleurs par défaut Dark theme
        m_defaultColors[DpColorRole::TextPrimary] = wxColour(255, 255, 255);
        m_defaultColors[DpColorRole::TextPrimary_Selected] = wxColour(255, 255, 255);
        m_defaultColors[DpColorRole::TextSecondary] = wxColour(200, 200, 200);
        m_defaultColors[DpColorRole::TextDisabled] = wxColour(67, 67, 67);
        
        m_defaultColors[DpColorRole::Background_1] = wxColour(24, 24, 24);
        m_defaultColors[DpColorRole::Background_2] = wxColour(34, 34, 34);
        m_defaultColors[DpColorRole::Background_3] = wxColour(24, 24, 24);
        m_defaultColors[DpColorRole::Background_4] = wxColour(34, 34, 34);
        m_defaultColors[DpColorRole::Background_rail] = wxColour(0, 0, 0);
        
        m_defaultColors[DpColorRole::Border_1] = wxColour(48, 49, 170);
        m_defaultColors[DpColorRole::Border_2] = wxColour(32, 134, 12);
        m_defaultColors[DpColorRole::Border_3] = wxColour(22, 247, 251);
        m_defaultColors[DpColorRole::Border_4] = wxColour(172, 6, 154);
        
        m_defaultColors[DpColorRole::HighlightPrimary] = wxColour(35, 110, 255);
        m_defaultColors[DpColorRole::HighlightSecondary] = wxColour(100, 100, 100);
        m_defaultColors[DpColorRole::HighlightDisabled] = wxColour(105, 105, 105);
    }
    
    bool ProcessThemeBroadcast(const wxString& message_body) {
        wxJSONReader reader;
        wxJSONValue root;
        if (reader.Parse(message_body, &root) != 0) {
            wxLogMessage("DpThemesBridge: ERROR parsing JSON");
            return true;
        }
        
        wxString messageType = root["type"].AsString();
        
        if (messageType == "theme_api_available") {
            // Le GUI annonce sa disponibilité
            if (!g_dpThemeAPI) {
                g_dpThemeAPI = reinterpret_cast<IDpThemeProvider*>((void*)root["api"].AsUInt64());
                
                if (g_dpThemeAPI) {
                    wxLogMessage("DpThemesBridge: Connected to theme API");
                    
                    // Mettre à jour le mode
                    wxString mode = root["mode"].AsString();
                    m_currentMode = (mode == "night") ? DpThemeMode::Night : DpThemeMode::Day;
                    
                    // Arrêter le timer de reconnexion
                    StopConnectionTimer();
                    
                    // Appeler le callback de connexion
                    if (m_onThemeApiAvailable) {
                        m_onThemeApiAvailable();
                    }
                    
                    // Notifier tous les callbacks
                    NotifyCallbacks();
                }
            }
        }
        else if (messageType == "request_theme_api") {
            // Un autre plugin demande l'API, on ne fait rien
            // (seul le GUI répond)
        }
        else if (messageType == "theme_changed") {
            // Le thème a changé
            wxString mode = root["mode"].AsString();
            m_currentMode = (mode == "night") ? DpThemeMode::Night : DpThemeMode::Day;
            
            wxLogMessage("DpThemesBridge: Theme changed to %s (mode: %s)", 
                         root["theme"].AsString(), mode);
            
            // Notifier tous les callbacks
            NotifyCallbacks();
        }
        
        return true;
    }
    
    void NotifyCallbacks() {
        for (const auto& [id, callback] : m_callbacks) {
            if (callback) callback();
        }
    }
    
    std::unordered_map<DpColorRole, wxColour> m_defaultColors;
    std::unordered_map<uint64_t, std::function<void()>> m_callbacks;
    uint64_t m_nextCallbackId = 1;
    std::function<void()> m_onThemeApiAvailable;
    wxTimer m_connectionTimer;
    DpThemeMode m_currentMode = DpThemeMode::Day;
};

#endif // !DP_GUI_PLUGIN

// ============================================================================
// PARTIE 4 : Macros utilitaires
// ============================================================================

#ifndef DP_GUI_PLUGIN
    #define DP_THEME_GET(role) (g_dpThemesBridge ? g_dpThemesBridge->Get(role) : wxColour(128, 128, 128))
    #define DP_THEME_MODE() (g_dpThemesBridge ? g_dpThemesBridge->GetMode() : DpThemeMode::Day)
#else
    #define DP_THEME_GET(role) DpThemeAPI::Instance().Get(role)
    #define DP_THEME_MODE() DpThemeAPI::Instance().GetMode()
#endif