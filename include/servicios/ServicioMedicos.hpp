// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Declarar las operaciones de negocio para médicos.

#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

#include "modelos/Medico.hpp"
#include "repositorio/ArchivoMedicos.hpp"
#include "servicios/ResultadoServicio.hpp"

namespace vitralis {

class ServicioMedicos {
public:
    ServicioMedicos(
        ListaMedicos& lista,
        ArchivoMedicos& archivo);

    [[nodiscard]] ResultadoServicio cargar(std::string& detalleError);

    [[nodiscard]] ResultadoServicio registrar(
        const Medico& medico,
        std::string& detalleError);

    [[nodiscard]] ResultadoServicio buscarPorCodigo(
        std::string_view codigo,
        Medico& resultado,
        std::string& detalleError) const;

    [[nodiscard]] std::size_t buscarPorTexto(
        std::string_view consulta,
        std::array<Medico, MAX_MEDICOS>& resultados) const;

    [[nodiscard]] std::size_t buscarPorEspecialidad(
        Especialidad especialidad,
        std::array<Medico, MAX_MEDICOS>& resultados) const;

    [[nodiscard]] ResultadoServicio editar(
        std::string_view codigo,
        std::string_view nombre,
        Especialidad especialidad,
        bool activo,
        std::string& detalleError);

    [[nodiscard]] ResultadoServicio cambiarEstado(
        std::string_view codigo,
        bool activo,
        std::string& detalleError);

    [[nodiscard]] const ListaMedicos& listar() const noexcept;

private:
    ListaMedicos& lista_;
    ArchivoMedicos& archivo_;
};

}  // namespace vitralis
