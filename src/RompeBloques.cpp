#include "RompeBloques.h"
#include "raylib.h"
#include "../Api´s/ApiClient.h"
#include "../Api´s/GameApiConfig.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <string>
#include <thread>

const int intTiempoPower       = 7;
const int intTiempoBarraGrande = 4;
const int filas                = 4;
const int columnas             = 6;
const int MAX_NIVELES          = 3;

const int ancho                = 1920;
const int alto                 = 1095;

const int juegoX               = 40;
const int juegoY               = 40;
const int juegoAncho           = 1400;
const int juegoAlto            = 1015;

const int hudX                 = juegoX + juegoAncho + 40;
const int hudAncho             = ancho - hudX - 20;

const int intBloquesVictoria   = filas * columnas;
const int MAX_PELOTAS          = 10;

const float velocidadPorNivel[MAX_NIVELES] = { 4.0f, 5.8f, 7.5f };

// ===========================
// STRUCTS
// ===========================

struct Bloque
{
    Rectangle rect;
    bool      activo;
    Color     color;
    int       vida;
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
// FUNCION: calcularTokensPorNivel
// ===========================

int calcularTokensPorNivel(int nivelCompletado)
{
    if (nivelCompletado >= MAX_NIVELES)
        return GameApiConfig::PREMIO_ALTO;

    // MODIFICADO: Ahora desde el nivel 1 completado otorga PREMIO_BAJO
    if (nivelCompletado >= 1)
        return GameApiConfig::PREMIO_BAJO;

    return 0;
}

// ===========================
// FUNCION: iniciarNuevaPartida
// ===========================

bool iniciarNuevaPartida(ApiClient& api, PartidaApi& partidaActual)
{
    std::string error;
    bool ok = api.iniciarPartida(
        partidaActual,
        error,
        GameApiConfig::VERSION_JUEGO,
        GameApiConfig::COSTO_PARTIDA
    );
    return ok;
}

// ===========================
// FUNCION: reportarScoreSiCorresponde
// ===========================

void reportarScoreSiCorresponde(
    ApiClient&        api,
    const PartidaApi& partidaActual,
    int               scoreActual,
    int               nivelActual,
    int&              ultimoScoreReportado)
{
    if (scoreActual - ultimoScoreReportado < GameApiConfig::REPORTAR_CADA_PUNTOS)
        return;

    std::string error;
    bool ok = api.reportarScore(
        partidaActual.idPartida,
        scoreActual,
        nivelActual,
        error
    );

    if (ok)
        ultimoScoreReportado = scoreActual;
}

// ===========================
// FUNCION: finalizarEnHilo
// ===========================

void finalizarEnHilo(
    ApiClient* api,
    PartidaApi  partida,
    int         score,
    int         nivel,
    std::string resultado,
    int         duracion,
    int         tokens)
{
    std::thread([api, partida, score, nivel, resultado, duracion, tokens]() {
        std::string error;
        api->finalizarPartida(
            partida.idPartida,
            score, nivel,
            resultado,
            duracion,
            tokens,
            error
        );
    }).detach();
}

// ===========================
// FUNCION: botonReiniciar
// ===========================

bool botonReiniciar(int x, int y, int ancho, int alto, const char* texto)
{
    Rectangle boton = { (float)x, (float)y, (float)ancho, (float)alto };
    Vector2 mouse   = GetMousePosition();
    bool hover      = CheckCollisionPointRec(mouse, boton);

    if (hover) DrawRectangleRec(boton, DARKGRAY);
    else       DrawRectangleRec(boton, GRAY);

    DrawRectangleLinesEx(boton, 2, WHITE);

    int textoAncho = MeasureText(texto, 20);
    DrawText(texto, x + (ancho - textoAncho) / 2, y + 18, 20, WHITE);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ===========================
// FUNCION: dibujarPantallaFinal
// ===========================

int dibujarPantallaFinal(bool gano, int tokensGanadosPartida)
{
    int resultado = 0;

    int cx = juegoX + juegoAncho / 2;
    int cy = juegoY + juegoAlto  / 2;

    DrawRectangle(juegoX, juegoY, juegoAncho, juegoAlto, { 0, 0, 0, 180 });

    int cuadroAncho = 500;
    int cuadroAlto  = 340;
    int cuadroX     = cx - cuadroAncho / 2;
    int cuadroY     = cy - cuadroAlto  / 2;

    DrawRectangleRounded(
        { (float)cuadroX, (float)cuadroY, (float)cuadroAncho, (float)cuadroAlto },
        0.15f, 8, { 20, 20, 20, 230 });

    Color colorBorde = gano ? GREEN : RED;
    DrawRectangleRoundedLinesEx(
        { (float)cuadroX, (float)cuadroY, (float)cuadroAncho, (float)cuadroAlto },
        0.15f, 8, 3.0f, colorBorde);

    const char* titulo = gano ? "GANASTE!" : "GAME OVER";
    Color colorTitulo  = gano ? GREEN : RED;
    int   tamTitulo    = 60;
    int   anchoTitulo  = MeasureText(titulo, tamTitulo);
    DrawText(titulo, cx - anchoTitulo / 2, cuadroY + 30, tamTitulo, colorTitulo);

    DrawLine(cuadroX + 30, cuadroY + 110,
             cuadroX + cuadroAncho - 30, cuadroY + 110,
             { 80, 80, 80, 255 });

    std::string txtTokens = "Tokens ganados: " + std::to_string(tokensGanadosPartida);
    int anchoTokens = MeasureText(txtTokens.c_str(), 22);
    DrawText(txtTokens.c_str(),
             cx - anchoTokens / 2,
             cuadroY + 125, 22, YELLOW);

    DrawLine(cuadroX + 30, cuadroY + 165,
             cuadroX + cuadroAncho - 30, cuadroY + 165,
             { 80, 80, 80, 255 });

    Vector2 mouse = GetMousePosition();

    int btnAncho = 200;
    int btnAlto  = 50;
    int btn1X    = cx - btnAncho - 20;
    int btn1Y    = cuadroY + 185;

    Rectangle boton1 = { (float)btn1X, (float)btn1Y, (float)btnAncho, (float)btnAlto };
    bool hover1      = CheckCollisionPointRec(mouse, boton1);
    Color colorBtn1  = hover1 ? GREEN : Color{ 0, 150, 0, 255 };
    DrawRectangleRounded(boton1, 0.3f, 6, colorBtn1);
    DrawRectangleRoundedLinesEx(boton1, 0.3f, 6, 2.0f, WHITE);
    const char* txtBtn1 = "Volver a jugar";
    int anchoBtn1       = MeasureText(txtBtn1, 18);
    DrawText(txtBtn1, btn1X + (btnAncho - anchoBtn1) / 2, btn1Y + 16, 18, WHITE);
    if (hover1 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) resultado = 1;

    int btn2X = cx + 20;
    int btn2Y = cuadroY + 185;

    Rectangle boton2 = { (float)btn2X, (float)btn2Y, (float)btnAncho, (float)btnAlto };
    bool hover2      = CheckCollisionPointRec(mouse, boton2);
    Color colorBtn2  = hover2 ? SKYBLUE : Color{ 0, 100, 180, 255 };
    DrawRectangleRounded(boton2, 0.3f, 6, colorBtn2);
    DrawRectangleRoundedLinesEx(boton2, 0.3f, 6, 2.0f, WHITE);
    const char* txtBtn2 = "Volver al Menu";
    int anchoBtn2       = MeasureText(txtBtn2, 18);
    DrawText(txtBtn2, btn2X + (btnAncho - anchoBtn2) / 2, btn2Y + 16, 18, WHITE);
    if (hover2 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) resultado = 2;

    return resultado;
}

// ===========================
// FUNCION: dibujarSinTokens
// ===========================

int dibujarSinTokens(int tokensGanadosPartida)
{
    int resultado = 0;

    int cx = juegoX + juegoAncho / 2;
    int cy = juegoY + juegoAlto  / 2;

    DrawRectangle(juegoX, juegoY, juegoAncho, juegoAlto, { 0, 0, 0, 200 });

    int cuadroAncho = 520;
    int cuadroAlto  = 280;
    int cuadroX     = cx - cuadroAncho / 2;
    int cuadroY     = cy - cuadroAlto  / 2;

    DrawRectangleRounded(
        { (float)cuadroX, (float)cuadroY, (float)cuadroAncho, (float)cuadroAlto },
        0.15f, 8, { 20, 20, 20, 240 });

    DrawRectangleRoundedLinesEx(
        { (float)cuadroX, (float)cuadroY, (float)cuadroAncho, (float)cuadroAlto },
        0.15f, 8, 3.0f, ORANGE);

    const char* titulo = "SIN TOKENS";
    int anchoTitulo    = MeasureText(titulo, 50);
    DrawText(titulo, cx - anchoTitulo / 2, cuadroY + 30, 50, ORANGE);

    const char* msg = "No tienes tokens suficientes para jugar.";
    int anchoMsg    = MeasureText(msg, 18);
    DrawText(msg, cx - anchoMsg / 2, cuadroY + 100, 18, LIGHTGRAY);

    std::string txtGanados = "Tokens ganados esta partida: " + std::to_string(tokensGanadosPartida);
    int anchoGanados       = MeasureText(txtGanados.c_str(), 20);
    DrawText(txtGanados.c_str(), cx - anchoGanados / 2, cuadroY + 135, 20, YELLOW);

    std::string txtCosto = "Costo por partida: " + std::to_string(GameApiConfig::COSTO_PARTIDA);
    int anchoCosto       = MeasureText(txtCosto.c_str(), 20);
    DrawText(txtCosto.c_str(), cx - anchoCosto / 2, cuadroY + 165, 20, RED);

    int btnAncho = 220;
    int btnAlto  = 50;
    int btnX     = cx - btnAncho / 2;
    int btnY     = cuadroY + 205;

    Rectangle boton = { (float)btnX, (float)btnY, (float)btnAncho, (float)btnAlto };
    Vector2 mouse   = GetMousePosition();
    bool hover      = CheckCollisionPointRec(mouse, boton);

    Color colorBtn = hover ? SKYBLUE : Color{ 0, 100, 180, 255 };
    DrawRectangleRounded(boton, 0.3f, 6, colorBtn);
    DrawRectangleRoundedLinesEx(boton, 0.3f, 6, 2.0f, WHITE);

    const char* txtBtn = "Volver al Menu";
    int anchoBtn       = MeasureText(txtBtn, 18);
    DrawText(txtBtn, btnX + (btnAncho - anchoBtn) / 2, btnY + 16, 18, WHITE);

    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) resultado = 2;

    return resultado;
}

// ===========================
// FUNCION: moverBarra
// ===========================

void moverBarra(Rectangle& barra)
{
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        barra.x -= 10.0f;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        barra.x += 10.0f;

    if (barra.x < juegoX)
        barra.x = juegoX;

    if (barra.x + barra.width > juegoX + juegoAncho)
        barra.x = juegoX + juegoAncho - barra.width;
}

void dibujarAreaJuego()
{
    DrawRectangleLinesEx({ 10, 10, (float)(ancho - 20), (float)(alto - 20) }, 3, DARKGRAY);
    DrawRectangleLinesEx(
        { (float)(juegoX - 5), (float)(juegoY - 5),
          (float)(juegoAncho + 10), (float)(juegoAlto + 10) },
        3, DARKGRAY);
}

void dibujarBarra(Rectangle barra)
{
    DrawRectangleRec(barra, WHITE);
}

void dibujarPelotas(Pelota pelotas[], int maxPelotas)
{
    for (int p = 0; p < maxPelotas; p++)
        if (pelotas[p].activa)
        {
            Color color = pelotas[p].bomba ? RED : SKYBLUE;
            DrawCircleV(pelotas[p].pos, pelotas[p].radio, color);
        }
}

void dibujarBloques(Bloque bloques[filas][columnas])
{
    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
        {
            if (!bloques[i][j].activo) continue;

            Rectangle r = bloques[i][j].rect;
            Color c     = bloques[i][j].color;
            Color borde = { (unsigned char)(c.r / 2),
                            (unsigned char)(c.g / 2),
                            (unsigned char)(c.b / 2), 255 };

            DrawRectangleRounded(r, 0.3f, 6, c);
            DrawRectangleRoundedLinesEx(r, 0.3f, 6, 3.0f, borde);

            if (bloques[i][j].vida == 1)
            {
                int cx = (int)(r.x + r.width  / 2);
                int cy = (int)(r.y + r.height / 2);
                DrawLine(cx - 8, cy - 6, cx + 4, cy + 8,  BLACK);
                DrawLine(cx + 4, cy + 8, cx - 2, cy + 14, BLACK);
                DrawLine(cx - 8, cy - 6, cx - 2, cy - 14, BLACK);
            }
        }
}

void dibujarPowerUp(PowerUp power)
{
    if (!power.activo) return;

    Color c = YELLOW;
    if (power.tipo == 1) c = GREEN;
    if (power.tipo == 2) c = RED;
    if (power.tipo == 3) c = BLUE;

    DrawRectangleRec(power.rect, c);
}

void dibujarHUD(int puntos, int vidas, int nivel, int tokensGanadosPartida)
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

    DrawText("TOKENS GANADOS", hudX + 20, juegoY + 560, 22, LIGHTGRAY);
    DrawText(TextFormat("%i", tokensGanadosPartida), hudX + 20, juegoY + 600, 40, YELLOW);
}

// ===========================
// POWER UPS
// ===========================

void aplicarPowerBolaExtra(Pelota pelotas[], int maxPelotas, Rectangle barra, float velocidadActual)
{
    for (int i = 0; i < maxPelotas; i++)
        if (!pelotas[i].activa)
        {
            pelotas[i] = {{ barra.x + barra.width / 2, barra.y - 10 },
                          { -velocidadActual, -velocidadActual }, 8.0f, true, false};
            break;
        }
}

void aplicarPowerDobleBola(Pelota pelotas[], int maxPelotas, Rectangle barra, float velocidadActual)
{
    for (int k = 0; k < 2; k++)
        for (int i = 0; i < maxPelotas; i++)
            if (!pelotas[i].activa)
            {
                float vx = (rand() % 2 == 0) ? velocidadActual : -velocidadActual;
                pelotas[i] = {{ barra.x + barra.width / 2, barra.y - 10 },
                               { vx, -velocidadActual }, 8.0f, true, false};
                break;
            }
}

void aplicarPowerBomba(Pelota pelotas[]) { pelotas[0].bomba = true; }

void aplicarPowerBarraGrande(Rectangle& barra, bool& barraGrandeActiva, double& tiempoBarraGrande)
{
    barra.width       = 220.0f;
    barraGrandeActiva = true;
    tiempoBarraGrande = GetTime();
}

void actualizarPowerUp(
    PowerUp& power, Rectangle& barra, Pelota pelotas[], int maxPelotas,
    bool& barraGrandeActiva, double& tiempoBarraGrande, double& ultimoPower,
    float velocidadActual)
{
    if (GetTime() - ultimoPower > intTiempoPower)
    {
        power.activo = true;
        power.tipo   = rand() % 4;
        power.rect   = { (float)(juegoX + rand() % (juegoAncho - 20)),
                         (float)(juegoY + 200), 20.0f, 20.0f };
        ultimoPower  = GetTime();
    }

    if (power.activo) power.rect.y += 2.0f;

    if (barraGrandeActiva && GetTime() - tiempoBarraGrande > intTiempoBarraGrande)
    {
        barra.width       = 140.0f;
        barraGrandeActiva = false;
    }

    if (power.activo && CheckCollisionRecs(power.rect, barra))
    {
        if (power.tipo == 0) aplicarPowerBolaExtra(pelotas, maxPelotas, barra, velocidadActual);
        if (power.tipo == 1) aplicarPowerDobleBola(pelotas, maxPelotas, barra, velocidadActual);
        if (power.tipo == 2) aplicarPowerBomba(pelotas);
        if (power.tipo == 3) aplicarPowerBarraGrande(barra, barraGrandeActiva, tiempoBarraGrande);
        power.activo = false;
    }
}

// ===========================
// FUNCION: cargarNivel
// ===========================

void cargarNivel(Bloque bloques[filas][columnas], Color colores[], int nivel)
{
    const float intAnchoBloques = 160.0f;
    const float intAltoBloques  = 80.0f;
    const float espacioH        = 10.0f;
    const float espacioV        = 10.0f;

    float totalAncho = columnas * intAnchoBloques + (columnas - 1) * espacioH;
    float margenIzq  = juegoX + (juegoAncho - totalAncho) / 2.0f;
    float margenTop  = juegoY + 60.0f;

    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
        {
            bloques[i][j].rect = {
                margenIzq + j * (intAnchoBloques + espacioH),
                margenTop + i * (intAltoBloques  + espacioV),
                intAnchoBloques, intAltoBloques
            };
            bloques[i][j].activo = true;
            bloques[i][j].color  = colores[rand() % 6];

            if (nivel == 1)      bloques[i][j].vida = 1;
            else if (nivel == 2) bloques[i][j].vida = (i % 2 == 0) ? 2 : 1;
            else                 bloques[i][j].vida = 2;
        }
}

// ===========================
// FUNCION: moverPelotas
// ===========================

void moverPelotas(
    Pelota pelotas[], int maxPelotas, Rectangle barra,
    Bloque bloques[filas][columnas],
    int& puntos, int& bloquesDestruidos,
    float& velocidadActual, bool romperBloques)
{
    for (int p = 0; p < maxPelotas; p++)
    {
        if (!pelotas[p].activa) continue;

        // CORREGIDO: Ahora usa 'pelotas[p]' correctamente
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
                for (int j = 0; j < columnas; j++)
                {
                    if (!bloques[i][j].activo) continue;

                    if (CheckCollisionCircleRec(
                            pelotas[p].pos, pelotas[p].radio, bloques[i][j].rect))
                    {
                        pelotas[p].vel.y *= -1;
                        bloques[i][j].vida--;

                        if (bloques[i][j].vida <= 0)
                        {
                            bloques[i][j].activo = false;
                            puntos++;
                            bloquesDestruidos++;

                            const int intPelotasDestruidos = 15;
                            if (bloquesDestruidos > 0 &&
                                bloquesDestruidos % intPelotasDestruidos == 0)
                            {
                                pelotas[p].vel.x *= 1.5f;
                                pelotas[p].vel.y *= 1.5f;
                                velocidadActual   = pelotas[p].vel.x;
                            }

                            if (pelotas[p].bomba)
                            {
                                for (int a = -1; a <= 1; a++)
                                    for (int b = -1; b <= 1; b++)
                                    {
                                        int ni = i + a, nj = j + b;
                                        if (ni >= 0 && ni < filas &&
                                            nj >= 0 && nj < columnas)
                                            if (bloques[ni][nj].activo)
                                            {
                                                bloques[ni][nj].activo = false;
                                                puntos++;
                                                bloquesDestruidos++;
                                            }
                                    }
                                pelotas[p].bomba = false;
                            }
                        }
                        break;
                    }
                }
        }
        else
        {
            for (int i = 0; i < filas; i++)
                for (int j = 0; j < columnas; j++)
                {
                    if (!bloques[i][j].activo) continue;
                    if (CheckCollisionCircleRec(
                            pelotas[p].pos, pelotas[p].radio, bloques[i][j].rect))
                    {
                        pelotas[p].vel.y *= -1;
                        break;
                    }
                }
        }
    }
}

// ===========================
// FUNCION: resetearPelotaYBarra
// ===========================

void resetearPelotaYBarra(Rectangle& barra, Pelota pelotas[], int maxPelotas,
                           bool& esperandoLanzar)
{
    for (int i = 0; i < maxPelotas; i++)
        pelotas[i].activa = false;

    pelotas[0] = {
        { barra.x + barra.width / 2, barra.y - 10 },
        { 0.0f, 0.0f }, 8.0f, true, false
    };
    esperandoLanzar = true;
}

// ===========================
// FUNCION: subirNivel
// ===========================

void subirNivel(
    Rectangle& barra, Pelota pelotas[], int maxPelotas,
    Bloque bloques[filas][columnas], Color colores[],
    int& bloquesDestruidos, bool& esperandoLanzar,
    PowerUp& power, bool& barraGrandeActiva,
    float& velocidadActual, int& nivel)
{
    nivel++;
    bloquesDestruidos = 0;
    velocidadActual   = velocidadPorNivel[nivel - 1];

    resetearPelotaYBarra(barra, pelotas, maxPelotas, esperandoLanzar);

    power.activo      = false;
    barraGrandeActiva = false;
    barra.width       = 140.0f;

    cargarNivel(bloques, colores, nivel);
}

// ===========================
// FUNCION: reiniciarJuego
// ===========================

void reiniciarJuego(
    Rectangle& barra, Pelota pelotas[], int maxPelotas,
    Bloque bloques[filas][columnas], Color colores[],
    int& puntos, int& vidas, int& bloquesDestruidos,
    bool& ganar, bool& perder, bool& esperandoLanzar,
    PowerUp& power, bool& barraGrandeActiva,
    float& velocidadActual,
    bool& sonidoGanadoReproducido, bool& sonidoPerdidoReproducido,
    int& nivel, bool& partidaFinalizada, int& ultimoScoreReportado,
    int& tokensGanadosPartida, bool& premioBajoEntregado)
{
    nivel           = 1;
    velocidadActual = velocidadPorNivel[0];
    barra           = { (float)(juegoX + juegoAncho / 2 - 70),
                        (float)(juegoY + juegoAlto - 150), 140.0f, 20.0f };

    resetearPelotaYBarra(barra, pelotas, maxPelotas, esperandoLanzar);
    cargarNivel(bloques, colores, nivel);

    puntos                   = 0;
    vidas                    = 3;
    bloquesDestruidos        = 0;
    ganar                    = false;
    perder                   = false;
    power.activo             = false;
    barraGrandeActiva        = false;
    partidaFinalizada        = false;
    ultimoScoreReportado     = 0;
    tokensGanadosPartida     = 0;
    premioBajoEntregado      = false;
    sonidoGanadoReproducido  = false;
    sonidoPerdidoReproducido = false;
}

// ===========================
// FUNCION PRINCIPAL
// ===========================

bool iniciarRompeBloque(ApiClient& api)
{
    InitAudioDevice();
    SetTargetFPS(60);
    srand(time(NULL));

    Color colores[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
    int nivel = 1;

    Rectangle barra = {
        (float)(juegoX + juegoAncho / 2 - 70),
        (float)(juegoY + juegoAlto - 150),
        140.0f, 20.0f
    };

    Pelota pelotas[MAX_PELOTAS];
    for (int i = 0; i < MAX_PELOTAS; i++) pelotas[i].activa = false;

    pelotas[0] = {{ barra.x + barra.width / 2, barra.y - 10 },
                  { 0.0f, 0.0f }, 8.0f, true, false};

    bool  esperandoLanzar = true;
    float velocidadActual = velocidadPorNivel[0];
    bool  romperBloques   = true;

    Bloque bloques[filas][columnas];
    cargarNivel(bloques, colores, nivel);

    int puntos = 0, vidas = 3, bloquesDestruidos = 0;
    bool ganar = false, perder = false;

    PowerUp power      = {};
    double ultimoPower = GetTime();
    bool   barraGrandeActiva = false;
    double tiempoBarraGrande = 0;

    Sound sonidoGanador  = LoadSound("../assets/Sonidodeganador.mp3");
    Sound sonidoPerdedor = LoadSound("../assets/JIJIJIJA.mp3");
    bool sonidoGanadoReproducido  = false;
    bool sonidoPerdidoReproducido = false;

    // API
    PartidaApi partidaActual;
    bool partidaIniciada      = false;
    bool partidaFinalizada    = false;
    bool sinTokens            = false;
    int  ultimoScoreReportado = 0;
    int  tokensGanadosPartida = 0;

    bool premioBajoEntregado = false;

    std::chrono::steady_clock::time_point inicioPartida;

    bool volverAlMenu = false;

    while (!WindowShouldClose())
    {
        // =========================
        // LOGICA
        // =========================

        if (!ganar && !perder && !sinTokens)
        {
            moverBarra(barra);

            if (esperandoLanzar)
            {
                pelotas[0].pos.x = barra.x + barra.width / 2;
                pelotas[0].pos.y = barra.y - pelotas[0].radio;

                bool  lanzar = false;
                float dirX   = velocidadActual;

                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
                    { lanzar = true; dirX = -velocidadActual; }
                else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                    { lanzar = true; dirX =  velocidadActual; }
                else if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE))
                    { lanzar = true; dirX = (rand() % 2 == 0) ? velocidadActual : -velocidadActual; }

                if (lanzar)
                {
                    pelotas[0].vel  = { dirX, -velocidadActual };
                    esperandoLanzar = false;

                    if (!partidaIniciada)
                    {
                        inicioPartida = std::chrono::steady_clock::now();
                        bool ok       = iniciarNuevaPartida(api, partidaActual);

                        if (!ok)
                        {
                            sinTokens       = true;
                            esperandoLanzar = true;
                            resetearPelotaYBarra(barra, pelotas, MAX_PELOTAS, esperandoLanzar);
                        }
                        else
                        {
                            partidaIniciada      = true;
                            tokensGanadosPartida = 0;
                            premioBajoEntregado  = false;
                        }
                    }
                }
            }

            actualizarPowerUp(power, barra, pelotas, MAX_PELOTAS,
                              barraGrandeActiva, tiempoBarraGrande, ultimoPower,
                              velocidadActual);

            if (!esperandoLanzar)
                moverPelotas(pelotas, MAX_PELOTAS, barra, bloques,
                             puntos, bloquesDestruidos, velocidadActual, romperBloques);

            if (partidaIniciada)
                reportarScoreSiCorresponde(api, partidaActual,
                                           puntos, nivel, ultimoScoreReportado);

            bool algunaPelotaActiva = false;
            for (int i = 0; i < MAX_PELOTAS; i++)
                if (pelotas[i].activa) { algunaPelotaActiva = true; break; }

            if (!algunaPelotaActiva)
            {
                vidas--;
                if (vidas <= 0)
                    perder = true;
                else
                    resetearPelotaYBarra(barra, pelotas, MAX_PELOTAS, esperandoLanzar);
            }

            // Verificar nivel completado
            if (bloquesDestruidos >= intBloquesVictoria)
            {
                if (nivel >= MAX_NIVELES)
                {
                    if (partidaIniciada && !ganar)
                    {
                        tokensGanadosPartida += GameApiConfig::PREMIO_ALTO;
                    }
                    ganar = true;
                }
                else
                {
                    // MODIFICADO: Suma PREMIO_BAJO tanto al completar Nivel 1 como Nivel 2
                    if (partidaIniciada)
                    {
                        tokensGanadosPartida += GameApiConfig::PREMIO_BAJO;
                        premioBajoEntregado   = true;
                    }

                    subirNivel(barra, pelotas, MAX_PELOTAS, bloques, colores,
                               bloquesDestruidos, esperandoLanzar,
                               power, barraGrandeActiva, velocidadActual, nivel);
                }
            }

            // Perder: finalizar en hilo
            if (perder && partidaIniciada && !partidaFinalizada)
            {
                auto ahora   = std::chrono::steady_clock::now();
                int duracion = (int)std::chrono::duration_cast<
                    std::chrono::seconds>(ahora - inicioPartida).count();

                // MODIFICADO: Enviamos tokensGanadosPartida para conservar lo acumulado al perder
                finalizarEnHilo(&api, partidaActual,
                                puntos, nivel, "LOSE", duracion, tokensGanadosPartida);
                partidaFinalizada = true;
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
        dibujarHUD(puntos, vidas, nivel, tokensGanadosPartida);

        if (sinTokens)
        {
            int accion = dibujarSinTokens(tokensGanadosPartida);
            if (accion == 2)
            {
                volverAlMenu = true;
                break;
            }
        }

        if (ganar)
        {
            if (partidaIniciada && !partidaFinalizada)
            {
                auto ahora   = std::chrono::steady_clock::now();
                int duracion = (int)std::chrono::duration_cast<
                    std::chrono::seconds>(ahora - inicioPartida).count();

                finalizarEnHilo(&api, partidaActual,
                                puntos, nivel, "WIN", duracion, tokensGanadosPartida);
                partidaFinalizada = true;
            }

            if (!sonidoGanadoReproducido)
            {
                PlaySound(sonidoGanador);
                sonidoGanadoReproducido = true;
            }

            int accion = dibujarPantallaFinal(true, tokensGanadosPartida);

            if (accion == 1)
            {
                reiniciarJuego(barra, pelotas, MAX_PELOTAS, bloques, colores,
                               puntos, vidas, bloquesDestruidos,
                               ganar, perder, esperandoLanzar,
                               power, barraGrandeActiva, velocidadActual,
                               sonidoGanadoReproducido, sonidoPerdidoReproducido,
                               nivel, partidaFinalizada, ultimoScoreReportado,
                               tokensGanadosPartida, premioBajoEntregado);
                partidaIniciada = false;
                sinTokens       = false;
            }
            else if (accion == 2)
            {
                volverAlMenu = true;
                break;
            }
        }

        if (perder)
        {
            if (!sonidoPerdidoReproducido)
            {
                PlaySound(sonidoPerdedor);
                sonidoPerdidoReproducido = true;
            }

            int accion = dibujarPantallaFinal(false, tokensGanadosPartida);

            if (accion == 1)
            {
                reiniciarJuego(barra, pelotas, MAX_PELOTAS, bloques, colores,
                               puntos, vidas, bloquesDestruidos,
                               ganar, perder, esperandoLanzar,
                               power, barraGrandeActiva, velocidadActual,
                               sonidoGanadoReproducido, sonidoPerdidoReproducido,
                               nivel, partidaFinalizada, ultimoScoreReportado,
                               tokensGanadosPartida, premioBajoEntregado);
                partidaIniciada = false;
                sinTokens       = false;
            }
            else if (accion == 2)
            {
                volverAlMenu = true;
                break;
            }
        }

        EndDrawing();
    }

    UnloadSound(sonidoGanador);
    UnloadSound(sonidoPerdedor);
    CloseAudioDevice();

    return volverAlMenu;
}