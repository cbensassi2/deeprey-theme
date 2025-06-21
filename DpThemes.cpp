#include "DpThemes.h"

// Implémentation de DpPalette
wxColour DpPalette::operator[](DpColorRole r) const {
    auto it = colors.find(r);
    return (it != colors.end()) ? it->second : wxColour();
}

// Variables statiques
bool DpThemeLibrary::initialized_ = false;
std::unordered_map<wxString, DpThemeProfile> DpThemeLibrary::themes_;

// Récupère tous les thèmes
std::vector<DpThemeProfile> DpThemeLibrary::GetAllThemes() {
    if (!initialized_) InitThemes();
    
    std::vector<DpThemeProfile> result;
    result.reserve(themes_.size());
    for (const auto& [name, theme] : themes_) {
        result.push_back(theme);
    }
    return result;
}

// Récupère un thème par son nom
DpThemeProfile DpThemeLibrary::GetTheme(const wxString& name) {
    if (!initialized_) InitThemes();
    
    auto it = themes_.find(name);
    if (it != themes_.end()) {
        return it->second;
    }
    // Retourne le thème par défaut si non trouvé
    return themes_[DpThemeConfig::DEFAULT];
}

// Récupère la liste des noms
std::vector<wxString> DpThemeLibrary::GetThemeNames() {
    if (!initialized_) InitThemes();
    
    std::vector<wxString> names;
    names.reserve(themes_.size());
    for (const auto& [name, _] : themes_) {
        names.push_back(name);
    }
    return names;
}

// Vérifie l'existence d'un thème
bool DpThemeLibrary::ThemeExists(const wxString& name) {
    if (!initialized_) InitThemes();
    return themes_.find(name) != themes_.end();
}

// Récupère une couleur spécifique
wxColour DpThemeLibrary::GetColor(const wxString& themeName, DpThemeMode mode, DpColorRole role) {
    if (!initialized_) InitThemes();
    
    auto it = themes_.find(themeName);
    if (it != themes_.end()) {
        return (mode == DpThemeMode::Night) ? it->second.night[role] : it->second.day[role];
    }
    return wxColour();
}

// Initialisation des thèmes
void DpThemeLibrary::InitThemes() {
    if (initialized_) return;
    
    /* ========== THÈME DARK ========== */
    DpThemeProfile dark;
    dark.name = "Dark";
    
    dark.day.colors = {
        {DpColorRole::TextPrimary,          {255,255,255}},
        {DpColorRole::TextPrimary_Selected, {255,255,255}},
        {DpColorRole::TextSecondary,        {200,200,200}},
        {DpColorRole::TextDisabled,         {67,67,67}},
        {DpColorRole::Background_1,         {24,24,24}},
        {DpColorRole::Background_2,         {34,34,34}},
        {DpColorRole::Background_3,         {24,24,24}},
        {DpColorRole::Background_4,         {34,34,34}},
        {DpColorRole::Background_rail,      {0,0,0}},
        {DpColorRole::Border_1,             {48,49,170}},
        {DpColorRole::Border_2,             {32,134,12}},
        {DpColorRole::Border_3,             {22,247,251}},
        {DpColorRole::Border_4,             {172,6,154}},
        {DpColorRole::HighlightPrimary,     {35,110,255}},
        {DpColorRole::HighlightSecondary,   {100,100,100}},
        {DpColorRole::HighlightDisabled,    {105,105,105}}
    };
    
    dark.night.colors = {
        {DpColorRole::TextPrimary,          {100,100,100}},
        {DpColorRole::TextPrimary_Selected, {100,100,100}},
        {DpColorRole::TextSecondary,        {60,60,60}},
        {DpColorRole::TextDisabled,         {25,25,25}},
        {DpColorRole::Background_1,         {6,6,6}},
        {DpColorRole::Background_2,         {8,8,8}},
        {DpColorRole::Background_3,         {6,6,6}},
        {DpColorRole::Background_4,         {8,8,8}},
        {DpColorRole::Background_rail,      {0,0,0}},
        {DpColorRole::Border_1,             {12,12,42}},
        {DpColorRole::Border_2,             {8,33,3}},
        {DpColorRole::Border_3,             {5,60,62}},
        {DpColorRole::Border_4,             {43,1,38}},
        {DpColorRole::HighlightPrimary,     {8,27,63}},
        {DpColorRole::HighlightSecondary,   {30,30,30}},
        {DpColorRole::HighlightDisabled,    {12,12,12}}
    };
    
    /* ========== THÈME DARK CAPSULE ========== */
    DpThemeProfile darkCapsule;
    darkCapsule.name = "Dark capsule";
    darkCapsule.day.colors = dark.day.colors;
    darkCapsule.night.colors = dark.night.colors;
    
    /* ========== THÈME OCEAN ========== */
    DpThemeProfile ocean;
    ocean.name = "Ocean";
    
    ocean.day.colors = {
        {DpColorRole::TextPrimary,          {255,255,255}},
        {DpColorRole::TextPrimary_Selected, {21,37,55}},
        {DpColorRole::TextSecondary,        {200,200,200}},
        {DpColorRole::TextDisabled,         {105,105,105}},
        {DpColorRole::Background_1,         {21,37,55}},
        {DpColorRole::Background_2,         {18,32,47}},
        {DpColorRole::Background_3,         {21,37,55}},
        {DpColorRole::Background_4,         {38,60,84}},
        {DpColorRole::Background_rail,      {10,18,28}},
        {DpColorRole::Border_1,             {48,49,170}},
        {DpColorRole::Border_2,             {32,134,12}},
        {DpColorRole::Border_3,             {22,247,251}},
        {DpColorRole::Border_4,             {172,6,154}},
        {DpColorRole::HighlightPrimary,     {11,197,209}},
        {DpColorRole::HighlightSecondary,   {100,100,100}},
        {DpColorRole::HighlightDisabled,    {60,60,60}}
    };
    
    ocean.night.colors = {
        {DpColorRole::TextPrimary,          {100,100,100}},
        {DpColorRole::TextPrimary_Selected, {8,14,20}},
        {DpColorRole::TextSecondary,        {60,60,60}},
        {DpColorRole::TextDisabled,         {25,25,25}},
        {DpColorRole::Background_1,         {5,9,13}},
        {DpColorRole::Background_2,         {4,7,10}},
        {DpColorRole::Background_3,         {5,9,13}},
        {DpColorRole::Background_4,         {9,15,21}},
        {DpColorRole::Background_rail,      {3,6,9}},
        {DpColorRole::Border_1,             {12,12,42}},
        {DpColorRole::Border_2,             {8,33,3}},
        {DpColorRole::Border_3,             {5,60,62}},
        {DpColorRole::Border_4,             {43,1,38}},
        {DpColorRole::HighlightPrimary,     {3,49,52}},
        {DpColorRole::HighlightSecondary,   {30,30,30}},
        {DpColorRole::HighlightDisabled,    {11,11,11}}
    };
    
    /* ========== THÈME ARCTIC ========== */
    DpThemeProfile arctic;
    arctic.name = "Arctic";
    
    arctic.day.colors = {
        {DpColorRole::TextPrimary,          {255,255,255}},
        {DpColorRole::TextPrimary_Selected, {15,45,75}},
        {DpColorRole::TextSecondary,        {200,200,200}},
        {DpColorRole::TextDisabled,         {18,18,18}},
        {DpColorRole::Background_1,         {15,45,75}},
        {DpColorRole::Background_2,         {12,36,60}},
        {DpColorRole::Background_3,         {15,45,75}},
        {DpColorRole::Background_4,         {25,55,85}},
        {DpColorRole::Background_rail,      {10,30,50}},
        {DpColorRole::Border_1,             {0,120,255}},
        {DpColorRole::Border_2,             {0,255,255}},
        {DpColorRole::Border_3,             {100,200,255}},
        {DpColorRole::Border_4,             {200,100,255}},
        {DpColorRole::HighlightPrimary,     {0,180,255}},
        {DpColorRole::HighlightSecondary,   {100,100,100}},
        {DpColorRole::HighlightDisabled,    {145,145,145}}
    };
    
    arctic.night.colors = {
        {DpColorRole::TextPrimary,          {100,100,100}},
        {DpColorRole::TextPrimary_Selected, {3,11,18}},
        {DpColorRole::TextSecondary,        {60,60,60}},
        {DpColorRole::TextDisabled,         {25,25,25}},
        {DpColorRole::Background_1,         {3,11,18}},
        {DpColorRole::Background_2,         {3,9,15}},
        {DpColorRole::Background_3,         {3,11,18}},
        {DpColorRole::Background_4,         {6,13,21}},
        {DpColorRole::Background_rail,      {2,7,12}},
        {DpColorRole::Border_1,             {0,30,63}},
        {DpColorRole::Border_2,             {0,63,63}},
        {DpColorRole::Border_3,             {25,50,63}},
        {DpColorRole::Border_4,             {50,25,63}},
        {DpColorRole::HighlightPrimary,     {0,45,63}},
        {DpColorRole::HighlightSecondary,   {30,30,30}},
        {DpColorRole::HighlightDisabled,    {36,36,36}}
    };
    
    /* ========== THÈME SUNSET ========== */
    DpThemeProfile sunset;
    sunset.name = "Sunset";
    
    sunset.day.colors = {
        {DpColorRole::TextPrimary,          {255,255,255}},
        {DpColorRole::TextPrimary_Selected, {80,30,20}},
        {DpColorRole::TextSecondary,        {200,200,200}},
        {DpColorRole::TextDisabled,         {18,18,18}},
        {DpColorRole::Background_1,         {80,30,20}},
        {DpColorRole::Background_2,         {60,25,18}},
        {DpColorRole::Background_3,         {80,30,20}},
        {DpColorRole::Background_4,         {100,40,30}},
        {DpColorRole::Background_rail,      {40,15,10}},
        {DpColorRole::Border_1,             {255,100,0}},
        {DpColorRole::Border_2,             {255,200,0}},
        {DpColorRole::Border_3,             {255,120,50}},
        {DpColorRole::Border_4,             {200,50,100}},
        {DpColorRole::HighlightPrimary,     {255,140,0}},
        {DpColorRole::HighlightSecondary,   {100,100,100}},
        {DpColorRole::HighlightDisabled,    {145,145,145}}
    };
    
    sunset.night.colors = {
        {DpColorRole::TextPrimary,          {100,100,100}},
        {DpColorRole::TextPrimary_Selected, {20,7,5}},
        {DpColorRole::TextSecondary,        {60,60,60}},
        {DpColorRole::TextDisabled,         {25,25,25}},
        {DpColorRole::Background_1,         {20,7,5}},
        {DpColorRole::Background_2,         {15,6,4}},
        {DpColorRole::Background_3,         {20,7,5}},
        {DpColorRole::Background_4,         {25,10,7}},
        {DpColorRole::Background_rail,      {10,3,2}},
        {DpColorRole::Border_1,             {63,25,0}},
        {DpColorRole::Border_2,             {63,50,0}},
        {DpColorRole::Border_3,             {63,30,12}},
        {DpColorRole::Border_4,             {50,12,25}},
        {DpColorRole::HighlightPrimary,     {63,35,0}},
        {DpColorRole::HighlightSecondary,   {30,30,30}},
        {DpColorRole::HighlightDisabled,    {36,36,36}}
    };
    
    /* ========== THÈME DEEPSEA ========== */
    DpThemeProfile deepsea;
    deepsea.name = "DeepSea";
    
    deepsea.day.colors = {
        {DpColorRole::TextPrimary,          {255,255,255}},
        {DpColorRole::TextPrimary_Selected, {10,50,40}},
        {DpColorRole::TextSecondary,        {200,200,200}},
        {DpColorRole::TextDisabled,         {18,18,18}},
        {DpColorRole::Background_1,         {10,50,40}},
        {DpColorRole::Background_2,         {8,40,32}},
        {DpColorRole::Background_3,         {10,50,40}},
        {DpColorRole::Background_4,         {15,60,48}},
        {DpColorRole::Background_rail,      {5,25,20}},
        {DpColorRole::Border_1,             {0,200,150}},
        {DpColorRole::Border_2,             {0,255,100}},
        {DpColorRole::Border_3,             {0,255,200}},
        {DpColorRole::Border_4,             {100,200,255}},
        {DpColorRole::HighlightPrimary,     {0,220,180}},
        {DpColorRole::HighlightSecondary,   {100,100,100}},
        {DpColorRole::HighlightDisabled,    {145,145,145}}
    };
    
    deepsea.night.colors = {
        {DpColorRole::TextPrimary,          {100,100,100}},
        {DpColorRole::TextPrimary_Selected, {2,12,10}},
        {DpColorRole::TextSecondary,        {60,60,60}},
        {DpColorRole::TextDisabled,         {25,25,25}},
        {DpColorRole::Background_1,         {2,12,10}},
        {DpColorRole::Background_2,         {2,10,8}},
        {DpColorRole::Background_3,         {2,12,10}},
        {DpColorRole::Background_4,         {3,15,12}},
        {DpColorRole::Background_rail,      {1,6,5}},
        {DpColorRole::Border_1,             {0,50,37}},
        {DpColorRole::Border_2,             {0,63,25}},
        {DpColorRole::Border_3,             {0,63,50}},
        {DpColorRole::Border_4,             {25,50,63}},
        {DpColorRole::HighlightPrimary,     {0,55,45}},
        {DpColorRole::HighlightSecondary,   {30,30,30}},
        {DpColorRole::HighlightDisabled,    {36,36,36}}
    };
    
    /* ========== THÈME STORM ========== */
    DpThemeProfile storm;
    storm.name = "Storm";
    
    storm.day.colors = {
        {DpColorRole::TextPrimary,          {255,255,255}},
        {DpColorRole::TextPrimary_Selected, {40,35,50}},
        {DpColorRole::TextSecondary,        {200,200,200}},
        {DpColorRole::TextDisabled,         {18,18,18}},
        {DpColorRole::Background_1,         {40,35,50}},
        {DpColorRole::Background_2,         {32,28,40}},
        {DpColorRole::Background_3,         {40,35,50}},
        {DpColorRole::Background_4,         {48,42,60}},
        {DpColorRole::Background_rail,      {20,17,25}},
        {DpColorRole::Border_1,             {150,100,255}},
        {DpColorRole::Border_2,             {100,150,255}},
        {DpColorRole::Border_3,             {180,100,255}},
        {DpColorRole::Border_4,             {255,100,200}},
        {DpColorRole::HighlightPrimary,     {160,120,255}},
        {DpColorRole::HighlightSecondary,   {100,100,100}},
        {DpColorRole::HighlightDisabled,    {145,145,145}}
    };
    
    storm.night.colors = {
        {DpColorRole::TextPrimary,          {100,100,100}},
        {DpColorRole::TextPrimary_Selected, {10,8,12}},
        {DpColorRole::TextSecondary,        {60,60,60}},
        {DpColorRole::TextDisabled,         {25,25,25}},
        {DpColorRole::Background_1,         {10,8,12}},
        {DpColorRole::Background_2,         {8,7,10}},
        {DpColorRole::Background_3,         {10,8,12}},
        {DpColorRole::Background_4,         {12,10,15}},
        {DpColorRole::Background_rail,      {5,4,6}},
        {DpColorRole::Border_1,             {37,25,63}},
        {DpColorRole::Border_2,             {25,37,63}},
        {DpColorRole::Border_3,             {45,25,63}},
        {DpColorRole::Border_4,             {63,25,50}},
        {DpColorRole::HighlightPrimary,     {40,30,63}},
        {DpColorRole::HighlightSecondary,   {30,30,30}},
        {DpColorRole::HighlightDisabled,    {36,36,36}}
    };
    
    // Enregistrement des thèmes
    themes_[dark.name] = dark;
    themes_[darkCapsule.name] = darkCapsule;
    themes_[ocean.name] = ocean;
    themes_[arctic.name] = arctic;
    themes_[sunset.name] = sunset;
    themes_[deepsea.name] = deepsea;
    themes_[storm.name] = storm;
    
    initialized_ = true;
}