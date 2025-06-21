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
// PARTIE 1 : Définitions communes (IDENTIQUES dans GUI et Radar)
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
};

// ============================================================================
// PARTIE 2 : Côté PLUGIN GUI uniquement
// ============================================================================

#ifdef DP_GUI_PLUGIN

#include "DpThemeManager.h"

class DpThemesAPI : public IDpThemeProvider {
public:
    static DpThemesAPI& Instance() {
        static DpThemesAPI instance;
        return instance;
    }
    
    // Implémentation de l'interface
    wxColour Get(DpColorRole role) const override {
        return DpThemeManager::Instance().Get(role);
    }
    
    DpThemeMode GetMode() const override {
        return DpThemeManager::Instance().GetMode();
    }
    
    // Envoyer le pointeur API aux autres plugins
    static void SendThemeAPIMessage() {
        wxJSONValue root;
        root["type"] = "theme_api";
        
        // Convertir le pointeur en wxULongLong puis en valeur
        wxULongLong apiPtr((wxULongLong_t)&Instance());
        root["api"] = apiPtr.GetValue();
        
        wxJSONWriter writer;
        wxString message;
        writer.Write(root, message);
        
        SendPluginMessage("THEME_API_TO_PLUGINS", message);
        wxLogMessage("GUI: Sent theme API pointer to plugins");
    }
    
    // Notifier un changement de thème
    static void NotifyThemeChanged() {
        wxJSONValue root;
        root["type"] = "theme_changed";
        
        wxJSONWriter writer;
        wxString message;
        writer.Write(root, message);
        
        SendPluginMessage("THEME_UPDATE", message);
        wxLogMessage("GUI: Sent theme change notification");
    }
    
private:
    DpThemesAPI() = default;
};

#endif // DP_GUI_PLUGIN

// ============================================================================
// PARTIE 3 : Côté AUTRES PLUGINS (Radar, etc.)
// ============================================================================

#ifndef DP_GUI_PLUGIN

class DpThemesBridge : public IDpThemeProvider {
public:
    DpThemesBridge() : m_themeAPI(nullptr) {
        // Initialiser avec des couleurs par défaut
        InitDefaultColors();
    }
    
    // Implémentation de l'interface
    wxColour Get(DpColorRole role) const override {
        if (m_themeAPI) {
            return m_themeAPI->Get(role);
        }
        // Utiliser les couleurs par défaut si pas connecté
        auto it = m_defaultColors.find(role);
        return (it != m_defaultColors.end()) ? it->second : wxColour(128, 128, 128);
    }
    
    DpThemeMode GetMode() const override {
        if (m_themeAPI) {
            return m_themeAPI->GetMode();
        }
        return DpThemeMode::Day;
    }
    
    // Traiter les messages du GUI
    void ProcessMessage(const wxString& message_id, const wxString& message_body) {
        if (message_id == "THEME_API_TO_PLUGINS") {
            ProcessThemeAPI(message_body);
        } else if (message_id == "THEME_UPDATE") {
            ProcessThemeUpdate(message_body);
        }
    }
    
    // Vérifier si connecté
    bool IsConnected() const {
        return m_themeAPI != nullptr;
    }
    
    // Ajouter un callback pour les changements de thème
    void AddThemeChangeCallback(std::function<void()> callback) {
        m_callbacks.push_back(callback);
    }
    
private:
    void InitDefaultColors() {
        m_defaultColors[DpColorRole::TextPrimary] = wxColour(255, 255, 255);
        m_defaultColors[DpColorRole::TextSecondary] = wxColour(200, 200, 200);
        m_defaultColors[DpColorRole::TextDisabled] = wxColour(128, 128, 128);
        m_defaultColors[DpColorRole::Background_1] = wxColour(24, 24, 24);
        m_defaultColors[DpColorRole::Background_2] = wxColour(34, 34, 34);
        m_defaultColors[DpColorRole::Background_3] = wxColour(24, 24, 24);
        m_defaultColors[DpColorRole::Background_4] = wxColour(34, 34, 34);
        // ... ajouter d'autres couleurs par défaut si nécessaire
    }
    
    void ProcessThemeAPI(const wxString& message_body) {
        wxJSONReader reader;
        wxJSONValue root;
        if (reader.Parse(message_body, &root) != 0) {
            wxLogMessage("DpThemesBridge: ERROR parsing JSON");
            return;
        }
        
        wxString messageType = root["type"].AsString();
        if (messageType == "theme_api") {
            // Récupérer la valeur comme wxUint64
            wxUint64 apiValue = root["api"].AsUInt64();
            m_themeAPI = reinterpret_cast<IDpThemeProvider*>((void*)apiValue);
            
            wxLogMessage("DpThemesBridge: Connected to theme API");
            
            // Notifier les callbacks
            NotifyCallbacks();
        }
    }
    
    void ProcessThemeUpdate(const wxString& message_body) {
        wxLogMessage("DpThemesBridge: Theme update received");
        NotifyCallbacks();
    }
    
    void NotifyCallbacks() {
        for (const auto& callback : m_callbacks) {
            if (callback) callback();
        }
    }
    
    IDpThemeProvider* m_themeAPI;
    std::unordered_map<DpColorRole, wxColour> m_defaultColors;
    std::vector<std::function<void()>> m_callbacks;
};

// Instance globale pour faciliter l'accès
extern DpThemesBridge* g_dpThemes;

#endif // !DP_GUI_PLUGIN