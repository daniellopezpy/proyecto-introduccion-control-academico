// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Declarar las operaciones de negocio para pacientes.

#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

#include "modelos/Paciente.hpp"
#include "repositorio/ArchivoPacientes.hpp"
#include "servicios/ResultadoServicio.hpp"

namespace vitralis {

class ServicioPacientes {
public:
    ServicioPacientes(
        ListaPacientes& lista,
        ArchivoPacientes& archivo);

    [[nodiscard]] ResultadoServicio cargar(std::string& detalleError);

    [[nodiscard]] ResultadoServicio registrar(
        const Paciente& paciente,
        std::string& detalleError);

    [[nodiscard]] ResultadoServicio buscarPorCodigo(
        std::string_view codigo,
        Paciente& resultado,
        std::string& detalleError) const;

    [[nodiscard]] std::size_t buscarPorTexto(
        std::string_view consulta,
        std::array<Paciente, MAX_PACIENTES>& resultados) const;

    [[nodiscard]] std::size_t buscarPorEspecialidad(
        Especialidad especialidad,
        std::array<Paciente, MAX_PACIENTES>& resultados) const;

    [[nodiscard]] ResultadoServicio editar(
        std::string_view codigo,
        std::string_view nombre,
        int edad,
        Especialidad especialidadRequerida,
        bool activo,
        std::string& detalleError);

    [[nodiscard]] ResultadoServicio cambiarEstado(
        std::string_view codigo,
        bool activo,
        std::string& detalleError);

    [[nodiscard]] const ListaPacientes& listar() const noexcept;

private:
    ListaPacientes& lista_;
    ArchivoPacientes& archivo_;
};

}  // namespace vitralis
