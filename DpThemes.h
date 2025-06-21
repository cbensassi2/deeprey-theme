#pragma once

#include <unordered_map>
#include <vector>
#include <wx/colour.h>
#include <wx/string.h>

// Version de la bibliothèque
namespace DpThemes {
    constexpr int VERSION_MAJOR = 1;
    constexpr int VERSION_MINOR = 0;
    constexpr int VERSION_PATCH = 0;
}

// Rôles des couleurs dans l'UI
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

// Mode jour/nuit
enum class DpThemeMode { 
    Day, 
    Night 
};

// Palette de couleurs
struct DpPalette {
    std::unordered_map<DpColorRole, wxColour> colors;
    wxColour operator[](DpColorRole r) const;
};

// Profil de thème complet
struct DpThemeProfile {
    wxString name;
    DpPalette day;
    DpPalette night;
};

// Configuration par défaut
namespace DpThemeConfig {
    constexpr auto GROUP = "/Appearance";
    constexpr auto KEY = "Theme";
    const wxString DEFAULT = "Ocean";
}

// Classe statique pour accéder aux thèmes
class DpThemeLibrary {
public:
    // Récupère tous les thèmes disponibles
    static std::vector<DpThemeProfile> GetAllThemes();
    
    // Récupère un thème par son nom
    static DpThemeProfile GetTheme(const wxString& name);
    
    // Récupère la liste des noms de thèmes
    static std::vector<wxString> GetThemeNames();
    
    // Vérifie si un thème existe
    static bool ThemeExists(const wxString& name);
    
    // Récupère une couleur spécifique
    static wxColour GetColor(const wxString& themeName, DpThemeMode mode, DpColorRole role);
    
private:
    // Initialise les thèmes au premier appel
    static void InitThemes();
    static bool initialized_;
    static std::unordered_map<wxString, DpThemeProfile> themes_;
};