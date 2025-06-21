#include "DpThemeClient.h"
#include <wx/jsonval.h>
#include <wx/jsonreader.h>
#include <wx/jsonwriter.h>
#include <wx/fileconf.h>

// Définition de l'événement
wxDEFINE_EVENT(EVT_DPTHEME_CHANGED, wxCommandEvent);

DpThemeClient& DpThemeClient::Instance() {
    static DpThemeClient instance;
    return instance;
}

void DpThemeClient::Init(const wxString& pluginName, const DpThemeClientCallbacks& callbacks) {
    m_pluginName = pluginName;
    m_callbacks = callbacks;
    m_initialized = true;
    
    // Charger depuis la config locale
    LoadFromConfig();
    
    // Demander le thème actuel au plugin principal
    RequestCurrentTheme();
}

void DpThemeClient::RequestCurrentTheme() {
    if (!m_initialized || !m_callbacks.sendMessage) return;
    
    // Construire la requête JSON
    wxJSONValue request;
    request["type"] = "theme_request";
    request["sender"] = m_pluginName;
    
    wxJSONWriter writer;
    wxString jsonStr;
    writer.Write(request, jsonStr);
    
    m_callbacks.sendMessage("DPTHEME_REQUEST", jsonStr);
}

wxColour DpThemeClient::GetColor(DpColorRole role) const {
    return (m_mode == DpThemeMode::Night) 
        ? m_cachedProfile.night[role] 
        : m_cachedProfile.day[role];
}

void DpThemeClient::HandleThemeMessage(const wxString& message_body) {
    wxLogMessage("DpThemeClient: Handling message: %s", message_body);
    
    wxJSONReader reader;
    wxJSONValue root;
    
    if (reader.Parse(message_body, &root) != 0) {
        wxLogMessage("DpThemeClient: ERROR parsing JSON");
        return;
    }
    
    wxString type = root["type"].AsString();
    wxLogMessage("DpThemeClient: Message type: %s", type);
    
    if (type == "theme_current" || type == "theme_changed") {
        wxString themeName = root["theme"].AsString();
        wxString modeStr = root["mode"].AsString();
        
        wxLogMessage("DpThemeClient: Theme=%s, Mode=%s", themeName, modeStr);
        
        DpThemeMode mode = (modeStr == "night") 
            ? DpThemeMode::Night 
            : DpThemeMode::Day;
        
        ApplyTheme(themeName, mode);
    }
}

void DpThemeClient::ApplyTheme(const wxString& themeName, DpThemeMode mode) {
    // Vérifier si le thème existe
    if (!DpThemeLibrary::ThemeExists(themeName)) {
        return;
    }
    
    // Vérifier si c'est un changement
    bool changed = (m_currentTheme != themeName || m_mode != mode);
    
    // Mettre à jour l'état
    m_currentTheme = themeName;
    m_mode = mode;
    
    // Charger le profil complet depuis la bibliothèque
    m_cachedProfile = DpThemeLibrary::GetTheme(themeName);
    
    // Sauvegarder dans la config
    SaveToConfig();
    
    // Notifier seulement si changement
    if (changed) {
        NotifyThemeChange();
    }
}

void DpThemeClient::NotifyThemeChange() {
     wxLogMessage("DpThemeClient: NotifyThemeChange called, %d callbacks registered", 
                 m_changeCallbacks.size());
    
    // Appeler tous les callbacks enregistrés
    for (auto& callback : m_changeCallbacks) {
        if (callback) {
            callback();
        }
    }
    
    // Envoyer un événement wx
    wxCommandEvent event(EVT_DPTHEME_CHANGED);
    ProcessEvent(event);
}

void DpThemeClient::RegisterCallback(ThemeChangeCallback callback) {
    m_changeCallbacks.push_back(callback);
}

void DpThemeClient::ForceRefresh() {
    NotifyThemeChange();
}

void DpThemeClient::LoadFromConfig() {
    if (!m_callbacks.getConfig) return;
    
    wxFileConfig* config = m_callbacks.getConfig();
    if (!config) return;
    
    wxString oldPath = config->GetPath();
    config->SetPath("/PlugIns/" + m_pluginName);
    
    m_currentTheme = config->Read("Theme", "Ocean");
    wxString modeStr = config->Read("ThemeMode", "day");
    m_mode = (modeStr == "night") ? DpThemeMode::Night : DpThemeMode::Day;
    
    config->SetPath(oldPath);
    
    // Charger le profil
    if (DpThemeLibrary::ThemeExists(m_currentTheme)) {
        m_cachedProfile = DpThemeLibrary::GetTheme(m_currentTheme);
    }
}

void DpThemeClient::SaveToConfig() {
    if (!m_callbacks.getConfig) return;
    
    wxFileConfig* config = m_callbacks.getConfig();
    if (!config) return;
    
    wxString oldPath = config->GetPath();
    config->SetPath("/PlugIns/" + m_pluginName);
    
    config->Write("Theme", m_currentTheme);
    config->Write("ThemeMode", m_mode == DpThemeMode::Night ? "night" : "day");
    
    config->SetPath(oldPath);
    config->Flush();
}