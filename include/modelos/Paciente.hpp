// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-19
// Propósito: Declarar el modelo Paciente y su contenedor de capacidad fija ListaPacientes.

#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "Especialidad.hpp"

namespace vitralis {

inline constexpr std::size_t MAX_PACIENTES = 500;

struct Paciente {
    std::string codigo;
    std::string nombre;
    int edad{};
    Especialidad especialidadRequerida{};
    bool activo{true};
};

struct ListaPacientes {
    std::array<Paciente, MAX_PACIENTES> datos{};
    std::size_t total{0};
};

}  // namespace vitralis
