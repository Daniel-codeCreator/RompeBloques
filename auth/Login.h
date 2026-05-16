#ifndef ROMPEBLOQUES_LOGIN_H
#define ROMPEBLOQUES_LOGIN_H

#include <string>
#include "../Api´s/ApiClient.h"

class Login {

private:
    ApiClient& api;
    UsuarioApi usuarioActual;
    bool autenticado;

public:

    explicit Login(ApiClient& apiClient);

    bool iniciarSesion(
        const std::string& username,
        const std::string& password,
        std::string& mensajeError
    );

    bool estaAutenticado() const;

    UsuarioApi getUsuarioActual() const;

    void cerrarSesion();
};

#endif