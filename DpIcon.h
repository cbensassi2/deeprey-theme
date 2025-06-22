#pragma once

#include <wx/string.h>
#include <wx/font.h>
#include <wx/filename.h>
#include <map>
#include <functional>

// Forward declaration
class wxWindow;

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
    
    // API publique
    wxString GetIconGlyph(DpIcon icon) const;
    wxString GetIconName(DpIcon icon) const;
    wxFont GetIconFont(int pointSize, wxWindow* parent = nullptr) const;
    
    // Vérifie si la fonte est chargée
    bool IsIconFontLoaded() const { return m_fontLoaded; }
    
private:
    DpIconManager() = default;
    ~DpIconManager() = default;
    
    // Non copiable
    DpIconManager(const DpIconManager&) = delete;
    DpIconManager& operator=(const DpIconManager&) = delete;
    
    // Membres privés
    bool m_initialized = false;
    bool m_fontLoaded = false;
    DpIconCallbacks m_callbacks;
    
    static const wxString kFaFamilyName;
    
    // Helper interne
    wxFont CreateScaledIconFont(int pointSize, wxWindow* parent) const;
    
    // Map des icônes
    static std::map<DpIcon, wxString> GetIconMap();
    static std::map<DpIcon, wxString> GetNameMap();
};