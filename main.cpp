#include "raylib.h"

#include "Api´s/ApiClient.h"
#include "Api´s/GameApiConfig.h"
#include "auth/Login.h"

int main() {

    InitWindow(1920, 1200, "Proyecto Arcade");

    SetTargetFPS(60);

    ApiClient api(
        GameApiConfig::BASE_URL,
        GameApiConfig::API_KEY,
        GameApiConfig::CODIGO_JUEGO
    );

    Login login(api);

    login.mostrarLogin();

    CloseWindow();

    return 0;
}