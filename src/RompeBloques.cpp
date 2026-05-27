#include "RompeBloques.h"
#include "raylib.h"
#include <cstdlib>
#include <ctime>

// ===========================
// CONSTANTES
// ===========================

const int intTiempoPower       = 7;
const int intTiempoBarraGrande = 4;
const int filas                = 4;
const int columnas             = 8;
const int MAX_NIVELES          = 3;

// Ventana total
const int ancho                = 1920;
const int alto                 = 1095;

// Area de juego (panel izquierdo)
const int juegoX               = 40;
const int juegoY               = 40;
const int juegoAncho           = 1400;
const int juegoAlto            = 1015;

// Panel HUD (panel derecho)
const int hudX                 = juegoX + juegoAncho + 40;
const int hudAncho             = ancho - hudX - 20;

const int intBloquesVictoria   = filas * columnas;
const int MAX_PELOTAS          = 10;

// Velocidad base por nivel
const float velocidadPorNivel[MAX_NIVELES] = { 4.0f, 6.0f, 9.0f };

// ===========================
// STRUCTS
// ===========================

struct Bloque
{
    Rectangle rect;
    bool      activo;
    Color     color;
    int       vida;      // 1 = normal, 2 = resistente (doble golpe)
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
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A))
        barra.x -= 7.0f;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        barra.x += 7.0f;

    if (barra.x < juegoX)
        barra.x = juegoX;

    if (barra.x + barra.width > juegoX + juegoAncho)
        barra.x = juegoX + juegoAncho - barra.width;
}

// ===========================
// FUNCION: dibujarAreaJuego
// ===========================

void dibujarAreaJuego()
{
    DrawRectangleLinesEx({ 10, 10, (float)(ancho - 20), (float)(alto - 20) }, 3, DARKGRAY);
    DrawRectangleLinesEx(
        { (float)(juegoX - 5), (float)(juegoY - 5),
          (float)(juegoAncho + 10), (float)(juegoAlto + 10) },
        3, DARKGRAY);
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
// Bloques con esquinas redondeadas, borde oscuro
// y grieta si vida == 1 (ya recibio un golpe)
// ===========================

void dibujarBloques(Bloque bloques[filas][columnas])
{
    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            if (!bloques[i][j].activo)
                continue;

            Rectangle r = bloques[i][j].rect;
            Color c     = bloques[i][j].color;

            // Sombra / borde oscuro
            Color borde = { (unsigned char)(c.r / 2),
                            (unsigned char)(c.g / 2),
                            (unsigned char)(c.b / 2), 255 };

            DrawRectangleRounded(r, 0.3f, 6, c);
            DrawRectangleRoundedLinesEx(r, 0.3f, 6, 3.0f, borde);

            // Si el bloque resistente ya recibio un golpe (vida == 1), dibujar grieta
            if (bloques[i][j].vida == 1)
            {
                // Linea diagonal simulando grieta
                int cx = (int)(r.x + r.width  / 2);
                int cy = (int)(r.y + r.height / 2);
                DrawLine(cx - 8, cy - 6, cx + 4, cy + 8, BLACK);
                DrawLine(cx + 4, cy + 8, cx - 2, cy + 14, BLACK);
                DrawLine(cx - 8, cy - 6, cx - 2, cy - 14, BLACK);
            }
        }
    }
}

// ===========================
// FUNCION: dibujarPowerUp
// ===========================

void dibujarPowerUp(PowerUp power)
{
    if (!power.activo)
        return;

    Color c = YELLOW;
    if (power.tipo == 1) c = GREEN;
    if (power.tipo == 2) c = RED;
    if (power.tipo == 3) c = BLUE;

    DrawRectangleRec(power.rect, c);
}

// ===========================
// FUNCION: dibujarHUD
// ===========================

void dibujarHUD(int puntos, int vidas, int nivel)
{
    DrawRectangleLinesEx(
        { (float)(hudX - 10), (float)(juegoY - 5),
          (float)(hudAncho + 10), (float)(juegoAlto + 10) },
        3, DARKGRAY);

    DrawText("VIDAS",  hudX + 20, juegoY + 30,  28, LIGHTGRAY);
    DrawText(TextFormat("%i", vidas),  hudX + 20, juegoY + 70,  40, WHITE);

    DrawText("PUNTOS", hudX + 20, juegoY + 200, 28, LIGHTGRAY);
    DrawText(TextFormat("%i", puntos), hudX + 20, juegoY + 240, 40, WHITE);

    DrawText("NIVEL",  hudX + 20, juegoY + 380, 28, LIGHTGRAY);
    DrawText(TextFormat("%i / %i", nivel, MAX_NIVELES), hudX + 20, juegoY + 420, 40, WHITE);
}

// ===========================
// POWER UPS
// ===========================

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
    if (GetTime() - ultimoPower > intTiempoPower)
    {
        power.activo = true;
        power.tipo   = rand() % 4;
        power.rect   = { (float)(juegoX + rand() % (juegoAncho - 20)),
                         (float)(juegoY + 200), 20.0f, 20.0f };
        ultimoPower  = GetTime();
    }

    if (power.activo)
        power.rect.y += 2.0f;

    if (barraGrandeActiva &&
        GetTime() - tiempoBarraGrande > intTiempoBarraGrande)
    {
        barra.width       = 140.0f;
        barraGrandeActiva = false;
    }

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
// FUNCION: cargarNivel
// Nivel 1: todos normales (vida 1)
// Nivel 2: mitad resistentes (vida 2)
// Nivel 3: todos resistentes (vida 2)
// ===========================

void cargarNivel(
    Bloque bloques[filas][columnas],
    Color  colores[],
    int    nivel)
{
    const float intAnchoBloques = 160.0f;
    const float intAltoBloques  = 50.0f;
    const float espacioH        = 10.0f;
    const float espacioV        = 10.0f;

    // Centrar los bloques horizontalmente en el area de juego
    float totalAncho = columnas * intAnchoBloques + (columnas - 1) * espacioH;
    float margenIzq  = juegoX + (juegoAncho - totalAncho) / 2.0f;
    float margenTop  = juegoY + 60.0f;

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            bloques[i][j].rect = {
                margenIzq + j * (intAnchoBloques + espacioH),
                margenTop + i * (intAltoBloques  + espacioV),
                intAnchoBloques,
                intAltoBloques
            };
            bloques[i][j].activo = true;
            bloques[i][j].color  = colores[rand() % 6];

            // Nivel 1: todos vida 1
            // Nivel 2: filas pares resistentes (vida 2)
            // Nivel 3: todos resistentes (vida 2)
            if (nivel == 1)
                bloques[i][j].vida = 1;
            else if (nivel == 2)
                bloques[i][j].vida = (i % 2 == 0) ? 2 : 1;
            else
                bloques[i][j].vida = 2;
        }
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
    float&    velocidadActual,
    bool      romperBloques)
{
    for (int p = 0; p < maxPelotas; p++)
    {
        if (!pelotas[p].activa)
            continue;

        pelotas[p].pos.x += pelotas[p].vel.x;
        pelotas[p].pos.y += pelotas[p].vel.y;

        if (pelotas[p].pos.x < juegoX + pelotas[p].radio ||
            pelotas[p].pos.x > juegoX + juegoAncho - pelotas[p].radio)
            pelotas[p].vel.x *= -1;

        if (pelotas[p].pos.y < juegoY + pelotas[p].radio)
            pelotas[p].vel.y *= -1;

        if (pelotas[p].pos.y > juegoY + juegoAlto)
        {
            pelotas[p].activa = false;
            continue;
        }

        if (CheckCollisionCircleRec(pelotas[p].pos, pelotas[p].radio, barra))
        {
            pelotas[p].vel.y *= -1;
            pelotas[p].pos.y  = barra.y - pelotas[p].radio;
        }

        if (romperBloques)
        {
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
                        pelotas[p].vel.y *= -1;

                        bloques[i][j].vida--;

                        if (bloques[i][j].vida <= 0)
                        {
                            bloques[i][j].activo = false;
                            puntos++;
                            bloquesDestruidos++;

                            // Aumentar velocidad cada 15 bloques destruidos
                            const int intPelotasDestruidos = 15;
                            if (bloquesDestruidos > 0 &&
                                bloquesDestruidos % intPelotasDestruidos == 0)
                            {
                                pelotas[p].vel.x *= 1.8f;
                                pelotas[p].vel.y *= 1.8f;
                                velocidadActual   = pelotas[p].vel.x;
                            }
                        }

                        // Efecto bomba: destruye adyacentes ignorando vida
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
        else
        {
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
                        pelotas[p].vel.y *= -1;
                        break;
                    }
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
    float&     velocidadActual,
    bool&      sonidoGanadoReproducido,
    bool&      sonidoPerdidoReproducido,
    int&       nivel)
{
    nivel           = 1;
    velocidadActual = velocidadPorNivel[0];
    barra           = { (float)(juegoX + juegoAncho / 2 - 70),
                        (float)(juegoY + juegoAlto - 150), 140.0f, 20.0f };

    for (int i = 0; i < maxPelotas; i++)
        pelotas[i].activa = false;

    pelotas[0] = {
        { barra.x + barra.width / 2, barra.y - 10 },
        { 0.0f, 0.0f },
        8.0f, true, false
    };

    esperandoLanzar = true;

    cargarNivel(bloques, colores, nivel);

    puntos            = 0;
    vidas             = 3;
    bloquesDestruidos = 0;
    ganar             = false;
    perder            = false;
    power.activo      = false;
    barraGrandeActiva = false;

    sonidoGanadoReproducido  = false;
    sonidoPerdidoReproducido = false;
}

// ===========================
// FUNCION: subirNivel
// Se llama cuando se destruyen todos los bloques
// ===========================

void subirNivel(
    Rectangle& barra,
    Pelota     pelotas[],
    int        maxPelotas,
    Bloque     bloques[filas][columnas],
    Color      colores[],
    int&       bloquesDestruidos,
    bool&      esperandoLanzar,
    PowerUp&   power,
    bool&      barraGrandeActiva,
    float&     velocidadActual,
    int&       nivel)
{
    nivel++;
    bloquesDestruidos = 0;

    // Velocidad base del nuevo nivel
    velocidadActual = velocidadPorNivel[nivel - 1];

    // Resetear pelota
    for (int i = 0; i < maxPelotas; i++)
        pelotas[i].activa = false;

    pelotas[0] = {
        { barra.x + barra.width / 2, barra.y - 10 },
        { 0.0f, 0.0f },
        8.0f, true, false
    };

    esperandoLanzar   = true;
    power.activo      = false;
    barraGrandeActiva = false;
    barra.width       = 140.0f;

    cargarNivel(bloques, colores, nivel);
}

// ===========================
// FUNCION PRINCIPAL
// ===========================

void iniciarRompeBloque()
{
    InitAudioDevice();
    SetTargetFPS(60);
    srand(time(NULL));

    Color colores[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };

    int nivel = 1;

    Rectangle barra = {
        (float)(juegoX + juegoAncho / 2 - 70),
        (float)(juegoY + juegoAlto - 150),
        140.0f,
        20.0f
    };

    Pelota pelotas[MAX_PELOTAS];
    for (int i = 0; i < MAX_PELOTAS; i++)
        pelotas[i].activa = false;

    pelotas[0] = {
        { barra.x + barra.width / 2, barra.y - 10 },
        { 0.0f, 0.0f },
        8.0f, true, false
    };

    bool  esperandoLanzar = true;
    float velocidadActual = velocidadPorNivel[0];
    bool  romperBloques   = true;

    Bloque bloques[filas][columnas];
    cargarNivel(bloques, colores, nivel);

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

    Sound sonidoGanador  = LoadSound("../assets/Sonidodeganador.mp3");
    Sound sonidoPerdedor = LoadSound("../assets/JIJIJIJA.mp3");

    bool sonidoGanadoReproducido  = false;
    bool sonidoPerdidoReproducido = false;

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

                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
                {
                    pelotas[0].vel  = { -velocidadActual, -velocidadActual };
                    esperandoLanzar = false;
                }
                else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                {
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
                             velocidadActual, romperBloques);

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
                    pelotas[0] = {
                        { barra.x + barra.width / 2, barra.y - 10 },
                        { 0.0f, 0.0f },
                        8.0f, true, false
                    };
                    esperandoLanzar = true;
                }
            }

            // Verificar si se destruyeron todos los bloques del nivel
            if (bloquesDestruidos >= intBloquesVictoria)
            {
                if (nivel >= MAX_NIVELES)
                {
                    ganar = true;   // gano todos los niveles
                }
                else
                {
                    subirNivel(
                        barra, pelotas, MAX_PELOTAS, bloques, colores,
                        bloquesDestruidos, esperandoLanzar,
                        power, barraGrandeActiva, velocidadActual, nivel);
                }
            }
        }

        // =========================
        // DIBUJO
        // =========================

        BeginDrawing();
        ClearBackground(BLACK);

        dibujarAreaJuego();
        dibujarBarra(barra);
        dibujarPelotas(pelotas, MAX_PELOTAS);
        dibujarBloques(bloques);
        dibujarPowerUp(power);
        dibujarHUD(puntos, vidas, nivel);

        // PANTALLA GANAR
        if (ganar)
        {
            if (!sonidoGanadoReproducido)
            {
                PlaySound(sonidoGanador);
                sonidoGanadoReproducido = true;
            }

            DrawText("GANASTE!",
                juegoX + juegoAncho / 2 - 130,
                juegoY + juegoAlto  / 2 - 50,
                60, GREEN);

            if (botonReiniciar(
                    juegoX + juegoAncho / 2 - 110,
                    juegoY + juegoAlto  / 2 + 30,
                    220, 60, "Volver a jugar"))
                reiniciarJuego(
                    barra, pelotas, MAX_PELOTAS, bloques, colores,
                    puntos, vidas, bloquesDestruidos,
                    ganar, perder, esperandoLanzar,
                    power, barraGrandeActiva, velocidadActual,
                    sonidoGanadoReproducido, sonidoPerdidoReproducido,
                    nivel);
        }

        // PANTALLA PERDER
        if (perder)
        {
            if (!sonidoPerdidoReproducido)
            {
                PlaySound(sonidoPerdedor);
                sonidoPerdidoReproducido = true;
            }

            DrawText("GAME OVER",
                juegoX + juegoAncho / 2 - 150,
                juegoY + juegoAlto  / 2 - 50,
                60, RED);

            if (botonReiniciar(
                    juegoX + juegoAncho / 2 - 110,
                    juegoY + juegoAlto  / 2 + 30,
                    220, 60, "Volver a jugar"))
                reiniciarJuego(
                    barra, pelotas, MAX_PELOTAS, bloques, colores,
                    puntos, vidas, bloquesDestruidos,
                    ganar, perder, esperandoLanzar,
                    power, barraGrandeActiva, velocidadActual,
                    sonidoGanadoReproducido, sonidoPerdidoReproducido,
                    nivel);
        }

        EndDrawing();
    }

    UnloadSound(sonidoGanador);
    UnloadSound(sonidoPerdedor);
    CloseAudioDevice();
}