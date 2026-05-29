#include "raylib.h"
#include "Api´s/ApiClient.h"
#include "Api´s/GameApiConfig.h"
#include "auth/Login.h"
#include "src/Menu.h"

int main() {

    InitWindow(1920, 1095, "Proyecto Arcade");
    SetTargetFPS(60);

    ApiClient api(
        GameApiConfig::BASE_URL,
        GameApiConfig::API_KEY,
        GameApiConfig::CODIGO_JUEGO
    );

    while (!WindowShouldClose()) {

        // LOGIN
        Login login(api);
        login.mostrarLogin();

        // SI EL USUARIO CERRO LA VENTANA
        if (WindowShouldClose()) {
            break;
        }

        // SI LOGIN EXITOSO
        if (login.estaAutenticado()) {

            Menu menu(api);
            menu.mostrar();
        }
    }

    CloseWindow();
    return 0;
}