#include <iostream>
#include <string>

#include "Api´s/ApiClient.h"
#include "Api´s/GameApiConfig.h"
#include "auth/Login.h"

int main() {

    ApiClient api(
        GameApiConfig::BASE_URL,
        GameApiConfig::API_KEY,
        GameApiConfig::CODIGO_JUEGO
    );

    Login login(api);

    std::string username;
    std::string password;
    std::string error;

    std::cout << "Usuario: ";
    std::cin >> username;

    std::cout << "Password: ";
    std::cin >> password;

    bool ok = login.iniciarSesion(
        username,
        password,
        error
    );

    if (!ok) {
        std::cout << "Error Login: " << error << std::endl;
        return 0;
    }

    std::cout << "Login exitoso!" << std::endl;

    return 0;
}