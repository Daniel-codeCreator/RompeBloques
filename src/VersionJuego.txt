#include "RompeBloques.h"
#include "raylib.h"
#include <cstdlib>
#include <ctime>

/*
const int intBloquesVictoria = 100;
*/
const int intTiempoPower = 7;
const int intTiempoBarraGrande = 4;

const int filas = 8;
const int columnas = 23;
const int intBloquesVictoria = filas * columnas;


struct Bloque
{
    Rectangle rect;
    bool activo;
    Color color;
};

struct Pelota
{
    Vector2 pos;
    Vector2 vel;
    float radio;
    bool activa;
    bool bomba;
};

struct PowerUp
{
    Rectangle rect;
    int tipo;
    bool activo;
};

bool botonReiniciar(
    int x,
    int y,
    int ancho,
    int alto,
    const char* texto
) {

    Rectangle boton =
    {
        (float)x,
        (float)y,
        (float)ancho,
        (float)alto
    };

    Vector2 mouse = GetMousePosition();

    bool hover = CheckCollisionPointRec(mouse, boton);

    if (hover)
        DrawRectangleRec(boton, DARKGRAY);
    else
        DrawRectangleRec(boton, GRAY);

    DrawRectangleLinesEx(boton, 2, WHITE);

    int textoAncho = MeasureText(texto, 20);

    DrawText(
        texto,
        x + (ancho - textoAncho) / 2,
        y + 18,
        20,
        WHITE
    );

    return hover &&
           IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void iniciarRompeBloque()
{
    // USA LA MISMA VENTANA
    const int ancho = 1200;
    const int alto = 720;

    SetTargetFPS(60);

    srand(time(NULL));

    Rectangle barra =
    {
        500.0f,
        650.0f,
        140.0f,
        20.0f
    };

    const int MAX_PELOTAS = 10;

    Pelota pelotas[MAX_PELOTAS];

    for (int i = 0; i < MAX_PELOTAS; i++)
        pelotas[i].activa = false;

    pelotas[0] =
    {
        {barra.x + barra.width / 2, barra.y - 10},
        {0.0f, 0.0f},
        8.0f,
        true,
        false
    };

    bool esperandoLanzar = true;

    Bloque bloques[filas][columnas];

    Color colores[] =
    {
        RED,
        ORANGE,
        YELLOW,
        GREEN,
        BLUE,
        PURPLE
    };


    /*
    Controla el tamaño de los bloques
    */
    /*for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            bloques[i][j].rect =
            {
                (float)(j * 100 + 90),
                (float)(i * 35 + 60),
                90.0f,
                30.0f
            };

            bloques[i][j].activo = true;
            bloques[i][j].color = colores[rand() % 6];
        }
    }*/

    for(int i=0;i<filas;i++)
    {
        for(int j=0;j<columnas;j++)
        {
            bloques[i][j].rect = {(float)(j*50+25),(float)(i*25+40),45.0f,20.0f};
            bloques[i][j].activo = true;
            bloques[i][j].color = colores[rand()%6];
        }
    }

    int puntos = 0;
    int vidas = 3;
    int bloquesDestruidos = 0;

    bool ganar = false;
    bool perder = false;

    PowerUp power;
    power.activo = false;

    double ultimoPower = GetTime();

    bool barraGrandeActiva = false;
    double tiempoBarraGrande = 0;

    while (!WindowShouldClose())
    {

        // =========================
        // LOGICA
        // =========================

        if (!ganar && !perder)
        {

            if (IsKeyDown(KEY_LEFT))
                barra.x -= 7.0f;

            if (IsKeyDown(KEY_RIGHT))
                barra.x += 7.0f;

            if (barra.x < 0)
                barra.x = 0;

            if (barra.x + barra.width > ancho)
                barra.x = ancho - barra.width;

            if (esperandoLanzar)
            {
                pelotas[0].pos.x =
                    barra.x + barra.width / 2;

                pelotas[0].pos.y =
                    barra.y - pelotas[0].radio;

                if (IsKeyPressed(KEY_LEFT) ||
                    IsKeyPressed(KEY_RIGHT))
                {
                    pelotas[0].vel =
                    {
                        4.0f,
                        -4.0f
                    };

                    esperandoLanzar = false;
                }
            }

            if (GetTime() - ultimoPower >
                intTiempoPower)
            {
                power.activo = true;

                power.tipo = rand() % 4;

                power.rect =
                {
                    (float)(rand() % (ancho - 20)),
                    200.0f,
                    20.0f,
                    20.0f
                };

                ultimoPower = GetTime();
            }

            if (power.activo)
                power.rect.y += 2.0f;

            if (barraGrandeActiva &&
                GetTime() - tiempoBarraGrande >
                intTiempoBarraGrande)
            {
                barra.width = 140.0f;
                barraGrandeActiva = false;
            }

            for (int p = 0;
                 p < MAX_PELOTAS &&
                 !esperandoLanzar;
                 p++)
            {

                if (!pelotas[p].activa)
                    continue;

                pelotas[p].pos.x +=
                    pelotas[p].vel.x;

                pelotas[p].pos.y +=
                    pelotas[p].vel.y;

                if (pelotas[p].pos.x <
                    pelotas[p].radio ||
                    pelotas[p].pos.x >
                    ancho - pelotas[p].radio)
                {
                    pelotas[p].vel.x *= -1;
                }

                if (pelotas[p].pos.y <
                    pelotas[p].radio)
                {
                    pelotas[p].vel.y *= -1;
                }

                if (pelotas[p].pos.y > alto)
                {
                    pelotas[p].activa = false;
                }

                if (CheckCollisionCircleRec(
                        pelotas[p].pos,
                        pelotas[p].radio,
                        barra))
                {
                    pelotas[p].vel.y *= -1;

                    pelotas[p].pos.y =
                        barra.y - pelotas[p].radio;
                }

                for (int i = 0; i < filas; i++)
                {
                    for (int j = 0; j < columnas; j++)
                    {

                        if (!bloques[i][j].activo)
                            continue;

                        if (CheckCollisionCircleRec(
                                pelotas[p].pos,
                                pelotas[p].radio,
                                bloques[i][j].rect))
                        {

                            bloques[i][j].activo =
                                false;

                            pelotas[p].vel.y *= -1;

                            puntos += 10;
                            bloquesDestruidos++;

                            int intPelotasDestruidos = 25;

                            if (bloquesDestruidos %
                                intPelotasDestruidos == 0)
                            {
                                pelotas[p].vel.x *= 1.1f;
                                pelotas[p].vel.y *= 1.1f;
                            }

                            if (pelotas[p].bomba)
                            {

                                for (int a = -1; a <= 1; a++)
                                {
                                    for (int b = -1; b <= 1; b++)
                                    {

                                        int ni = i + a;
                                        int nj = j + b;

                                        if (ni >= 0 &&
                                            ni < filas &&
                                            nj >= 0 &&
                                            nj < columnas)
                                        {

                                            if (bloques[ni][nj].activo)
                                            {
                                                bloques[ni][nj].activo =
                                                    false;

                                                puntos += 10;
                                                bloquesDestruidos++;
                                            }
                                        }
                                    }
                                }

                                pelotas[p].bomba = false;
                            }

                            break;
                        }
                    }
                }
            }

            bool algunaPelotaActiva = false;

            for (int i = 0; i < MAX_PELOTAS; i++)
            {
                if (pelotas[i].activa)
                {
                    algunaPelotaActiva = true;
                    break;
                }
            }

            if (!algunaPelotaActiva)
            {

                vidas--;

                if (vidas <= 0)
                {
                    perder = true;
                }
                else
                {

                    pelotas[0] =
                    {
                        {barra.x + barra.width / 2,
                         barra.y - 10},

                        {0.0f, 0.0f},

                        8.0f,
                        true,
                        false
                    };

                    esperandoLanzar = true;
                }
            }

            if (power.activo)
            {

                if (CheckCollisionRecs(
                        power.rect,
                        barra))
                {

                    if (power.tipo == 0)
                    {

                        for (int i = 0;
                             i < MAX_PELOTAS;
                             i++)
                        {

                            if (!pelotas[i].activa)
                            {

                                pelotas[i] =
                                {
                                    {barra.x +
                                     barra.width / 2,
                                     barra.y - 10},

                                    {-4.0f, -4.0f},

                                    8.0f,
                                    true,
                                    false
                                };

                                break;
                            }
                        }
                    }

                    if (power.tipo == 1)
                    {

                        for (int k = 0; k < 2; k++)
                        {
                            for (int i = 0;
                                 i < MAX_PELOTAS;
                                 i++)
                            {

                                if (!pelotas[i].activa)
                                {

                                    float velocidadX =
                                        (float)((rand() % 5) - 2);

                                    if (velocidadX == 0)
                                        velocidadX = 2.0f;

                                    pelotas[i] =
                                    {
                                        {barra.x +
                                         barra.width / 2,
                                         barra.y - 10},

                                        {velocidadX,
                                         -4.0f},

                                        8.0f,
                                        true,
                                        false
                                    };

                                    break;
                                }
                            }
                        }
                    }

                    if (power.tipo == 2)
                    {
                        pelotas[0].bomba = true;
                    }

                    if (power.tipo == 3)
                    {
                        barra.width = 220.0f;

                        barraGrandeActiva = true;

                        tiempoBarraGrande =
                            GetTime();
                    }

                    power.activo = false;
                }
            }

            if (bloquesDestruidos >=
                intBloquesVictoria)
            {
                ganar = true;
            }
        }

        // =========================
        // DIBUJO
        // =========================

        BeginDrawing();

        ClearBackground(BLACK);

        DrawRectangleRec(barra, WHITE);

        for (int p = 0; p < MAX_PELOTAS; p++)
        {
            if (pelotas[p].activa)
            {
                DrawCircleV(
                    pelotas[p].pos,
                    pelotas[p].radio,
                    SKYBLUE
                );
            }
        }

        for (int i = 0; i < filas; i++)
        {
            for (int j = 0; j < columnas; j++)
            {

                if (bloques[i][j].activo)
                {
                    DrawRectangleRec(
                        bloques[i][j].rect,
                        bloques[i][j].color
                    );
                }
            }
        }

        if (power.activo)
        {

            Color c = YELLOW;

            if (power.tipo == 1)
                c = GREEN;

            if (power.tipo == 2)
                c = RED;

            if (power.tipo == 3)
                c = BLUE;

            DrawRectangleRec(power.rect, c);
        }

        DrawText(
            TextFormat("Puntos: %i", puntos),
            20,
            10,
            20,
            WHITE
        );

        DrawText(
            TextFormat("Vidas: %i", vidas),
            1000,
            10,
            20,
            WHITE
        );

        // =========================
        // GANAR
        // =========================

        if (ganar)
        {

            DrawText(
                "GANASTE!",
                470,
                300,
                50,
                GREEN
            );

            if (botonReiniciar(
                    500,
                    400,
                    220,
                    60,
                    "Volver a jugar"))
            {

                barra =
                {
                    500.0f,
                    650.0f,
                    140.0f,
                    20.0f
                };

                for (int i = 0;
                     i < MAX_PELOTAS;
                     i++)
                {
                    pelotas[i].activa = false;
                }

                pelotas[0] =
                {
                    {barra.x + barra.width / 2,
                     barra.y - 10},

                    {0.0f, 0.0f},

                    8.0f,
                    true,
                    false
                };

                esperandoLanzar = true;

                for (int i = 0; i < filas; i++)
                {
                    for (int j = 0; j < columnas; j++)
                    {
                        bloques[i][j].activo = true;
                        bloques[i][j].color =
                            colores[rand() % 6];
                    }
                }

                puntos = 0;
                vidas = 3;
                bloquesDestruidos = 0;

                ganar = false;
                perder = false;

                power.activo = false;

                barraGrandeActiva = false;
            }
        }

        // =========================
        // PERDER
        // =========================

        if (perder)
        {

            DrawText(
                "GAME OVER",
                430,
                300,
                50,
                RED
            );

            if (botonReiniciar(
                    500,
                    400,
                    220,
                    60,
                    "Volver a jugar"))
            {

                barra =
                {
                    500.0f,
                    650.0f,
                    140.0f,
                    20.0f
                };

                for (int i = 0;
                     i < MAX_PELOTAS;
                     i++)
                {
                    pelotas[i].activa = false;
                }

                pelotas[0] =
                {
                    {barra.x + barra.width / 2,
                     barra.y - 10},

                    {0.0f, 0.0f},

                    8.0f,
                    true,
                    false
                };

                esperandoLanzar = true;

                for (int i = 0; i < filas; i++)
                {
                    for (int j = 0; j < columnas; j++)
                    {
                        bloques[i][j].activo = true;
                        bloques[i][j].color =
                            colores[rand() % 6];
                    }
                }

                puntos = 0;
                vidas = 3;
                bloquesDestruidos = 0;

                ganar = false;
                perder = false;

                power.activo = false;

                barraGrandeActiva = false;
            }
        }

        EndDrawing();
    }
}