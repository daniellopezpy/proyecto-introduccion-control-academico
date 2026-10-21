// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Declarar el repositorio de médicos respaldado por un archivo delimitado.

#pragma once

#include <filesystem>
#include <string>

#include "modelos/Medico.hpp"

namespace vitralis {

class ArchivoMedicos {
public:
    explicit ArchivoMedicos(
        std::filesystem::path ruta = "data/medicos.dat");

    [[nodiscard]] bool cargar(
        ListaMedicos& destino,
        std::string& detalleError) const;

    [[nodiscard]] bool guardar(
        const ListaMedicos& origen,
        std::string& detalleError) const;

private:
    std::filesystem::path ruta_;
};

}  // namespace vitralis
