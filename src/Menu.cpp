#include "Menu.h"
#include "raylib.h"
#include "RompeBloques.h"

Menu::Menu(ApiClient& apiClient) : api(apiClient) {}

void Menu::mostrar() {

    int screenWidth  = 1920;
    int screenHeight = 1095;

    int centroX = screenWidth / 2;
    int centroY = screenHeight / 2;

    // BOTON JUGAR
    int botonWidth  = 220;
    int botonHeight = 40;

    Rectangle botonJugar = {
        (float)(centroX - botonWidth / 2),
        (float)(centroY),
        (float)botonWidth,
        (float)botonHeight
    };

    // BOTON CERRAR SESION
    Rectangle botonCerrarSesion = {
        (float)(centroX - botonWidth / 2),
        (float)(centroY + 80),
        (float)botonWidth,
        (float)botonHeight
    };

    bool salirMenu = false;

    while (!WindowShouldClose() && !salirMenu) {

        Vector2 mouse = GetMousePosition();
        bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // JUGAR
        if (click && CheckCollisionPointRec(mouse, botonJugar))
        {
            iniciarRompeBloque(api);
        }

        // CERRAR SESION
        if (click && CheckCollisionPointRec(mouse, botonCerrarSesion))
        {
            api.cerrarSesion();
            salirMenu = true;
        }

        BeginDrawing();

        ClearBackground(BLACK);

        // TITULO
        const char* titulo = "BIENVENIDOS A PROYECTO ARCADE";

        int tituloSize  = 30;
        int tituloWidth = MeasureText(titulo, tituloSize);

        DrawText(
            titulo,
            centroX - tituloWidth / 2,
            centroY - 120,
            tituloSize,
            WHITE
        );

        // BOTON JUGAR
        DrawRectangleRec(botonJugar, GREEN);

        const char* textoJugar = "Jugar";

        int textoJugarWidth = MeasureText(textoJugar, 20);

        DrawText(
            textoJugar,
            botonJugar.x + (botonWidth / 2) - (textoJugarWidth / 2),
            botonJugar.y + 10,
            20,
            BLACK
        );

        // BOTON CERRAR SESION
        DrawRectangleRec(botonCerrarSesion, RED);

        const char* textoCerrar = "Cerrar Sesion";

        int textoCerrarWidth = MeasureText(textoCerrar, 20);

        DrawText(
            textoCerrar,
            botonCerrarSesion.x + (botonWidth / 2) - (textoCerrarWidth / 2),
            botonCerrarSesion.y + 10,
            20,
            WHITE
        );

        EndDrawing();
    }
}