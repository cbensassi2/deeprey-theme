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
// Interface partagée entre les plugins
// ============================================================================

#include "DpThemeTypes.h"  // Inclure les types partagés

// Interface minimale pour accéder aux thèmes
class IDpThemeProvider {
public:
    virtual ~IDpThemeProvider() = default;
    
    // Accès aux couleurs
    virtual wxColour Get(DpColorRole role) const = 0;
    virtual DpThemeMode GetMode() const = 0;
    virtual wxString GetCurrentThemeName() const = 0;
    
    // Notifications
    virtual uint64_t AddThemeChangeCallback(std::function<void()> callback) = 0;
    virtual void RemoveThemeChangeCallback(uint64_t callbackId) = 0;
};

// ============================================================================
// Côté Plugin GUI (fournisseur de thèmes)
// ============================================================================

#ifdef DP_GUI_PLUGIN  // Défini uniquement dans le plugin GUI

#include "DpThemeManager.h"  // Votre gestionnaire de thèmes existant

class DpThemesAPI : public IDpThemeProvider {
public:
    static DpThemesAPI& Instance() {
        static DpThemesAPI instance;
        return instance;
    }
    
    // IDpThemeProvider implementation
    wxColour Get(DpColorRole role) const override {
        return DpThemeManager::Instance().Get(role);
    }
    
    DpThemeMode GetMode() const override {
        return DpThemeManager::Instance().GetMode();
    }
    
    wxString GetCurrentThemeName() const override {
        // À implémenter dans DpThemeManager si pas déjà fait
        return DpThemeManager::Instance().GetCurrentThemeName();
    }
    
    uint64_t AddThemeChangeCallback(std::function<void()> callback) override {
        uint64_t id = m_nextCallbackId++;
        m_callbacks[id] = callback;
        return id;
    }
    
    void RemoveThemeChangeCallback(uint64_t callbackId) override {
        m_callbacks.erase(callbackId);
    }
    
    // Méthodes spécifiques au GUI
    void NotifyThemeChanged() {
        // Appeler tous les callbacks
        for (const auto& [id, callback] : m_callbacks) {
            if (callback) callback();
        }
        
        // Envoyer un message aux autres plugins
        SendThemeChangedMessage();
    }
    
    void ProcessMessage(const wxString& message_id, const wxString& message_body) {
        if (message_id != "THEME_API_REQUEST") return;
        
        wxJSONReader reader;
        wxJSONValue root;
        if (reader.Parse(message_body, &root) != 0) return;
        
        wxString requestType = root["type"].AsString();
        
        if (requestType == "get_api") {
            SendThemeAPIPointer();
        } else if (requestType == "get_current_theme") {
            SendCurrentTheme();
        }
    }
    
private:
    DpThemesAPI() = default;
    
    void SendThemeAPIPointer() {
        wxJSONValue root;
        root["type"] = "api_pointer";
        root["api"] = reinterpret_cast<wxUint64>(this);
        
        wxJSONWriter writer;
        wxString message;
        writer.Write(root, message);
        
        SendPluginMessage("THEME_API_RESPONSE", message);
    }
    
    void SendThemeChangedMessage() {
        wxJSONValue root;
        root["type"] = "theme_changed";
        root["theme_name"] = GetCurrentThemeName();
        root["mode"] = (GetMode() == DpThemeMode::Day) ? "day" : "night";
        
        // Optionnel : envoyer toutes les couleurs pour le cache
        wxJSONValue colors;
        for (int i = 0; i < static_cast<int>(DpColorRole::HighlightDisabled) + 1; ++i) {
            DpColorRole role = static_cast<DpColorRole>(i);
            wxColour color = Get(role);
            colors[std::to_string(i)]["r"] = color.Red();
            colors[std::to_string(i)]["g"] = color.Green();
            colors[std::to_string(i)]["b"] = color.Blue();
        }
        root["colors"] = colors;
        
        wxJSONWriter writer;
        wxString message;
        writer.Write(root, message);
        
        SendPluginMessage("THEME_UPDATE", message);
    }
    
    void SendCurrentTheme() {
        SendThemeChangedMessage();  // Réutilise la même logique
    }
    
    std::unordered_map<uint64_t, std::function<void()>> m_callbacks;
    uint64_t m_nextCallbackId = 1;
};

#endif // DP_GUI_PLUGIN

// ============================================================================
// Côté autres plugins (consommateurs de thèmes)
// ============================================================================

class DpThemesBridge : public IDpThemeProvider {
public:
    DpThemesBridge() : m_themeAPI(nullptr), m_nextCallbackId(1) {
        // Initialiser avec des couleurs par défaut
        InitDefaultColors();
    }
    
    // IDpThemeProvider implementation
    wxColour Get(DpColorRole role) const override {
        if (m_themeAPI) {
            // Accès direct via l'API
            return m_themeAPI->Get(role);
        } else {
            // Utiliser le cache local
            auto it = m_colorCache.find(role);
            return (it != m_colorCache.end()) ? it->second : wxColour(128, 128, 128);
        }
    }
    
    DpThemeMode GetMode() const override {
        if (m_themeAPI) {
            return m_themeAPI->GetMode();
        }
        return m_cachedMode;
    }
    
    wxString GetCurrentThemeName() const override {
        if (m_themeAPI) {
            return m_themeAPI->GetCurrentThemeName();
        }
        return m_cachedThemeName;
    }
    
    uint64_t AddThemeChangeCallback(std::function<void()> callback) override {
        uint64_t id = m_nextCallbackId++;
        m_localCallbacks[id] = callback;
        
        // Si on a l'API, enregistrer aussi là-bas
        if (m_themeAPI) {
            m_apiCallbackIds[id] = m_themeAPI->AddThemeChangeCallback(callback);
        }
        
        return id;
    }
    
    void RemoveThemeChangeCallback(uint64_t callbackId) override {
        m_localCallbacks.erase(callbackId);
        
        // Si on a l'API, désenregistrer aussi
        if (m_themeAPI && m_apiCallbackIds.count(callbackId)) {
            m_themeAPI->RemoveThemeChangeCallback(m_apiCallbackIds[callbackId]);
            m_apiCallbackIds.erase(callbackId);
        }
    }
    
    // Méthodes de synchronisation
    void RequestThemeAPI() {
        wxJSONValue root;
        root["type"] = "get_api";
        
        wxJSONWriter writer;
        wxString message;
        writer.Write(root, message);
        
        SendPluginMessage("THEME_API_REQUEST", message);
    }
    
    void ProcessMessage(const wxString& message_id, const wxString& message_body) {
        if (message_id == "THEME_API_RESPONSE") {
            ProcessAPIResponse(message_body);
        } else if (message_id == "THEME_UPDATE") {
            ProcessThemeUpdate(message_body);
        }
    }
    
    bool IsConnected() const {
        return m_themeAPI != nullptr;
    }
    
    void OnConnected(std::function<void()> callback) {
        if (IsConnected()) {
            callback();
        } else {
            m_onConnectedCallbacks.push_back(callback);
        }
    }
    
private:
    void InitDefaultColors() {
        // Couleurs par défaut en cas d'absence de connexion
        m_colorCache[DpColorRole::TextPrimary] = wxColour(255, 255, 255);
        m_colorCache[DpColorRole::TextSecondary] = wxColour(200, 200, 200);
        m_colorCache[DpColorRole::TextDisabled] = wxColour(128, 128, 128);
        m_colorCache[DpColorRole::Background_1] = wxColour(24, 24, 24);
        m_colorCache[DpColorRole::Background_2] = wxColour(34, 34, 34);
        // ... autres couleurs par défaut
    }
    
    void ProcessAPIResponse(const wxString& message_body) {
        wxJSONReader reader;
        wxJSONValue root;
        if (reader.Parse(message_body, &root) != 0) return;
        
        wxString responseType = root["type"].AsString();
        
        if (responseType == "api_pointer") {
            m_themeAPI = reinterpret_cast<IDpThemeProvider*>(root["api"].AsUInt64());
            
            // Réenregistrer tous les callbacks existants
            for (const auto& [id, callback] : m_localCallbacks) {
                if (callback) {
                    m_apiCallbackIds[id] = m_themeAPI->AddThemeChangeCallback(callback);
                }
            }
            
            // Notifier la connexion
            for (const auto& callback : m_onConnectedCallbacks) {
                if (callback) callback();
            }
            m_onConnectedCallbacks.clear();
            
            // Rafraîchir l'UI
            NotifyThemeChanged();
        }
    }
    
    void ProcessThemeUpdate(const wxString& message_body) {
        wxJSONReader reader;
        wxJSONValue root;
        if (reader.Parse(message_body, &root) != 0) return;
        
        // Mettre à jour le cache
        m_cachedThemeName = root["theme_name"].AsString();
        m_cachedMode = (root["mode"].AsString() == "day") ? DpThemeMode::Day : DpThemeMode::Night;
        
        // Mettre à jour les couleurs si fournies
        if (root.HasMember("colors")) {
            wxJSONValue colors = root["colors"];
            for (int i = 0; i < static_cast<int>(DpColorRole::HighlightDisabled) + 1; ++i) {
                wxString key = std::to_string(i);
                if (colors.HasMember(key)) {
                    int r = colors[key]["r"].AsInt();
                    int g = colors[key]["g"].AsInt();
                    int b = colors[key]["b"].AsInt();
                    m_colorCache[static_cast<DpColorRole>(i)] = wxColour(r, g, b);
                }
            }
        }
        
        // Notifier les changements
        NotifyThemeChanged();
    }
    
    void NotifyThemeChanged() {
        for (const auto& [id, callback] : m_localCallbacks) {
            if (callback) callback();
        }
    }
    
    IDpThemeProvider* m_themeAPI;
    std::unordered_map<DpColorRole, wxColour> m_colorCache;
    wxString m_cachedThemeName;
    DpThemeMode m_cachedMode = DpThemeMode::Day;
    
    std::unordered_map<uint64_t, std::function<void()>> m_localCallbacks;
    std::unordered_map<uint64_t, uint64_t> m_apiCallbackIds;
    uint64_t m_nextCallbackId;
    
    std::vector<std::function<void()>> m_onConnectedCallbacks;
};

// Global pour l'accès facile
extern DpThemesBridge* g_dpThemes;