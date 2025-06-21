#pragma once

#include <wx/colour.h>

// ============================================================================
// Types partagés entre DpThemeManager et DpThemesBridge
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