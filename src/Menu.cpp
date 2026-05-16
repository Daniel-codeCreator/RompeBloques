#include "Menu.h"
#include "raylib.h"
#include "RompeBloques.h"

void Menu::mostrar() {

    Rectangle botonJugar = { 450, 230, 220, 40 };

    while (!WindowShouldClose()) {

        Vector2 mouse = GetMousePosition();

        bool click =
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // =========================
        // CLICK BOTON JUGAR
        // =========================

        if (click &&
            CheckCollisionPointRec(mouse, botonJugar))
        {
            iniciarRompeBloque();
            break;
        }

        // =========================
        // DIBUJO
        // =========================

        BeginDrawing();

        ClearBackground(BLACK);

        DrawText(
            "BIENVENIDOS A PROYECTO ARCADE",
            400,
            150,
            20,
            WHITE
        );

        // BOTON
        DrawRectangleRec(botonJugar, GREEN);

        DrawText(
            "Jugar",
            520,
            240,
            20,
            BLACK
        );

        EndDrawing();
    }
}