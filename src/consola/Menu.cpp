// Autor: Daniel · Vitralis Health Labs
// Fecha: 2026-10-20
// Propósito: Implementar el menú interactivo aislado para pacientes y médicos.

#include "consola/Menu.hpp"

#include <array>
#include <charconv>
#include <cstddef>
#include <istream>
#include <ostream>
#include <string>
#include <string_view>

namespace {

bool leerLinea(
    std::istream& entrada,
    std::ostream& salida,
    std::string_view mensaje,
    std::string& valor) {
    salida << mensaje;
    return static_cast<bool>(std::getline(entrada, valor));
}

bool leerEntero(
    std::istream& entrada,
    std::ostream& salida,
    std::string_view mensaje,
    int& valor) {
    std::string texto;
    while (leerLinea(entrada, salida, mensaje, texto)) {
        if (!texto.empty()) {
            const auto resultado = std::from_chars(
                texto.data(),
                texto.data() + texto.size(),
                valor);
            if (resultado.ec == std::errc{}
                && resultado.ptr == texto.data() + texto.size()) {
                return true;
            }
        }

        salida << "Entrada numerica invalida.\n";
    }

    return false;
}

bool leerEstado(
    std::istream& entrada,
    std::ostream& salida,
    bool& activo) {
    std::string texto;
    while (leerLinea(
        entrada,
        salida,
        "Estado activo (1=si, 0=no): ",
        texto)) {
        if (texto == "1" || texto == "s" || texto == "S") {
            activo = true;
            return true;
        }
        if (texto == "0" || texto == "n" || texto == "N") {
            activo = false;
            return true;
        }

        salida << "Indica 1, 0, s o n.\n";
    }

    return false;
}

bool leerEspecialidad(
    std::istream& entrada,
    std::ostream& salida,
    vitralis::Especialidad& especialidad) {
    constexpr std::array<vitralis::Especialidad, 6> OPCIONES{
        vitralis::Especialidad::MedicinaGeneral,
        vitralis::Especialidad::Pediatria,
        vitralis::Especialidad::Cardiologia,
        vitralis::Especialidad::Ginecologia,
        vitralis::Especialidad::Traumatologia,
        vitralis::Especialidad::Dermatologia};

    salida << "Especialidades:\n";
    for (std::size_t indice = 0; indice < OPCIONES.size(); ++indice) {
        salida << "  " << (indice + 1) << ". "
               << vitralis::especialidadToString(OPCIONES[indice]) << '\n';
    }

    int opcion = 0;
    while (leerEntero(entrada, salida, "Seleccion: ", opcion)) {
        if (opcion >= 1
            && static_cast<std::size_t>(opcion) <= OPCIONES.size()) {
            especialidad = OPCIONES[static_cast<std::size_t>(opcion - 1)];
            return true;
        }

        salida << "Seleccion de especialidad invalida.\n";
    }

    return false;
}

void mostrarResultado(
    std::ostream& salida,
    vitralis::ResultadoServicio resultado,
    std::string_view detalle) {
    if (resultado == vitralis::ResultadoServicio::Exito) {
        salida << "Operacion completada.\n";
        return;
    }

    salida << "Error ["
           << vitralis::resultadoServicioToString(resultado)
           << "]: "
           << detalle
           << '\n';
}

void mostrarPaciente(
    std::ostream& salida,
    const vitralis::Paciente& paciente) {
    salida << paciente.codigo << " | "
           << paciente.nombre << " | edad "
           << paciente.edad << " | "
           << vitralis::especialidadToString(paciente.especialidadRequerida)
           << " | activo " << (paciente.activo ? "si" : "no") << '\n';
}

void mostrarMedico(
    std::ostream& salida,
    const vitralis::Medico& medico) {
    salida << medico.codigo << " | "
           << medico.nombre << " | "
           << vitralis::especialidadToString(medico.especialidad)
           << " | activo " << (medico.activo ? "si" : "no") << '\n';
}

bool leerPaciente(
    std::istream& entrada,
    std::ostream& salida,
    vitralis::Paciente& paciente) {
    if (!leerLinea(entrada, salida, "Codigo P-0001: ", paciente.codigo)
        || !leerLinea(entrada, salida, "Nombre: ", paciente.nombre)
        || !leerEntero(entrada, salida, "Edad: ", paciente.edad)
        || !leerEspecialidad(entrada, salida, paciente.especialidadRequerida)
        || !leerEstado(entrada, salida, paciente.activo)) {
        return false;
    }

    return true;
}

bool leerMedico(
    std::istream& entrada,
    std::ostream& salida,
    vitralis::Medico& medico) {
    if (!leerLinea(entrada, salida, "Codigo M-0001: ", medico.codigo)
        || !leerLinea(entrada, salida, "Nombre: ", medico.nombre)
        || !leerEspecialidad(entrada, salida, medico.especialidad)
        || !leerEstado(entrada, salida, medico.activo)) {
        return false;
    }

    return true;
}

void listarPacientes(
    vitralis::ServicioPacientes& servicio,
    std::ostream& salida) {
    const vitralis::ListaPacientes& lista = servicio.listar();
    if (lista.total == 0) {
        salida << "No hay pacientes registrados.\n";
        return;
    }

    for (std::size_t indice = 0; indice < lista.total; ++indice) {
        mostrarPaciente(salida, lista.datos[indice]);
    }
}

void listarMedicos(
    vitralis::ServicioMedicos& servicio,
    std::ostream& salida) {
    const vitralis::ListaMedicos& lista = servicio.listar();
    if (lista.total == 0) {
        salida << "No hay medicos registrados.\n";
        return;
    }

    for (std::size_t indice = 0; indice < lista.total; ++indice) {
        mostrarMedico(salida, lista.datos[indice]);
    }
}

void buscarPacientePorCodigo(
    vitralis::ServicioPacientes& servicio,
    std::istream& entrada,
    std::ostream& salida) {
    std::string codigo;
    if (!leerLinea(entrada, salida, "Codigo: ", codigo)) {
        return;
    }

    vitralis::Paciente paciente{};
    std::string error;
    const auto resultado = servicio.buscarPorCodigo(codigo, paciente, error);
    if (resultado == vitralis::ResultadoServicio::Exito) {
        mostrarPaciente(salida, paciente);
    } else {
        mostrarResultado(salida, resultado, error);
    }
}

void buscarPacientesPorTexto(
    vitralis::ServicioPacientes& servicio,
    std::istream& entrada,
    std::ostream& salida) {
    std::string consulta;
    if (!leerLinea(entrada, salida, "Texto de busqueda: ", consulta)) {
        return;
    }

    std::array<vitralis::Paciente, vitralis::MAX_PACIENTES> resultados{};
    const std::size_t total = servicio.buscarPorTexto(consulta, resultados);
    if (total == 0) {
        salida << "No se encontraron pacientes.\n";
        return;
    }

    for (std::size_t indice = 0; indice < total; ++indice) {
        mostrarPaciente(salida, resultados[indice]);
    }
}

void editarPaciente(
    vitralis::ServicioPacientes& servicio,
    std::istream& entrada,
    std::ostream& salida) {
    std::string codigo;
    if (!leerLinea(entrada, salida, "Codigo del paciente: ", codigo)) {
        return;
    }

    vitralis::Paciente actual{};
    std::string error;
    if (servicio.buscarPorCodigo(codigo, actual, error)
        != vitralis::ResultadoServicio::Exito) {
        salida << error << '\n';
        return;
    }

    if (!leerLinea(entrada, salida, "Nuevo nombre: ", actual.nombre)
        || !leerEntero(entrada, salida, "Nueva edad: ", actual.edad)
        || !leerEspecialidad(
            entrada, salida, actual.especialidadRequerida)
        || !leerEstado(entrada, salida, actual.activo)) {
        return;
    }

    const auto resultado = servicio.editar(
        codigo,
        actual.nombre,
        actual.edad,
        actual.especialidadRequerida,
        actual.activo,
        error);
    mostrarResultado(salida, resultado, error);
}

void menuPacientes(
    vitralis::ServicioPacientes& servicio,
    std::istream& entrada,
    std::ostream& salida) {
    while (true) {
        salida << "\nPacientes\n"
               << "1. Registrar\n"
               << "2. Buscar por codigo\n"
               << "3. Buscar por texto\n"
               << "4. Editar\n"
               << "5. Listar\n"
               << "6. Cambiar estado\n"
               << "0. Volver\n";

        int opcion = 0;
        if (!leerEntero(entrada, salida, "Opcion: ", opcion) || opcion == 0) {
            return;
        }

        if (opcion == 1) {
            vitralis::Paciente paciente{};
            if (leerPaciente(entrada, salida, paciente)) {
                std::string error;
                const auto resultado = servicio.registrar(paciente, error);
                mostrarResultado(salida, resultado, error);
            }
        } else if (opcion == 2) {
            buscarPacientePorCodigo(servicio, entrada, salida);
        } else if (opcion == 3) {
            buscarPacientesPorTexto(servicio, entrada, salida);
        } else if (opcion == 4) {
            editarPaciente(servicio, entrada, salida);
        } else if (opcion == 5) {
            listarPacientes(servicio, salida);
        } else if (opcion == 6) {
            std::string codigo;
            bool activo = false;
            if (leerLinea(entrada, salida, "Codigo: ", codigo)
                && leerEstado(entrada, salida, activo)) {
                std::string error;
                const auto resultado = servicio.cambiarEstado(codigo, activo, error);
                mostrarResultado(salida, resultado, error);
            }
        } else {
            salida << "Opcion invalida.\n";
        }
    }
}

void buscarMedicoPorCodigo(
    vitralis::ServicioMedicos& servicio,
    std::istream& entrada,
    std::ostream& salida) {
    std::string codigo;
    if (!leerLinea(entrada, salida, "Codigo: ", codigo)) {
        return;
    }

    vitralis::Medico medico{};
    std::string error;
    const auto resultado = servicio.buscarPorCodigo(codigo, medico, error);
    if (resultado == vitralis::ResultadoServicio::Exito) {
        mostrarMedico(salida, medico);
    } else {
        mostrarResultado(salida, resultado, error);
    }
}

void buscarMedicosPorTexto(
    vitralis::ServicioMedicos& servicio,
    std::istream& entrada,
    std::ostream& salida) {
    std::string consulta;
    if (!leerLinea(entrada, salida, "Texto de busqueda: ", consulta)) {
        return;
    }

    std::array<vitralis::Medico, vitralis::MAX_MEDICOS> resultados{};
    const std::size_t total = servicio.buscarPorTexto(consulta, resultados);
    if (total == 0) {
        salida << "No se encontraron medicos.\n";
        return;
    }

    for (std::size_t indice = 0; indice < total; ++indice) {
        mostrarMedico(salida, resultados[indice]);
    }
}

void buscarMedicosPorEspecialidad(
    vitralis::ServicioMedicos& servicio,
    std::istream& entrada,
    std::ostream& salida) {
    vitralis::Especialidad especialidad{};
    if (!leerEspecialidad(entrada, salida, especialidad)) {
        return;
    }

    std::array<vitralis::Medico, vitralis::MAX_MEDICOS> resultados{};
    const std::size_t total = servicio.buscarPorEspecialidad(
        especialidad,
        resultados);
    if (total == 0) {
        salida << "No se encontraron medicos.\n";
        return;
    }

    for (std::size_t indice = 0; indice < total; ++indice) {
        mostrarMedico(salida, resultados[indice]);
    }
}

void editarMedico(
    vitralis::ServicioMedicos& servicio,
    std::istream& entrada,
    std::ostream& salida) {
    std::string codigo;
    if (!leerLinea(entrada, salida, "Codigo del medico: ", codigo)) {
        return;
    }

    vitralis::Medico actual{};
    std::string error;
    if (servicio.buscarPorCodigo(codigo, actual, error)
        != vitralis::ResultadoServicio::Exito) {
        salida << error << '\n';
        return;
    }

    if (!leerLinea(entrada, salida, "Nuevo nombre: ", actual.nombre)
        || !leerEspecialidad(entrada, salida, actual.especialidad)
        || !leerEstado(entrada, salida, actual.activo)) {
        return;
    }

    const auto resultado = servicio.editar(
        codigo,
        actual.nombre,
        actual.especialidad,
        actual.activo,
        error);
    mostrarResultado(salida, resultado, error);
}

void menuMedicos(
    vitralis::ServicioMedicos& servicio,
    std::istream& entrada,
    std::ostream& salida) {
    while (true) {
        salida << "\nMedicos\n"
               << "1. Registrar\n"
               << "2. Buscar por codigo\n"
               << "3. Buscar por texto\n"
               << "4. Buscar por especialidad\n"
               << "5. Editar\n"
               << "6. Listar\n"
               << "7. Cambiar estado\n"
               << "0. Volver\n";

        int opcion = 0;
        if (!leerEntero(entrada, salida, "Opcion: ", opcion) || opcion == 0) {
            return;
        }

        if (opcion == 1) {
            vitralis::Medico medico{};
            if (leerMedico(entrada, salida, medico)) {
                std::string error;
                const auto resultado = servicio.registrar(medico, error);
                mostrarResultado(salida, resultado, error);
            }
        } else if (opcion == 2) {
            buscarMedicoPorCodigo(servicio, entrada, salida);
        } else if (opcion == 3) {
            buscarMedicosPorTexto(servicio, entrada, salida);
        } else if (opcion == 4) {
            buscarMedicosPorEspecialidad(servicio, entrada, salida);
        } else if (opcion == 5) {
            editarMedico(servicio, entrada, salida);
        } else if (opcion == 6) {
            listarMedicos(servicio, salida);
        } else if (opcion == 7) {
            std::string codigo;
            bool activo = false;
            if (leerLinea(entrada, salida, "Codigo: ", codigo)
                && leerEstado(entrada, salida, activo)) {
                std::string error;
                const auto resultado = servicio.cambiarEstado(codigo, activo, error);
                mostrarResultado(salida, resultado, error);
            }
        } else {
            salida << "Opcion invalida.\n";
        }
    }
}

void mostrarEspecialidades(std::ostream& salida) {
    salida << "Especialidades disponibles:\n"
           << "- MEDICINAGENERAL\n"
           << "- PEDIATRIA\n"
           << "- CARDIOLOGIA\n"
           << "- GINECOLOGIA\n"
           << "- TRAUMATOLOGIA\n"
           << "- DERMATOLOGIA\n";
}

void mostrarPendiente(
    std::ostream& salida,
    std::string_view modulo) {
    salida << modulo
           << " queda pendiente de integracion con el equipo correspondiente.\n";
}

}  // namespace

namespace vitralis {

int ejecutarMenuConsola(
    ServicioPacientes& servicioPacientes,
    ServicioMedicos& servicioMedicos,
    std::istream& entrada,
    std::ostream& salida) {
    std::string error;
    if (servicioPacientes.cargar(error) != ResultadoServicio::Exito) {
        salida << "Aviso: no se pudieron cargar pacientes: " << error << '\n';
    }
    if (servicioMedicos.cargar(error) != ResultadoServicio::Exito) {
        salida << "Aviso: no se pudieron cargar medicos: " << error << '\n';
    }

    while (true) {
        salida << "\nVitralis Clinic\n"
               << "1. Pacientes\n"
               << "2. Medicos\n"
               << "3. Especialidades\n"
               << "4. Citas (pendiente)\n"
               << "5. Reportes (pendiente)\n"
               << "6. Login (pendiente)\n"
               << "0. Salir\n";

        int opcion = 0;
        if (!leerEntero(entrada, salida, "Opcion: ", opcion)) {
            return 0;
        }

        if (opcion == 0) {
            return 0;
        }
        if (opcion == 1) {
            menuPacientes(servicioPacientes, entrada, salida);
        } else if (opcion == 2) {
            menuMedicos(servicioMedicos, entrada, salida);
        } else if (opcion == 3) {
            mostrarEspecialidades(salida);
        } else if (opcion == 4) {
            mostrarPendiente(salida, "Citas");
        } else if (opcion == 5) {
            mostrarPendiente(salida, "Reportes");
        } else if (opcion == 6) {
            mostrarPendiente(salida, "Login");
        } else {
            salida << "Opcion invalida.\n";
        }
    }
}

}  // namespace vitralis
