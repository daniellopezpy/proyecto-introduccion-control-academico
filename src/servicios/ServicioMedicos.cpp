// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Implementar validaciones y operaciones persistentes para médicos.

#include "servicios/ServicioMedicos.hpp"

#include <cctype>
#include <utility>

namespace {

bool codigoValido(std::string_view codigo) noexcept {
    constexpr std::size_t LONGITUD_CODIGO = 6;
    if (codigo.size() != LONGITUD_CODIGO
        || codigo[0] != 'M'
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
    const vitralis::ListaMedicos& lista,
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
    vitralis::Especialidad especialidad,
    std::string& detalleError) {
    if (!nombreValido(nombre)) {
        detalleError = "El nombre no puede estar vacio ni contener separadores.";
        return vitralis::ResultadoServicio::NombreInvalido;
    }

    if (!especialidadValida(especialidad)) {
        detalleError = "La especialidad no es valida.";
        return vitralis::ResultadoServicio::EspecialidadInvalida;
    }

    detalleError.clear();
    return vitralis::ResultadoServicio::Exito;
}

vitralis::ResultadoServicio persistir(
    vitralis::ListaMedicos& lista,
    vitralis::ArchivoMedicos& archivo,
    const vitralis::ListaMedicos& respaldo,
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

ServicioMedicos::ServicioMedicos(
    ListaMedicos& lista,
    ArchivoMedicos& archivo)
    : lista_(lista), archivo_(archivo) {}

ResultadoServicio ServicioMedicos::cargar(std::string& detalleError) {
    if (archivo_.cargar(lista_, detalleError)) {
        return ResultadoServicio::Exito;
    }

    return ResultadoServicio::CargaFallida;
}

ResultadoServicio ServicioMedicos::registrar(
    const Medico& medico,
    std::string& detalleError) {
    if (!codigoValido(medico.codigo)) {
        detalleError = "El codigo debe tener el formato M-0001.";
        return ResultadoServicio::CodigoInvalido;
    }

    const ResultadoServicio validacion = validarDatos(
        medico.nombre,
        medico.especialidad,
        detalleError);
    if (validacion != ResultadoServicio::Exito) {
        return validacion;
    }

    std::size_t indiceExistente = 0;
    if (encontrarIndice(lista_, medico.codigo, indiceExistente)) {
        detalleError = "Ya existe un medico con ese codigo.";
        return ResultadoServicio::CodigoDuplicado;
    }

    if (lista_.total >= MAX_MEDICOS) {
        detalleError = "Se alcanzo MAX_MEDICOS.";
        return ResultadoServicio::LimiteAlcanzado;
    }

    const ListaMedicos respaldo = lista_;
    lista_.datos[lista_.total] = medico;
    ++lista_.total;
    return persistir(lista_, archivo_, respaldo, detalleError);
}

ResultadoServicio ServicioMedicos::buscarPorCodigo(
    std::string_view codigo,
    Medico& resultado,
    std::string& detalleError) const {
    if (!codigoValido(codigo)) {
        detalleError = "El codigo debe tener el formato M-0001.";
        return ResultadoServicio::CodigoInvalido;
    }

    std::size_t indice = 0;
    if (!encontrarIndice(lista_, codigo, indice)) {
        detalleError = "No se encontro el medico solicitado.";
        return ResultadoServicio::NoEncontrado;
    }

    resultado = lista_.datos[indice];
    detalleError.clear();
    return ResultadoServicio::Exito;
}

std::size_t ServicioMedicos::buscarPorTexto(
    std::string_view consulta,
    std::array<Medico, MAX_MEDICOS>& resultados) const {
    if (consulta.empty()) {
        return 0;
    }

    std::size_t totalResultados = 0;
    for (std::size_t indice = 0;
         indice < lista_.total && totalResultados < resultados.size();
         ++indice) {
        const Medico& medico = lista_.datos[indice];
        if (medico.codigo.find(consulta) != std::string::npos
            || medico.nombre.find(consulta) != std::string::npos) {
            resultados[totalResultados] = medico;
            ++totalResultados;
        }
    }

    return totalResultados;
}

std::size_t ServicioMedicos::buscarPorEspecialidad(
    Especialidad especialidad,
    std::array<Medico, MAX_MEDICOS>& resultados) const {
    if (!especialidadValida(especialidad)) {
        return 0;
    }

    std::size_t totalResultados = 0;
    for (std::size_t indice = 0;
         indice < lista_.total && totalResultados < resultados.size();
         ++indice) {
        const Medico& medico = lista_.datos[indice];
        if (medico.especialidad == especialidad) {
            resultados[totalResultados] = medico;
            ++totalResultados;
        }
    }

    return totalResultados;
}

ResultadoServicio ServicioMedicos::editar(
    std::string_view codigo,
    std::string_view nombre,
    Especialidad especialidad,
    bool activo,
    std::string& detalleError) {
    if (!codigoValido(codigo)) {
        detalleError = "El codigo debe tener el formato M-0001.";
        return ResultadoServicio::CodigoInvalido;
    }

    const ResultadoServicio validacion = validarDatos(
        nombre,
        especialidad,
        detalleError);
    if (validacion != ResultadoServicio::Exito) {
        return validacion;
    }

    std::size_t indice = 0;
    if (!encontrarIndice(lista_, codigo, indice)) {
        detalleError = "No se encontro el medico solicitado.";
        return ResultadoServicio::NoEncontrado;
    }

    const ListaMedicos respaldo = lista_;
    Medico& medico = lista_.datos[indice];
    medico.nombre = nombre;
    medico.especialidad = especialidad;
    medico.activo = activo;
    return persistir(lista_, archivo_, respaldo, detalleError);
}

ResultadoServicio ServicioMedicos::cambiarEstado(
    std::string_view codigo,
    bool activo,
    std::string& detalleError) {
    if (!codigoValido(codigo)) {
        detalleError = "El codigo debe tener el formato M-0001.";
        return ResultadoServicio::CodigoInvalido;
    }

    std::size_t indice = 0;
    if (!encontrarIndice(lista_, codigo, indice)) {
        detalleError = "No se encontro el medico solicitado.";
        return ResultadoServicio::NoEncontrado;
    }

    const ListaMedicos respaldo = lista_;
    lista_.datos[indice].activo = activo;
    return persistir(lista_, archivo_, respaldo, detalleError);
}

const ListaMedicos& ServicioMedicos::listar() const noexcept {
    return lista_;
}

}  // namespace vitralis
