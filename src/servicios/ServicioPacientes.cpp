// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Implementar validaciones y operaciones persistentes para pacientes.

#include "servicios/ServicioPacientes.hpp"

#include <cctype>
#include <utility>

namespace {

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

bool nombreValido(std::string_view nombre) noexcept {
    if (nombre.empty()
        || nombre.find_first_of("|\r\n") != std::string_view::npos) {
        return false;
    }

    for (const unsigned char caracter : nombre) {
        if (!std::isspace(caracter)) {
            return true;
        }
    }

    return false;
}

bool especialidadValida(vitralis::Especialidad especialidad) noexcept {
    return !vitralis::especialidadToString(especialidad).empty();
}

bool encontrarIndice(
    const vitralis::ListaPacientes& lista,
    std::string_view codigo,
    std::size_t& indiceEncontrado) noexcept {
    for (std::size_t indice = 0; indice < lista.total; ++indice) {
        if (lista.datos[indice].codigo == codigo) {
            indiceEncontrado = indice;
            return true;
        }
    }

    return false;
}

vitralis::ResultadoServicio validarDatos(
    std::string_view nombre,
    int edad,
    vitralis::Especialidad especialidad,
    std::string& detalleError) {
    if (!nombreValido(nombre)) {
        detalleError = "El nombre no puede estar vacio ni contener separadores.";
        return vitralis::ResultadoServicio::NombreInvalido;
    }

    if (edad < 0 || edad > 120) {
        detalleError = "La edad debe estar entre 0 y 120.";
        return vitralis::ResultadoServicio::EdadInvalida;
    }

    if (!especialidadValida(especialidad)) {
        detalleError = "La especialidad no es valida.";
        return vitralis::ResultadoServicio::EspecialidadInvalida;
    }

    detalleError.clear();
    return vitralis::ResultadoServicio::Exito;
}

vitralis::ResultadoServicio persistir(
    vitralis::ListaPacientes& lista,
    vitralis::ArchivoPacientes& archivo,
    const vitralis::ListaPacientes& respaldo,
    std::string& detalleError) {
    std::string errorRepositorio;
    if (archivo.guardar(lista, errorRepositorio)) {
        detalleError.clear();
        return vitralis::ResultadoServicio::Exito;
    }

    lista = respaldo;
    detalleError = std::move(errorRepositorio);
    return vitralis::ResultadoServicio::PersistenciaFallida;
}

}  // namespace

namespace vitralis {

ServicioPacientes::ServicioPacientes(
    ListaPacientes& lista,
    ArchivoPacientes& archivo)
    : lista_(lista), archivo_(archivo) {}

ResultadoServicio ServicioPacientes::cargar(std::string& detalleError) {
    if (archivo_.cargar(lista_, detalleError)) {
        return ResultadoServicio::Exito;
    }

    return ResultadoServicio::CargaFallida;
}

ResultadoServicio ServicioPacientes::registrar(
    const Paciente& paciente,
    std::string& detalleError) {
    if (!codigoValido(paciente.codigo)) {
        detalleError = "El codigo debe tener el formato P-0001.";
        return ResultadoServicio::CodigoInvalido;
    }

    const ResultadoServicio validacion = validarDatos(
        paciente.nombre,
        paciente.edad,
        paciente.especialidadRequerida,
        detalleError);
    if (validacion != ResultadoServicio::Exito) {
        return validacion;
    }

    std::size_t indiceExistente = 0;
    if (encontrarIndice(lista_, paciente.codigo, indiceExistente)) {
        detalleError = "Ya existe un paciente con ese codigo.";
        return ResultadoServicio::CodigoDuplicado;
    }

    if (lista_.total >= MAX_PACIENTES) {
        detalleError = "Se alcanzo MAX_PACIENTES.";
        return ResultadoServicio::LimiteAlcanzado;
    }

    const ListaPacientes respaldo = lista_;
    lista_.datos[lista_.total] = paciente;
    ++lista_.total;
    return persistir(lista_, archivo_, respaldo, detalleError);
}

ResultadoServicio ServicioPacientes::buscarPorCodigo(
    std::string_view codigo,
    Paciente& resultado,
    std::string& detalleError) const {
    if (!codigoValido(codigo)) {
        detalleError = "El codigo debe tener el formato P-0001.";
        return ResultadoServicio::CodigoInvalido;
    }

    std::size_t indice = 0;
    if (!encontrarIndice(lista_, codigo, indice)) {
        detalleError = "No se encontro el paciente solicitado.";
        return ResultadoServicio::NoEncontrado;
    }

    resultado = lista_.datos[indice];
    detalleError.clear();
    return ResultadoServicio::Exito;
}

std::size_t ServicioPacientes::buscarPorTexto(
    std::string_view consulta,
    std::array<Paciente, MAX_PACIENTES>& resultados) const {
    if (consulta.empty()) {
        return 0;
    }

    std::size_t totalResultados = 0;
    for (std::size_t indice = 0;
         indice < lista_.total && totalResultados < resultados.size();
         ++indice) {
        const Paciente& paciente = lista_.datos[indice];
        if (paciente.codigo.find(consulta) != std::string::npos
            || paciente.nombre.find(consulta) != std::string::npos) {
            resultados[totalResultados] = paciente;
            ++totalResultados;
        }
    }

    return totalResultados;
}

std::size_t ServicioPacientes::buscarPorEspecialidad(
    Especialidad especialidad,
    std::array<Paciente, MAX_PACIENTES>& resultados) const {
    if (!especialidadValida(especialidad)) {
        return 0;
    }

    std::size_t totalResultados = 0;
    for (std::size_t indice = 0;
         indice < lista_.total && totalResultados < resultados.size();
         ++indice) {
        const Paciente& paciente = lista_.datos[indice];
        if (paciente.especialidadRequerida == especialidad) {
            resultados[totalResultados] = paciente;
            ++totalResultados;
        }
    }

    return totalResultados;
}

ResultadoServicio ServicioPacientes::editar(
    std::string_view codigo,
    std::string_view nombre,
    int edad,
    Especialidad especialidadRequerida,
    bool activo,
    std::string& detalleError) {
    if (!codigoValido(codigo)) {
        detalleError = "El codigo debe tener el formato P-0001.";
        return ResultadoServicio::CodigoInvalido;
    }

    const ResultadoServicio validacion = validarDatos(
        nombre,
        edad,
        especialidadRequerida,
        detalleError);
    if (validacion != ResultadoServicio::Exito) {
        return validacion;
    }

    std::size_t indice = 0;
    if (!encontrarIndice(lista_, codigo, indice)) {
        detalleError = "No se encontro el paciente solicitado.";
        return ResultadoServicio::NoEncontrado;
    }

    const ListaPacientes respaldo = lista_;
    Paciente& paciente = lista_.datos[indice];
    paciente.nombre = nombre;
    paciente.edad = edad;
    paciente.especialidadRequerida = especialidadRequerida;
    paciente.activo = activo;
    return persistir(lista_, archivo_, respaldo, detalleError);
}

ResultadoServicio ServicioPacientes::cambiarEstado(
    std::string_view codigo,
    bool activo,
    std::string& detalleError) {
    if (!codigoValido(codigo)) {
        detalleError = "El codigo debe tener el formato P-0001.";
        return ResultadoServicio::CodigoInvalido;
    }

    std::size_t indice = 0;
    if (!encontrarIndice(lista_, codigo, indice)) {
        detalleError = "No se encontro el paciente solicitado.";
        return ResultadoServicio::NoEncontrado;
    }

    const ListaPacientes respaldo = lista_;
    lista_.datos[indice].activo = activo;
    return persistir(lista_, archivo_, respaldo, detalleError);
}

const ListaPacientes& ServicioPacientes::listar() const noexcept {
    return lista_;
}

}  // namespace vitralis
