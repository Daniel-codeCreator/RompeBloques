#include "RompeBloques.h"
#include "raylib.h"
#include <cstdlib>
#include <ctime>

// ===========================
// CONSTANTES
// ===========================

const int intTiempoPower       = 7;
const int intTiempoBarraGrande = 4;
const int filas                = 8;
const int columnas             = 23;
const int intBloquesVictoria   = filas * columnas;
const int ancho                = 1200;
const int alto                 = 720;
const int MAX_PELOTAS          = 10;

// ===========================
// STRUCTS
// ===========================

struct Bloque
{
    Rectangle rect;
    bool      activo;
    Color     color;
};

struct Pelota
{
    Vector2 pos;
    Vector2 vel;
    float   radio;
    bool    activa;
    bool    bomba;
};

struct PowerUp
{
    Rectangle rect;
    int       tipo;
    bool      activo;
};

// ===========================
// FUNCION: botonReiniciar
// ===========================

bool botonReiniciar(int x, int y, int ancho, int alto, const char* texto)
{
    Rectangle boton = { (float)x, (float)y, (float)ancho, (float)alto };
    Vector2 mouse   = GetMousePosition();
    bool hover      = CheckCollisionPointRec(mouse, boton);

    if (hover)
        DrawRectangleRec(boton, DARKGRAY);
    else
        DrawRectangleRec(boton, GRAY);

    DrawRectangleLinesEx(boton, 2, WHITE);

    int textoAncho = MeasureText(texto, 20);
    DrawText(texto, x + (ancho - textoAncho) / 2, y + 18, 20, WHITE);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ===========================
// FUNCION: moverBarra
// ===========================

void moverBarra(Rectangle& barra)
{
    if (IsKeyDown(KEY_LEFT))
        barra.x -= 7.0f;

    if (IsKeyDown(KEY_RIGHT))
        barra.x += 7.0f;

    if (barra.x < 0)
        barra.x = 0;

    if (barra.x + barra.width > ancho)
        barra.x = ancho - barra.width;
}

// ===========================
// FUNCION: dibujarBarra
// ===========================

void dibujarBarra(Rectangle barra)
{
    DrawRectangleRec(barra, WHITE);
}

// ===========================
// FUNCION: dibujarPelotas
// ===========================

void dibujarPelotas(Pelota pelotas[], int maxPelotas)
{
    for (int p = 0; p < maxPelotas; p++)
    {
        if (pelotas[p].activa)
        {
            Color color = pelotas[p].bomba ? RED : SKYBLUE;
            DrawCircleV(pelotas[p].pos, pelotas[p].radio, color);
        }
    }
}

// ===========================
// FUNCION: dibujarBloques
// ===========================

void dibujarBloques(Bloque bloques[filas][columnas])
{
    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
            if (bloques[i][j].activo)
                DrawRectangleRec(bloques[i][j].rect, bloques[i][j].color);
}


void dibujarPowerUp(PowerUp power)
{
    if (!power.activo)
        return;

    /*aplicarPowerBolaExtra()  Tipo 0 — amarillo
    aplicarPowerDobleBola    Tipo 1 — verde
    aplicarPowerBomba()      Tipo 2 — rojo
    aplicarPowerBarraGrande()  Tipo 3 — azul*/

    Color c = YELLOW;
    if (power.tipo == 1) c = GREEN;
    if (power.tipo == 2) c = RED;
    if (power.tipo == 3) c = BLUE;

    DrawRectangleRec(power.rect, c);
}

//dibuja los puntos y las vidas
void dibujarHUD(int puntos, int vidas)
{
    DrawText(TextFormat("Puntos: %i", puntos), 20,   10, 20, WHITE);
    DrawText(TextFormat("Vidas: %i",  vidas),  1000, 10, 20, WHITE);
}


void aplicarPowerBolaExtra(Pelota pelotas[], int maxPelotas, Rectangle barra)
{
    for (int i = 0; i < maxPelotas; i++)
    {
        if (!pelotas[i].activa)
        {
            pelotas[i] = {
                { barra.x + barra.width / 2, barra.y - 10 },
                { -4.0f, -4.0f },
                8.0f, true, false
            };
            break;
        }
    }
}


void aplicarPowerDobleBola(Pelota pelotas[], int maxPelotas, Rectangle barra)
{
    for (int k = 0; k < 2; k++)
    {
        for (int i = 0; i < maxPelotas; i++)
        {
            if (!pelotas[i].activa)
            {
                float velocidadX = (float)((rand() % 5) - 2);
                if (velocidadX == 0) velocidadX = 2.0f;

                pelotas[i] = {
                    { barra.x + barra.width / 2, barra.y - 10 },
                    { velocidadX, -4.0f },
                    8.0f, true, false
                };
                break;
            }
        }
    }
}

void aplicarPowerBomba(Pelota pelotas[])
{
    pelotas[0].bomba = true;
}

void aplicarPowerBarraGrande(
    Rectangle& barra,
    bool&      barraGrandeActiva,
    double&    tiempoBarraGrande)
{
    barra.width       = 220.0f;
    barraGrandeActiva = true;
    tiempoBarraGrande = GetTime();
}

void actualizarPowerUp(
    PowerUp&   power,
    Rectangle& barra,
    Pelota     pelotas[],
    int        maxPelotas,
    bool&      barraGrandeActiva,
    double&    tiempoBarraGrande,
    double&    ultimoPower)
{
    // Generar nuevo power-up si paso el tiempo
    if (GetTime() - ultimoPower > intTiempoPower)
    {
        power.activo = true;
        power.tipo   = rand() % 4;
        power.rect   = { (float)(rand() % (ancho - 20)), 200.0f, 20.0f, 20.0f };
        ultimoPower  = GetTime();
    }

    // Mover power-up hacia abajo
    if (power.activo)
        power.rect.y += 2.0f;

    // Desactivar barra grande si paso su tiempo
    if (barraGrandeActiva &&
        GetTime() - tiempoBarraGrande > intTiempoBarraGrande)
    {
        barra.width       = 140.0f;
        barraGrandeActiva = false;
    }

    // Verificar colision power-up con barra
    if (power.activo && CheckCollisionRecs(power.rect, barra))
    {
        if (power.tipo == 0) aplicarPowerBolaExtra(pelotas, maxPelotas, barra);
        if (power.tipo == 1) aplicarPowerDobleBola(pelotas, maxPelotas, barra);
        if (power.tipo == 2) aplicarPowerBomba(pelotas);
        if (power.tipo == 3) aplicarPowerBarraGrande(barra, barraGrandeActiva, tiempoBarraGrande);

        power.activo = false;
    }
}

// ===========================
// FUNCION: moverPelotas
// ===========================

void moverPelotas(
    Pelota    pelotas[],
    int       maxPelotas,
    Rectangle barra,
    Bloque    bloques[filas][columnas],
    int&      puntos,
    int&      bloquesDestruidos,
    float&    velocidadActual)
{
    for (int p = 0; p < maxPelotas; p++)
    {
        if (!pelotas[p].activa)
            continue;

        pelotas[p].pos.x += pelotas[p].vel.x;
        pelotas[p].pos.y += pelotas[p].vel.y;

        // Rebote en paredes laterales
        if (pelotas[p].pos.x < pelotas[p].radio ||
            pelotas[p].pos.x > ancho - pelotas[p].radio)
            pelotas[p].vel.x *= -1;

        // Rebote en techo
        if (pelotas[p].pos.y < pelotas[p].radio)
            pelotas[p].vel.y *= -1;

        // Pelota cae fuera
        if (pelotas[p].pos.y > alto)
        {
            pelotas[p].activa = false;
            continue;
        }

        // Colision con barra
        if (CheckCollisionCircleRec(pelotas[p].pos, pelotas[p].radio, barra))
        {
            pelotas[p].vel.y *= -1;
            pelotas[p].pos.y  = barra.y - pelotas[p].radio;
        }

        // Colision con bloques
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
                    bloques[i][j].activo = false;
                    pelotas[p].vel.y    *= -1;
                    puntos++;
                    bloquesDestruidos++;

                    // Aumentar velocidad cada 15 bloques y guardarla
                    const int intPelotasDestruidos = 15;
                    if (bloquesDestruidos > 0 &&
                        bloquesDestruidos % intPelotasDestruidos == 0)
                    {
                        pelotas[p].vel.x *= 1.5f;
                        pelotas[p].vel.y *= 1.5f;
                        velocidadActual = pelotas[p].vel.x; // guardar velocidad acumulada
                    }

                    // Efecto bomba
                    if (pelotas[p].bomba)
                    {
                        for (int a = -1; a <= 1; a++)
                        {
                            for (int b = -1; b <= 1; b++)
                            {
                                int ni = i + a;
                                int nj = j + b;

                                if (ni >= 0 && ni < filas &&
                                    nj >= 0 && nj < columnas)
                                {
                                    if (bloques[ni][nj].activo)
                                    {
                                        bloques[ni][nj].activo = false;
                                        puntos++;
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
}

// ===========================
// FUNCION: reiniciarJuego
// ===========================

void reiniciarJuego(
    Rectangle& barra,
    Pelota     pelotas[],
    int        maxPelotas,
    Bloque     bloques[filas][columnas],
    Color      colores[],
    int&       puntos,
    int&       vidas,
    int&       bloquesDestruidos,
    bool&      ganar,
    bool&      perder,
    bool&      esperandoLanzar,
    PowerUp&   power,
    bool&      barraGrandeActiva,
    float&     velocidadActual)
{
    barra           = { 500.0f, 650.0f, 140.0f, 20.0f };
    velocidadActual = 4.0f;   // velocidad base al reiniciar todo

    for (int i = 0; i < maxPelotas; i++)
        pelotas[i].activa = false;

    pelotas[0] = {
        { barra.x + barra.width / 2, barra.y - 10 },
        { 0.0f, 0.0f },
        8.0f, true, false
    };

    esperandoLanzar = true;

    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
        {
            bloques[i][j].activo = true;
            bloques[i][j].color  = colores[rand() % 6];
        }

    puntos            = 0;
    vidas             = 3;
    bloquesDestruidos = 0;
    ganar             = false;
    perder            = false;
    power.activo      = false;
    barraGrandeActiva = false;
}

// ===========================
// FUNCION PRINCIPAL
// ===========================

void iniciarRompeBloque()
{
    SetTargetFPS(60);
    srand(time(NULL));

    Color colores[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };

    Rectangle barra = { 500.0f, 650.0f, 140.0f, 20.0f };

    Pelota pelotas[MAX_PELOTAS];
    for (int i = 0; i < MAX_PELOTAS; i++)
        pelotas[i].activa = false;

    pelotas[0] = {
        { barra.x + barra.width / 2, barra.y - 10 },
        { 0.0f, 0.0f },
        8.0f, true, false
    };

    bool  esperandoLanzar = true;
    float velocidadActual = 4.0f;  // velocidad base, se acumula con los bloques

    Bloque bloques[filas][columnas];
    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
        {
            bloques[i][j].rect   = { (float)(j * 50 + 25), (float)(i * 25 + 40), 45.0f, 20.0f };
            bloques[i][j].activo = true;
            bloques[i][j].color  = colores[rand() % 6];
        }

    int puntos            = 0;
    int vidas             = 3;
    int bloquesDestruidos = 0;

    bool ganar  = false;
    bool perder = false;

    PowerUp power  = {};
    power.activo   = false;
    double ultimoPower = GetTime();

    bool   barraGrandeActiva = false;
    double tiempoBarraGrande = 0;

    while (!WindowShouldClose())
    {
        // =========================
        // LOGICA
        // =========================

        if (!ganar && !perder)
        {
            moverBarra(barra);

            if (esperandoLanzar)
            {
                pelotas[0].pos.x = barra.x + barra.width / 2;
                pelotas[0].pos.y = barra.y - pelotas[0].radio;

                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))
                {
                    // Usar velocidad acumulada, no siempre 4.0f fija
                    pelotas[0].vel  = { velocidadActual, -velocidadActual };
                    esperandoLanzar = false;
                }
            }

            actualizarPowerUp(
                power, barra, pelotas, MAX_PELOTAS,
                barraGrandeActiva, tiempoBarraGrande, ultimoPower);

            if (!esperandoLanzar)
                moverPelotas(pelotas, MAX_PELOTAS, barra,
                             bloques, puntos, bloquesDestruidos,
                             velocidadActual);

            // Verificar si todas las pelotas cayeron
            bool algunaPelotaActiva = false;
            for (int i = 0; i < MAX_PELOTAS; i++)
                if (pelotas[i].activa) { algunaPelotaActiva = true; break; }

            if (!algunaPelotaActiva)
            {
                vidas--;

                if (vidas <= 0)
                {
                    perder = true;
                }
                else
                {
                    // Restaurar pelota SIN resetear velocidadActual
                    pelotas[0] = {
                        { barra.x + barra.width / 2, barra.y - 10 },
                        { 0.0f, 0.0f },
                        8.0f, true, false
                    };
                    esperandoLanzar = true;
                }
            }

            if (bloquesDestruidos >= intBloquesVictoria)
                ganar = true;
        }

        // =========================
        // DIBUJO
        // =========================

        BeginDrawing();
        ClearBackground(BLACK);

        dibujarBarra(barra);
        dibujarPelotas(pelotas, MAX_PELOTAS);
        dibujarBloques(bloques);
        dibujarPowerUp(power);
        dibujarHUD(puntos, vidas);

        // PANTALLA GANAR
        if (ganar)
        {
            DrawText("GANASTE!", 470, 300, 50, GREEN);

            if (botonReiniciar(500, 400, 220, 60, "Volver a jugar"))
                reiniciarJuego(
                    barra, pelotas, MAX_PELOTAS, bloques, colores,
                    puntos, vidas, bloquesDestruidos,
                    ganar, perder, esperandoLanzar,
                    power, barraGrandeActiva, velocidadActual);
        }

        // PANTALLA PERDER
        if (perder)
        {
            DrawText("GAME OVER", 430, 300, 50, RED);

            if (botonReiniciar(500, 400, 220, 60, "Volver a jugar"))
                reiniciarJuego(
                    barra, pelotas, MAX_PELOTAS, bloques, colores,
                    puntos, vidas, bloquesDestruidos,
                    ganar, perder, esperandoLanzar,
                    power, barraGrandeActiva, velocidadActual);
        }

        EndDrawing();
    }
}