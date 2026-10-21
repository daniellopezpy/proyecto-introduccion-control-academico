// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Implementar la lectura y escritura validada de pacientes en formato delimitado.

#include "repositorio/ArchivoPacientes.hpp"

#include <array>
#include <charconv>
#include <fstream>
#include <string_view>
#include <system_error>
#include <utility>

namespace {

bool separarCampos(
    const std::string& linea,
    std::array<std::string, 5>& campos) {
    std::size_t inicio = 0;

    for (std::size_t indice = 0; indice + 1 < campos.size(); ++indice) {
        const std::size_t separador = linea.find('|', inicio);
        if (separador == std::string::npos) {
            return false;
        }

        campos[indice] = linea.substr(inicio, separador - inicio);
        inicio = separador + 1;
    }

    campos.back() = linea.substr(inicio);
    return true;
}

bool campoValido(std::string_view campo) noexcept {
    return !campo.empty()
        && campo.find_first_of("|\r\n") == std::string_view::npos;
}

bool codigoValido(std::string_view codigo) noexcept {
    constexpr std::size_t LONGITUD_CODIGO = 6;
    if (codigo.size() != LONGITUD_CODIGO
        || codigo[0] != 'P'
        || codigo[1] != '-') {
        return false;
    }

    for (std::size_t indice = 2; indice < codigo.size(); ++indice) {
        if (codigo[indice] < '0' || codigo[indice] > '9') {
            return false;
        }
    }

    return true;
}

bool edadValida(std::string_view texto, int& edad) noexcept {
    if (texto.empty()) {
        return false;
    }

    const auto resultado = std::from_chars(
        texto.data(),
        texto.data() + texto.size(),
        edad);

    return resultado.ec == std::errc{}
        && resultado.ptr == texto.data() + texto.size()
        && edad >= 0
        && edad <= 120;
}

bool activoValido(std::string_view texto, bool& activo) noexcept {
    if (texto == "1") {
        activo = true;
        return true;
    }

    if (texto == "0") {
        activo = false;
        return true;
    }

    return false;
}

std::string errorDeLinea(
    std::size_t numeroLinea,
    std::string_view detalle) {
    return "pacientes.dat, linea "
        + std::to_string(numeroLinea)
        + ": "
        + std::string(detalle);
}

bool codigoRepetido(
    const vitralis::ListaPacientes& lista,
    std::string_view codigo) noexcept {
    for (std::size_t indice = 0; indice < lista.total; ++indice) {
        if (lista.datos[indice].codigo == codigo) {
            return true;
        }
    }

    return false;
}

}  // namespace

namespace vitralis {

ArchivoPacientes::ArchivoPacientes(std::filesystem::path ruta)
    : ruta_(std::move(ruta)) {}

bool ArchivoPacientes::cargar(
    ListaPacientes& destino,
    std::string& detalleError) const {
    detalleError.clear();

    std::error_code errorSistema;
    const bool existe = std::filesystem::exists(ruta_, errorSistema);
    if (errorSistema) {
        detalleError = "No se pudo verificar el archivo de pacientes: "
            + errorSistema.message();
        return false;
    }

    if (!existe) {
        destino = {};
        return true;
    }

    std::ifstream archivo(ruta_);
    if (!archivo) {
        detalleError = "No se pudo abrir el archivo de pacientes para lectura.";
        return false;
    }

    ListaPacientes temporal{};
    std::string linea;
    std::size_t numeroLinea = 0;

    while (std::getline(archivo, linea)) {
        ++numeroLinea;
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }

        std::array<std::string, 5> campos{};
        if (!separarCampos(linea, campos)) {
            detalleError = errorDeLinea(numeroLinea, "cantidad de campos invalida");
            return false;
        }

        if (!codigoValido(campos[0])) {
            detalleError = errorDeLinea(numeroLinea, "codigo invalido");
            return false;
        }

        if (!campoValido(campos[1])) {
            detalleError = errorDeLinea(numeroLinea, "nombre invalido");
            return false;
        }

        int edad = 0;
        if (!edadValida(campos[2], edad)) {
            detalleError = errorDeLinea(numeroLinea, "edad invalida");
            return false;
        }

        const auto especialidad = especialidadFromString(campos[3]);
        if (!especialidad.has_value()) {
            detalleError = errorDeLinea(numeroLinea, "especialidad invalida");
            return false;
        }

        bool activo = false;
        if (!activoValido(campos[4], activo)) {
            detalleError = errorDeLinea(numeroLinea, "estado activo invalido");
            return false;
        }

        if (codigoRepetido(temporal, campos[0])) {
            detalleError = errorDeLinea(numeroLinea, "codigo duplicado");
            return false;
        }

        if (temporal.total >= MAX_PACIENTES) {
            detalleError = errorDeLinea(numeroLinea, "se excede MAX_PACIENTES");
            return false;
        }

        temporal.datos[temporal.total] = Paciente{
            campos[0],
            campos[1],
            edad,
            *especialidad,
            activo};
        ++temporal.total;
    }

    if (!archivo.eof() && archivo.fail()) {
        detalleError = "Se produjo un error al leer el archivo de pacientes.";
        return false;
    }

    destino = std::move(temporal);
    return true;
}

bool ArchivoPacientes::guardar(
    const ListaPacientes& origen,
    std::string& detalleError) const {
    detalleError.clear();

    if (origen.total > MAX_PACIENTES) {
        detalleError = "La lista excede MAX_PACIENTES.";
        return false;
    }

    for (std::size_t indice = 0; indice < origen.total; ++indice) {
        const Paciente& paciente = origen.datos[indice];
        if (!codigoValido(paciente.codigo)) {
            detalleError = "El paciente contiene un codigo invalido.";
            return false;
        }

        if (!campoValido(paciente.nombre)) {
            detalleError = "El paciente contiene un nombre invalido.";
            return false;
        }

        if (paciente.edad < 0 || paciente.edad > 120) {
            detalleError = "El paciente contiene una edad invalida.";
            return false;
        }

        if (especialidadToString(paciente.especialidadRequerida).empty()) {
            detalleError = "El paciente contiene una especialidad invalida.";
            return false;
        }

        if (codigoRepetido(origen, paciente.codigo)) {
            for (std::size_t anterior = 0; anterior < indice; ++anterior) {
                if (origen.datos[anterior].codigo == paciente.codigo) {
                    detalleError = "La lista contiene codigos duplicados.";
                    return false;
                }
            }
        }
    }

    const std::filesystem::path directorio = ruta_.parent_path();
    if (!directorio.empty()) {
        std::error_code errorSistema;
        std::filesystem::create_directories(directorio, errorSistema);
        if (errorSistema) {
            detalleError = "No se pudo crear el directorio de pacientes: "
                + errorSistema.message();
            return false;
        }
    }

    std::ofstream archivo(ruta_, std::ios::trunc);
    if (!archivo) {
        detalleError = "No se pudo abrir el archivo de pacientes para escritura.";
        return false;
    }

    for (std::size_t indice = 0; indice < origen.total; ++indice) {
        const Paciente& paciente = origen.datos[indice];
        archivo << paciente.codigo << '|'
                << paciente.nombre << '|'
                << paciente.edad << '|'
                << especialidadToString(paciente.especialidadRequerida) << '|'
                << (paciente.activo ? '1' : '0') << '\n';
    }

    if (!archivo) {
        detalleError = "Se produjo un error al escribir el archivo de pacientes.";
        return false;
    }

    return true;
}

}  // namespace vitralis
