#include "DpIcons.h"
#include <wx/window.h>  // Pour wxWindow
#include <wx/font.h>
#include <wx/log.h>
#include <wx/filefn.h>
#include <wx/filename.h>

// Définition des noms de famille Font Awesome
const wxString DpIconManager::kFaFamilyName = "Font Awesome 6 Free Solid";
const wxString DpIconManager::kFaProFamilyName = "Font Awesome 6 Pro Solid";

// Singleton
DpIconManager& DpIconManager::Instance() {
    static DpIconManager instance;
    return instance;
}

// Initialisation
void DpIconManager::Init(const DpIconCallbacks& callbacks) {
    m_callbacks = callbacks;
    m_initialized = true;
}

// Charge la fonte Font Awesome Free
bool DpIconManager::LoadFreeFont() {
    if (!m_initialized || !m_callbacks.getDataPath) {
        wxLogWarning("DpIconManager not initialized properly");
        return false;
    }
    
    wxFileName fn;
    fn.SetPath(m_callbacks.getDataPath());
    fn.AppendDir("data");
    fn.AppendDir("resources");
    fn.SetFullName("Font Awesome 6 Free-Solid-900.otf");
    
    if (!wxFileExists(fn.GetFullPath())) {
        wxLogWarning("Font Awesome Free file not found: %s", fn.GetFullPath());
        return false;
    }
    
    if (!wxFont::AddPrivateFont(fn.GetFullPath())) {
        wxLogWarning("Unable to load Font Awesome Free: %s", fn.GetFullPath());
        return false;
    }
    
    wxLogMessage("Font Awesome Free loaded successfully from: %s", fn.GetFullPath());
    return true;
}

// Charge la fonte Font Awesome Pro
bool DpIconManager::LoadProFont() {
    if (!m_initialized || !m_callbacks.getDataPath) {
        return false;
    }
    
    wxFileName fn;
    fn.SetPath(m_callbacks.getDataPath());
    fn.AppendDir("data");
    fn.AppendDir("resources");
    fn.SetFullName("Font Awesome 6 Pro-Solid-900.otf");
    
    if (!wxFileExists(fn.GetFullPath())) {
        wxLogDebug("Font Awesome Pro file not found: %s", fn.GetFullPath());
        return false;
    }
    
    if (!wxFont::AddPrivateFont(fn.GetFullPath())) {
        wxLogDebug("Unable to load Font Awesome Pro: %s", fn.GetFullPath());
        return false;
    }
    
    m_proFontLoaded = true;
    wxLogMessage("Font Awesome Pro loaded successfully from: %s", fn.GetFullPath());
    return true;
}

// Charge les fontes Font Awesome
bool DpIconManager::LoadIconFont() {
    if (m_fontLoaded) {
        return true;  // Déjà chargée
    }
    
    // Essaie d'abord de charger Font Awesome Pro
    if (LoadProFont()) {
        m_fontLoaded = true;
        // Si on a réussi à charger Pro et que c'est le type courant, on garde Pro
        if (m_currentFontType == DpFontAwesomeType::Pro) {
            return true;
        }
    }
    
    // Charge toujours Font Awesome Free (fallback ou choix explicite)
    if (LoadFreeFont()) {
        m_fontLoaded = true;
        // Si on n'a pas pu charger Pro, force Free
        if (!m_proFontLoaded) {
            m_currentFontType = DpFontAwesomeType::Free;
        }
        return true;
    }
    
    return false;
}

// Définit le type de fonte à utiliser
void DpIconManager::SetFontType(DpFontAwesomeType type) {
    if (type == DpFontAwesomeType::Pro && !m_proFontLoaded) {
        wxLogWarning("Font Awesome Pro not available, falling back to Free");
        m_currentFontType = DpFontAwesomeType::Free;
    } else {
        m_currentFontType = type;
    }
}

// Création d'une police avec mise à l'échelle DPI
wxFont DpIconManager::CreateScaledIconFont(int pointSize, wxWindow* parent) const {
    // S'assurer que la fonte est chargée
    const_cast<DpIconManager*>(this)->LoadIconFont();
    
    // Sélectionne le nom de famille en fonction du type actuel
    wxString familyName = (m_currentFontType == DpFontAwesomeType::Pro && m_proFontLoaded) 
                          ? kFaProFamilyName 
                          : kFaFamilyName;
    
    wxFontInfo info(pointSize);
    info.Family(wxFONTFAMILY_DEFAULT)
        .FaceName(familyName)
        .Weight(wxFONTWEIGHT_NORMAL)
        .Style(wxFONTSTYLE_NORMAL)
        .AntiAliased(true);
    
    wxFont font(info);
    
    // Application du facteur DPI si un parent est fourni
    if (parent) {
        font = font.Scaled(parent->GetDPIScaleFactor());
    }
    
    return font;
}

// API publique
wxFont DpIconManager::GetIconFont(int pointSize, wxWindow* parent) const {
    return CreateScaledIconFont(pointSize, parent);
}

wxString DpIconManager::GetIconGlyph(DpIcon icon) const {
    auto& iconMap = GetIconMap();
    auto it = iconMap.find(icon);
    if (it != iconMap.end()) {
        return it->second;
    }
    // Retourne une icône par défaut (point d'interrogation)
    return wxString::FromUTF8(u8"\uf128");
}

wxString DpIconManager::GetIconName(DpIcon icon) const {
    auto& nameMap = GetNameMap();
    auto it = nameMap.find(icon);
    if (it != nameMap.end()) {
        return it->second;
    }
    return "Unknown";
}

// Map statique des glyphes
std::map<DpIcon, wxString> DpIconManager::GetIconMap() {
    static std::map<DpIcon, wxString> iconMap = {
        // Icônes de navigation et UI
        {DpIcon::Mark,              wxString::FromUTF8(u8"\uf3c5")},  // location-dot
        {DpIcon::NavBar,            wxString::FromUTF8(u8"\uf0c9")},  // bars
        {DpIcon::DayNight,          wxString::FromUTF8(u8"\uf185")},  // sun
        {DpIcon::Views,             wxString::FromUTF8(u8"\uf06e")},  // eye
        {DpIcon::ComboViews,        wxString::FromUTF8(u8"\uf5fd")},  // layer-group
        {DpIcon::Settings,          wxString::FromUTF8(u8"\uf013")},  // gear
        {DpIcon::LegacySettings,    wxString::FromUTF8(u8"\uf0a0")},  // sliders-h
        {DpIcon::RoutesWaypoints,   wxString::FromUTF8(u8"\uf4d7")},  // route
        
        // Icônes de contrôle
        {DpIcon::Close,             wxString::FromUTF8(u8"\uf00d")},  // xmark
        {DpIcon::Plus,              wxString::FromUTF8(u8"\uf067")},  // plus
        {DpIcon::Minus,             wxString::FromUTF8(u8"\uf068")},  // minus
        {DpIcon::ChevronUp,         wxString::FromUTF8(u8"\uf077")},  // chevron-up
        {DpIcon::ChevronDown,       wxString::FromUTF8(u8"\uf078")},  // chevron-down
        {DpIcon::ChevronLeft,       wxString::FromUTF8(u8"\uf053")},  // chevron-left
        {DpIcon::ChevronRight,      wxString::FromUTF8(u8"\uf054")},  // chevron-right
        
        // Icônes d'état
        {DpIcon::Check,             wxString::FromUTF8(u8"\uf00c")},  // check
        {DpIcon::Warning,           wxString::FromUTF8(u8"\uf071")},  // triangle-exclamation
        {DpIcon::Info,              wxString::FromUTF8(u8"\uf05a")},  // circle-info
        {DpIcon::Error,             wxString::FromUTF8(u8"\uf057")},  // circle-xmark
        {DpIcon::Circle,            wxString::FromUTF8(u8"\uf111")},  // circle (cercle plein)
        
        // Icônes diverses
        {DpIcon::Search,            wxString::FromUTF8(u8"\uf002")},  // magnifying-glass
        {DpIcon::Filter,            wxString::FromUTF8(u8"\uf0b0")},  // filter
        {DpIcon::Sort,              wxString::FromUTF8(u8"\uf0dc")},  // sort
        {DpIcon::Refresh,           wxString::FromUTF8(u8"\uf021")},  // arrows-rotate
        {DpIcon::Save,              wxString::FromUTF8(u8"\uf0c7")},  // floppy-disk
        {DpIcon::Open,              wxString::FromUTF8(u8"\uf07c")},  // folder-open
        {DpIcon::Delete,            wxString::FromUTF8(u8"\uf2ed")},  // trash-can
        {DpIcon::Edit,              wxString::FromUTF8(u8"\uf044")},  // pen-to-square
        {DpIcon::Copy,              wxString::FromUTF8(u8"\uf0c5")},  // copy
        {DpIcon::Paste,             wxString::FromUTF8(u8"\uf0ea")},  // clipboard
        
        // Icônes système
        {DpIcon::PowerOff,          wxString::FromUTF8(u8"\uf011")},  // power-off
        {DpIcon::Sleep,             wxString::FromUTF8(u8"\uf186")},  // moon
        {DpIcon::Screenshot,        wxString::FromUTF8(u8"\uf030")},  // camera
        {DpIcon::TouchLock,         wxString::FromUTF8(u8"\uf256")},  // hand
        {DpIcon::Brightness,        wxString::FromUTF8(u8"\uf185")},  // sun
        {DpIcon::Wifi,              wxString::FromUTF8(u8"\uf1eb")},  // wifi
        {DpIcon::Link,              wxString::FromUTF8(u8"\uf0c1")},  // link
        {DpIcon::Sun,               wxString::FromUTF8(u8"\uf185")},  // sun
        {DpIcon::Moon,              wxString::FromUTF8(u8"\uf186")},  // moon
        
        // Nouvelles icônes Pro
        {DpIcon::LocationXmark,     wxString::FromUTF8(u8"\uf60e")},  // location-xmark
        {DpIcon::XmarkLarge,        wxString::FromUTF8(u8"\ue59b")},  // xmark-large
        {DpIcon::GaugeLow,          wxString::FromUTF8(u8"\uf627")},  // gauge-low
        {DpIcon::SidebarFlip,       wxString::FromUTF8(u8"\ue24f")},  // sidebar-flip
        {DpIcon::GridHorizontal,    wxString::FromUTF8(u8"\ue307")},  // grid-horizontal
        {DpIcon::TableLayout,       wxString::FromUTF8(u8"\ue290")},  // table-layout
        {DpIcon::SlidersUp,         wxString::FromUTF8(u8"\uf3f1")},  // sliders-up
        {DpIcon::RectanglesMixed,   wxString::FromUTF8(u8"\ue323")},  // rectangles-mixed
        {DpIcon::PlusLarge,         wxString::FromUTF8(u8"\ue59e")},  // plus-large
        {DpIcon::ListTimeline,      wxString::FromUTF8(u8"\ue1d1")},  // list-timeline
        {DpIcon::HouseDay,          wxString::FromUTF8(u8"\ue00e")},  // house-day
        {DpIcon::BrightnessLow,     wxString::FromUTF8(u8"\ue0ca")},  // brightness-low
        {DpIcon::BrightnessHigh,    wxString::FromUTF8(u8"\ue0c9")},  // brightness
		{DpIcon::RectangleWide,     wxString::FromUTF8(u8"\uf2fc")},  // rectangle-wide
		{DpIcon::Square,            wxString::FromUTF8(u8"\uf0c8")},  // Square
    };
    return iconMap;
}

// Map statique des noms
std::map<DpIcon, wxString> DpIconManager::GetNameMap() {
    static std::map<DpIcon, wxString> nameMap = {
        {DpIcon::Mark,              "Mark"},
        {DpIcon::NavBar,            "NavBar"},
        {DpIcon::DayNight,          "Day/Night"},
        {DpIcon::Views,             "Views"},
        {DpIcon::ComboViews,        "Combo Views"},
        {DpIcon::Settings,          "Settings"},
        {DpIcon::LegacySettings,    "Legacy Settings"},
        {DpIcon::RoutesWaypoints,   "Routes & Waypoints"},
        {DpIcon::Close,             "Close"},
        {DpIcon::Plus,              "Plus"},
        {DpIcon::Minus,             "Minus"},
        {DpIcon::ChevronUp,         "Chevron Up"},
        {DpIcon::ChevronDown,       "Chevron Down"},
        {DpIcon::ChevronLeft,       "Chevron Left"},
        {DpIcon::ChevronRight,      "Chevron Right"},
        {DpIcon::Check,             "Check"},
        {DpIcon::Warning,           "Warning"},
        {DpIcon::Info,              "Info"},
        {DpIcon::Error,             "Error"},
        {DpIcon::Circle,            "Circle"},
        {DpIcon::Search,            "Search"},
        {DpIcon::Filter,            "Filter"},
        {DpIcon::Sort,              "Sort"},
        {DpIcon::Refresh,           "Refresh"},
        {DpIcon::Save,              "Save"},
        {DpIcon::Open,              "Open"},
        {DpIcon::Delete,            "Delete"},
        {DpIcon::Edit,              "Edit"},
        {DpIcon::Copy,              "Copy"},
        {DpIcon::Paste,             "Paste"},
        {DpIcon::PowerOff,          "Power Off"},
        {DpIcon::Sleep,             "Sleep"},
        {DpIcon::Screenshot,        "Screenshot"},
        {DpIcon::TouchLock,         "Touch Lock"},
        {DpIcon::Brightness,        "Brightness"},
        {DpIcon::Wifi,              "Wifi"},
        {DpIcon::Link,              "Link"},
        {DpIcon::Sun,               "Sun"},
        {DpIcon::Moon,              "Moon"},
        {DpIcon::LocationXmark,     "Location X-Mark"},
        {DpIcon::XmarkLarge,        "X-Mark Large"},
        {DpIcon::GaugeLow,          "Gauge Low"},
        {DpIcon::SidebarFlip,       "Sidebar Flip"},
        {DpIcon::GridHorizontal,    "Grid Horizontal"},
        {DpIcon::TableLayout,       "Table Layout"},
        {DpIcon::SlidersUp,         "Sliders Up"},
        {DpIcon::RectanglesMixed,   "Rectangles Mixed"},
        {DpIcon::PlusLarge,         "Plus Large"},
        {DpIcon::ListTimeline,      "List Timeline"},
        {DpIcon::HouseDay,          "House Day"},
        {DpIcon::BrightnessLow,     "Brightness Low"},
        {DpIcon::BrightnessHigh,    "Brightness High"},
		{DpIcon::RectangleWide,     "Rectangle Wide"},
		{DpIcon::Square,    		"Square"},
		
    };
    return nameMap;
}