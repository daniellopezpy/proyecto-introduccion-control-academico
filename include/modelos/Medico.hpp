// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-19
// Propósito: Declarar el modelo Medico y su contenedor de capacidad fija ListaMedicos.

#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "Especialidad.hpp"

namespace vitralis {

inline constexpr std::size_t MAX_MEDICOS = 60;

struct Medico {
    std::string codigo;
    std::string nombre;
    Especialidad especialidad{};
    bool activo{true};
};

struct ListaMedicos {
    std::array<Medico, MAX_MEDICOS> datos{};
    std::size_t total{0};
};

}  // namespace vitralis
