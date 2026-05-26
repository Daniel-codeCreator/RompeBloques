#include "Menu.h"
#include "raylib.h"
#include "RompeBloques.h"


void Menu::mostrar() {

    Rectangle botonJugar  = { 450, 230, 220, 40 };
    /*
    Rectangle botonRanking = { 450, 300, 220, 40 };
    */

    while (!WindowShouldClose()) {

        Vector2 mouse = GetMousePosition();

        bool click =
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // BOTON JUGAR
        if (click &&
            CheckCollisionPointRec(mouse, botonJugar))
        {
            iniciarRompeBloque();
            break;
        }


        BeginDrawing();

        ClearBackground(BLACK);

        DrawText(
            "BIENVENIDOS A PROYECTO ARCADE",
            330,
            150,
            30,
            WHITE
        );

        // BOTON JUGAR
        DrawRectangleRec(botonJugar, GREEN);

        DrawText(
            "Jugar",
            530,
            240,
            20,
            BLACK
        );

        // BOTON RANKING
        /*DrawRectangleRec(botonRanking, BLUE);

        DrawText(
            "Consultar Ranking",
            475,
            310,
            20,
            WHITE
        );*/

        EndDrawing();
    }
}