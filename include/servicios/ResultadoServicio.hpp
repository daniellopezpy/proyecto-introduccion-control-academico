// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Definir resultados comunes para las operaciones de servicios.

#pragma once

#include <string_view>

namespace vitralis {

enum class ResultadoServicio {
    Exito,
    CodigoInvalido,
    CodigoDuplicado,
    NombreInvalido,
    EdadInvalida,
    EspecialidadInvalida,
    LimiteAlcanzado,
    NoEncontrado,
    CargaFallida,
    PersistenciaFallida
};

inline constexpr bool resultadoExitoso(ResultadoServicio resultado) noexcept {
    return resultado == ResultadoServicio::Exito;
}

inline constexpr std::string_view resultadoServicioToString(
    ResultadoServicio resultado) noexcept {
    switch (resultado) {
        case ResultadoServicio::Exito: return "OK";
        case ResultadoServicio::CodigoInvalido: return "CODIGO_INVALIDO";
        case ResultadoServicio::CodigoDuplicado: return "CODIGO_DUPLICADO";
        case ResultadoServicio::NombreInvalido: return "NOMBRE_INVALIDO";
        case ResultadoServicio::EdadInvalida: return "EDAD_INVALIDA";
        case ResultadoServicio::EspecialidadInvalida:
            return "ESPECIALIDAD_INVALIDA";
        case ResultadoServicio::LimiteAlcanzado: return "LIMITE_ALCANZADO";
        case ResultadoServicio::NoEncontrado: return "NO_ENCONTRADO";
        case ResultadoServicio::CargaFallida: return "CARGA_FALLIDA";
        case ResultadoServicio::PersistenciaFallida:
            return "PERSISTENCIA_FALLIDA";
    }

    return "RESULTADO_INVALIDO";
}

}  // namespace vitralis
