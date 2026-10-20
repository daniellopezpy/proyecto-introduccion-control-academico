// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-19
// Propósito: Declarar la enumeración Especialidad y sus conversiones canónicas a/desde texto.

#pragma once

#include <optional>
#include <string_view>

namespace vitralis {

enum class Especialidad {
    MedicinaGeneral,
    Pediatria,
    Cardiologia,
    Ginecologia,
    Traumatologia,
    Dermatologia
};

inline constexpr std::string_view
especialidadToString(Especialidad valor) noexcept {
    switch (valor) {
        case Especialidad::MedicinaGeneral: return "MEDICINAGENERAL";
        case Especialidad::Pediatria:       return "PEDIATRIA";
        case Especialidad::Cardiologia:     return "CARDIOLOGIA";
        case Especialidad::Ginecologia:     return "GINECOLOGIA";
        case Especialidad::Traumatologia:   return "TRAUMATOLOGIA";
        case Especialidad::Dermatologia:    return "DERMATOLOGIA";
    }
    return {};
}

inline constexpr std::optional<Especialidad>
especialidadFromString(std::string_view valor) noexcept {
    constexpr Especialidad TODAS[] = {
        Especialidad::MedicinaGeneral,
        Especialidad::Pediatria,
        Especialidad::Cardiologia,
        Especialidad::Ginecologia,
        Especialidad::Traumatologia,
        Especialidad::Dermatologia
    };
    for (const Especialidad candidata : TODAS) {
        if (especialidadToString(candidata) == valor) {
            return candidata;
        }
    }
    return std::nullopt;
}

}  // namespace vitralis
