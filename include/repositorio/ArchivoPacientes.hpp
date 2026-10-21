// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Declarar el repositorio de pacientes respaldado por un archivo delimitado.

#pragma once

#include <filesystem>
#include <string>

#include "modelos/Paciente.hpp"

namespace vitralis {

class ArchivoPacientes {
public:
    explicit ArchivoPacientes(
        std::filesystem::path ruta = "data/pacientes.dat");

    [[nodiscard]] bool cargar(
        ListaPacientes& destino,
        std::string& detalleError) const;

    [[nodiscard]] bool guardar(
        const ListaPacientes& origen,
        std::string& detalleError) const;

private:
    std::filesystem::path ruta_;
};

}  // namespace vitralis
