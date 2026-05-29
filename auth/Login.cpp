#include "Login.h"
#include <iostream>
#include "../src/Menu.h"
#include "raylib.h"

Login::Login(ApiClient& apiClient)
    : api(apiClient),
      autenticado(false) {
}

bool Login::iniciarSesion(
    const std::string& username,
    const std::string& password,
    std::string& mensajeError
) {

    if (username.empty()) {
        mensajeError = "El usuario no puede estar vacio.";
        return false;
    }

    if (password.empty()) {
        mensajeError = "La contrasena no puede estar vacia.";
        return false;
    }

    UsuarioApi usuario;

    bool loginOk = api.loginJugador(
        username,
        password,
        usuario,
        mensajeError
    );

    if (!loginOk) {
        autenticado = false;
        return false;
    }

    usuarioActual = usuario;
    autenticado = true;

    return true;
}

bool Login::estaAutenticado() const {
    return autenticado && api.tieneSesion();
}

UsuarioApi Login::getUsuarioActual() const {
    return usuarioActual;
}

void Login::cerrarSesion() {
    api.cerrarSesion();
    usuarioActual = UsuarioApi();
    autenticado = false;
}

void Login::mostrarLogin() {

    bool mostrarPassword = false;

    int screenWidth  = 1920;
    int screenHeight = 1095;

    int centroX = screenWidth / 2;

    int inputWidth  = 350;
    int inputHeight = 50;

    // INPUTS
    Rectangle inputUsuario = {
        (float)(centroX - inputWidth / 2),
        180,
        (float)inputWidth,
        (float)inputHeight
    };

    Rectangle inputPassword = {
        (float)(centroX - inputWidth / 2),
        300,
        (float)inputWidth,
        (float)inputHeight
    };

    // BOTONES
    Rectangle botonLogin = {
        (float)(centroX - 60),
        430,
        120,
        40
    };

    Rectangle botonOjo = {
        inputPassword.x + inputPassword.width + 10,
        inputPassword.y,
        40,
        50
    };

    std::string username = "";
    std::string password = "";
    std::string mensajeError = "";

    bool escribiendoUsuario = true;

    SetExitKey(0);

    while (!WindowShouldClose()) {

        Vector2 mouse = GetMousePosition();
        bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // CLICK EN INPUTS
        if (click && CheckCollisionPointRec(mouse, inputUsuario)) {
            escribiendoUsuario = true;
        }

        if (click && CheckCollisionPointRec(mouse, inputPassword)) {
            escribiendoUsuario = false;
        }

        // ESCRITURA
        int key = GetCharPressed();

        while (key > 0) {

            if (key >= 32 && key <= 125) {

                if (escribiendoUsuario) {
                    username += (char)key;
                }
                else {
                    password += (char)key;
                }
            }

            key = GetCharPressed();
        }

        // BORRAR
        if (IsKeyPressed(KEY_BACKSPACE)) {

            if (escribiendoUsuario && !username.empty()) {
                username.pop_back();
            }
            else if (!escribiendoUsuario && !password.empty()) {
                password.pop_back();
            }
        }

        // TAB
        if (IsKeyPressed(KEY_TAB)) {
            escribiendoUsuario = !escribiendoUsuario;
        }

        // MOSTRAR PASSWORD
        if (click && CheckCollisionPointRec(mouse, botonOjo)) {
            mostrarPassword = !mostrarPassword;
        }

        // LOGIN
        if (click && CheckCollisionPointRec(mouse, botonLogin)) {

            bool ok = iniciarSesion(username, password, mensajeError);

            if (ok) {
                Menu menu(api);
                menu.mostrar();
                break;
            }
        }

        BeginDrawing();

        ClearBackground(BLACK);

        // TITULO CENTRADO
        const char* titulo = "PROYECTO ARCADE";
        int tituloSize = 40;

        int tituloWidth = MeasureText(titulo, tituloSize);

        DrawText(
            titulo,
            centroX - tituloWidth / 2,
            80,
            tituloSize,
            WHITE
        );

        // USER
        DrawRectangleRec(
            inputUsuario,
            escribiendoUsuario ? GRAY : DARKGRAY
        );

        DrawText(
            username.c_str(),
            inputUsuario.x + 10,
            inputUsuario.y + 15,
            25,
            WHITE
        );

        DrawText(
            "USUARIO",
            inputUsuario.x,
            inputUsuario.y - 30,
            20,
            LIGHTGRAY
        );

        // PASSWORD
        DrawRectangleRec(
            inputPassword,
            !escribiendoUsuario ? GRAY : DARKGRAY
        );

        std::string passToShow = mostrarPassword
            ? password
            : std::string(password.length(), '*');

        DrawText(
            passToShow.c_str(),
            inputPassword.x + 10,
            inputPassword.y + 15,
            25,
            WHITE
        );

        DrawText(
            "PASSWORD",
            inputPassword.x,
            inputPassword.y - 30,
            20,
            LIGHTGRAY
        );

        // BOTON OJO
        DrawRectangleRec(botonOjo, DARKGRAY);

        DrawText(
            "O",
            botonOjo.x + 12,
            botonOjo.y + 10,
            30,
            WHITE
        );

        // BOTON LOGIN
        DrawRectangleRec(botonLogin, GREEN);

        DrawText(
            "LOGIN",
            botonLogin.x + 25,
            botonLogin.y + 10,
            20,
            BLACK
        );

        // TEXTO INFO
        const char* info = "Click o TAB para cambiar campo";

        int infoWidth = MeasureText(info, 20);

        DrawText(
            info,
            centroX - infoWidth / 2,
            390,
            20,
            LIGHTGRAY
        );

        // ERROR
        if (!mensajeError.empty()) {

            int errorWidth = MeasureText(
                mensajeError.c_str(),
                20
            );

            DrawText(
                mensajeError.c_str(),
                centroX - errorWidth / 2,
                500,
                20,
                RED
            );
        }

        EndDrawing();
    }
}