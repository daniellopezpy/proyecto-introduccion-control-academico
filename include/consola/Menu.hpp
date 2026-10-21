// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Declarar el menú interactivo aislado para pacientes y médicos.

#pragma once

#include <iosfwd>

#include "servicios/ServicioMedicos.hpp"
#include "servicios/ServicioPacientes.hpp"

namespace vitralis {

int ejecutarMenuConsola(
    ServicioPacientes& servicioPacientes,
    ServicioMedicos& servicioMedicos,
    std::istream& entrada,
    std::ostream& salida);

}  // namespace vitralis
