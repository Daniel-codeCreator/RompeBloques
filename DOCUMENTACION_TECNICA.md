# Documentación Técnica - Proyecto Arcade (Rompe Bloques)

Este documento detalla la estructura y el funcionamiento del código del "Proyecto Arcade", específicamente el juego "Rompe Bloques". El proyecto está desarrollado en C++ utilizando la biblioteca **Raylib** para la interfaz gráfica y **CPR** para el consumo de una API REST.

---

## 1. Punto de Entrada (`main.cpp`)

Es el archivo principal que orquesta la ejecución del programa.
- **`main()`**:
  - Inicializa la ventana de Raylib (`InitWindow`).
  - Configura el cliente de la API (`ApiClient`) con las credenciales del juego.
  - Ejecuta un bucle principal que alterna entre la pantalla de **Login** y el **Menú Principal** si el usuario está autenticado.

---

## 2. Módulo de API (`Api´s/`)

Encargado de la comunicación con el servidor backend para gestionar sesiones, partidas y rankings.

### `ApiClient.h` / `ApiClient.cpp`
Clase que maneja las peticiones HTTP (GET/POST).
- **`probarConexion()`**: Verifica la disponibilidad del servidor y la validez de la API KEY.
- **`loginJugador()`**: Autentica al usuario y obtiene un token de sesión (JWT) y sus datos básicos (saldo, correo).
- **`iniciarPartida()`**: Registra el inicio de una nueva partida en el servidor y descuenta los tokens correspondientes.
- **`reportarScore()`**: Envía el puntaje actual al servidor periódicamente durante el juego.
- **`finalizarPartida()`**: Envía los resultados finales (score, nivel, victoria/derrota, tokens ganados) para cerrar la sesión de juego en el backend.
- **`consultarRanking()`**: Recupera la lista de los mejores puntajes registrados para este juego.

### `GameApiConfig.h`
Espacio de nombres (`GameApiConfig`) que contiene constantes de configuración:
- `BASE_URL`: Dirección del servidor.
- `API_KEY` y `CODIGO_JUEGO`: Identificadores únicos del proyecto.
- `COSTO_PARTIDA`, `PREMIOS`: Reglas de negocio del juego.

---

## 3. Módulo de Autenticación (`auth/`)

Gestiona la interfaz de acceso de los usuarios.

### `Login.h` / `Login.cpp`
- **`mostrarLogin()`**: Dibuja la interfaz de inicio de sesión (campos de texto para usuario y contraseña, botón de login). Maneja la entrada de teclado y eventos de mouse.
- **`iniciarSesion()`**: Lógica interna que valida que los campos no estén vacíos y llama a `api.loginJugador()`.
- **`estaAutenticado()`**: Verifica si existe una sesión activa.
- **`cerrarSesion()`**: Limpia los datos del usuario y el token.

---

## 4. Módulo del Menú (`src/Menu.h / Menu.cpp`)

- **`mostrar()`**: Presenta el menú principal después del login. Permite al usuario:
  - **Jugar**: Inicia el módulo `RompeBloques`.
  - **Cerrar Sesión**: Regresa a la pantalla de login.

---

## 5. Módulo del Juego (`src/RompeBloques.h / RompeBloques.cpp`)

Contiene toda la lógica de físicas, renderizado y reglas del juego "Rompe Bloques".

### Funciones Principales de Lógica
- **`iniciarRompeBloque()`**: Función principal que contiene el bucle de juego (`while (!WindowShouldClose())`).
- **`cargarNivel()`**: Configura la disposición de los bloques, sus colores y vidas según el nivel actual.
- **`moverBarra()`**: Gestiona el movimiento horizontal de la plataforma mediante teclado (Flechas o A/D).
- **`moverPelotas()`**: Maneja las colisiones de las pelotas con las paredes, la barra y los bloques. Actualiza el puntaje y detecta si una pelota sale del área de juego.
- **`actualizarPowerUp()`**: Controla la aparición, caída y efecto de los Power-Ups al colisionar con la barra.
- **`subirNivel()`**: Reinicia los elementos para avanzar al siguiente nivel de dificultad.
- **`reiniciarJuego()`**: Restablece todas las variables (puntos, vidas, nivel) para una nueva partida.

### Integración con la API
- **`reportarScoreSiCorresponde()`**: Llama a la API para guardar el progreso si se ha superado un umbral de puntos.
- **`finalizarEnHilo()`**: Ejecuta la llamada a `finalizarPartida` en un hilo secundario (`std::thread`) para evitar que el juego se congele mientras espera la respuesta del servidor.
- **`calcularTokensPorNivel()`**: Determina cuántos tokens ha ganado el jugador basado en su desempeño.

### Funciones de Renderizado (UI)
- **`dibujarAreaJuego()`**: Dibuja los límites y marcos del juego.
- **`dibujarPelotas()`, `dibujarBloques()`, `dibujarBarra()`, `dibujarPowerUp()`**: Funciones encargadas de pintar cada elemento en pantalla.
- **`dibujarHUD()`**: Muestra la información de vidas, puntos y nivel actual en el lateral derecho.
- **`dibujarPantallaFinal()`**: Muestra el mensaje de "GANASTE" o "GAME OVER" con botones para reintentar o volver al menú.

---

## Estructuras de Datos
- **`Bloque`**: Almacena posición (`Rectangle`), estado, color y resistencia (vida).
- **`Pelota`**: Almacena posición, velocidad, radio y estados especiales (como si es tipo "bomba").
- **`PowerUp`**: Almacena posición, tipo de poder y estado de actividad.
