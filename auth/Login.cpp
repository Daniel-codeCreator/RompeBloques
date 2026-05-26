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

    // INPUTS
    Rectangle inputUsuario  = { 220, 180, 350, 50 };
    Rectangle inputPassword = { 220, 300, 350, 50 };

    // BOTONES
    Rectangle botonLogin = { 320, 430, 120, 40 };
    Rectangle botonOjo   = { 580, 300, 40, 50 };

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
                } else {
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
                Menu menu;
                menu.mostrar();
                break;
            }
        }

        BeginDrawing();

        ClearBackground(BLACK);

        DrawText(
            "PROYECTO ARCADE",
            250,
            80,
            40,
            WHITE
        );

        // USER
        DrawRectangleRec(
            inputUsuario,
            escribiendoUsuario ? GRAY : DARKGRAY
        );

        DrawText(
            username.c_str(),
            230,
            195,
            25,
            WHITE
        );

        DrawText(
            "USUARIO",
            220,
            150,
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
            230,
            315,
            25,
            WHITE
        );

        DrawText(
            "PASSWORD",
            220,
            270,
            20,
            LIGHTGRAY
        );

        // BOTON OJO
        DrawRectangleRec(botonOjo, DARKGRAY);

        DrawText(
            "O",
            592,
            310,
            30,
            WHITE
        );

        // BOTON LOGIN
        DrawRectangleRec(botonLogin, GREEN);

        DrawText(
            "LOGIN",
            345,
            440,
            20,
            BLACK
        );

        // TEXTO INFO
        DrawText(
            "Click o TAB para cambiar campo",
            220,
            390,
            20,
            LIGHTGRAY
        );

        // ERROR
        if (!mensajeError.empty()) {

            DrawText(
                mensajeError.c_str(),
                180,
                500,
                20,
                RED
            );
        }

        EndDrawing();
    }
}