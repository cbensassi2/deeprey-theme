#pragma once

#include <wx/string.h>
#include <wx/font.h>
#include <wx/filename.h>
#include <map>
#include <functional>

// Forward declaration
class wxWindow;

/**
 * @brief Type de fonte Font Awesome
 */
enum class DpFontAwesomeType {
    Free,   // Font Awesome 6 Free Solid
    Pro     // Font Awesome 6 Pro Solid
};

/**
 * @brief Énumération des icônes disponibles
 */
enum class DpIcon {
    // Icônes de navigation et UI
    Mark,              // Marqueur/Repère
    NavBar,            // Barres de navigation
    DayNight,          // Soleil/Lune
    Views,             // Oeil (vues simples)
    ComboViews,        // Groupe de couches
    Settings,          // Paramètres
    LegacySettings,    // Anciens paramètres
    RoutesWaypoints,   // Routes et waypoints
    
    // Icônes de contrôle
    Close,             // X de fermeture
    Plus,              // Ajout
    Minus,             // Suppression
    ChevronUp,         // Flèche haut
    ChevronDown,       // Flèche bas
    ChevronLeft,       // Flèche gauche
    ChevronRight,      // Flèche droite
    
    // Icônes d'état
    Check,             // Coche de validation
    Warning,           // Triangle d'avertissement
    Info,              // Information
    Error,             // Erreur
    Circle,            // Circle
    
    // Icônes diverses
    Search,            // Recherche
    Filter,            // Filtre
    Sort,              // Tri
    Refresh,           // Actualiser
    Save,              // Sauvegarder
    Open,              // Ouvrir
    Delete,            // Supprimer
    Edit,              // Éditer
    Copy,              // Copier
    Paste,             // Coller
    
    // Icônes système
    PowerOff,          // Bouton d'arrêt
    Sleep,             // Mode veille
    Screenshot,        // Capture d'écran
    TouchLock,         // Verrouillage tactile
    Brightness,        // Luminosité
    Wifi,              // Sans fil
    Link,              // Lien/Connexion
    Sun,               // Soleil (mode jour)
    Moon,              // Lune (mode nuit)
    
    // Nouvelles icônes Pro
    LocationXmark,     // location-xmark (alternative à Mark)
    XmarkLarge,        // xmark-large (alternative à Close)
    GaugeLow,          // gauge-low (alternative à NavBar)
    SidebarFlip,       // sidebar-flip (menu latéral)
    GridHorizontal,    // grid-horizontal (menu accueil)
    TableLayout,       // table-layout (combo alternatif)
    SlidersUp,         // sliders-up (contrôle système)
    RectanglesMixed,   // rectangles-mixed (combo 2)
    PlusLarge,         // plus-large (bouton plus)
    ListTimeline,      // list-timeline (panneau)
    HouseDay,          // house-day (contrôle système 2)
    BrightnessLow,     // brightness-low (luminosité faible)
    BrightnessHigh,    // brightness (luminosité haute)
	RectangleWide,     // rectangle-wide
};

/**
 * @brief Callbacks pour la gestion des icônes
 */
struct DpIconCallbacks {
    std::function<wxString()> getDataPath;  // Fonction pour obtenir le chemin des données
};

/**
 * @brief Gestionnaire d'icônes centralisé
 */
class DpIconManager {
public:
    static DpIconManager& Instance();
    
    // Initialisation avec callbacks
    void Init(const DpIconCallbacks& callbacks);
    
    // Charge la fonte Font Awesome (si pas déjà fait)
    bool LoadIconFont();
    
    // Nouvelle méthode pour définir le type de fonte à utiliser
    void SetFontType(DpFontAwesomeType type);
    DpFontAwesomeType GetFontType() const { return m_currentFontType; }
    
    // API publique
    wxString GetIconGlyph(DpIcon icon) const;
    wxString GetIconName(DpIcon icon) const;
    wxFont GetIconFont(int pointSize, wxWindow* parent = nullptr) const;
    
    // Vérifie si la fonte est chargée
    bool IsIconFontLoaded() const { return m_fontLoaded; }
    
    // Nouvelle méthode pour vérifier si Font Awesome Pro est disponible
    bool IsProFontAvailable() const { return m_proFontLoaded; }
    
private:
    DpIconManager() = default;
    ~DpIconManager() = default;
    
    // Non copiable
    DpIconManager(const DpIconManager&) = delete;
    DpIconManager& operator=(const DpIconManager&) = delete;
    
    // Membres privés
    bool m_initialized = false;
    bool m_fontLoaded = false;
    bool m_proFontLoaded = false;
    DpFontAwesomeType m_currentFontType = DpFontAwesomeType::Free;
    DpIconCallbacks m_callbacks;
    
    static const wxString kFaFamilyName;
    static const wxString kFaProFamilyName;
    
    // Helper internes
    wxFont CreateScaledIconFont(int pointSize, wxWindow* parent) const;
    bool LoadProFont();
    bool LoadFreeFont();
    
    // Map des icônes
    static std::map<DpIcon, wxString> GetIconMap();
    static std::map<DpIcon, wxString> GetNameMap();
};