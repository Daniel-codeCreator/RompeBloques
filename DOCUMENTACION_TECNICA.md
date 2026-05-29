# Documentación Técnica - Proyecto Arcade (Rompe Bloques)

Este documento detalla la estructura y el funcionamiento del código para los archivos principales del juego.

---

## 1. Archivo: `main.cpp`
Es el punto de entrada de la aplicación.
- **Función `main()`**:
  - Inicializa la ventana con Raylib.
  - Configura el objeto `ApiClient` para la comunicación con el servidor.
  - Gestiona el flujo principal: primero muestra el Login y, tras una autenticación exitosa, abre el Menú Principal.
  - Controla el cierre limpio de la aplicación.

---

## 2. Módulo de Login (`auth/Login.h` y `auth/Login.cpp`)
Encargado de la gestión de acceso de usuarios.
- **`mostrarLogin()`**: Ejecuta el bucle de la interfaz de login. Dibuja los cuadros de texto, el botón de visibilidad de contraseña y el botón de ingreso. Gestiona los eventos de teclado para escribir el usuario y la contraseña.
- **`iniciarSesion()`**: Valida que los campos no estén vacíos y utiliza el `ApiClient` para verificar las credenciales en el servidor.
- **`estaAutenticado()`**: Devuelve el estado actual de la sesión.
- **`cerrarSesion()`**: Finaliza la sesión actual del usuario.

---

## 3. Módulo de Menú (`src/Menu.h` y `src/Menu.cpp`)
Interfaz de navegación después de iniciar sesión.
- **`mostrar()`**: Dibuja el menú principal con opciones para "Jugar" y "Cerrar Sesión".
  - Al pulsar "Jugar", llama a la función `iniciarRompeBloque`.
  - Al pulsar "Cerrar Sesión", notifica a la API y regresa a la pantalla de Login.

---

## 4. Módulo Rompe Bloques (`src/RompeBloques.h` y `src/RompeBloques.cpp`)
Contiene toda la lógica del juego principal.

### Funciones de Control y Lógica:
- **`iniciarRompeBloque()`**: Función principal que orquestra el juego. Inicializa recursos, sonidos y el bucle de juego.
- **`cargarNivel()`**: Configura la posición y resistencia de los bloques dependiendo del nivel actual.
- **`moverBarra()`**: Controla el movimiento de la paleta del jugador.
- **`moverPelotas()`**: Gestiona el movimiento físico de las pelotas y sus colisiones con paredes, bloques y paleta.
- **`actualizarPowerUp()`**: Maneja la generación y aplicación de poderes especiales (pelota extra, barra grande, bomba).
- **`subirNivel()`**: Prepara el juego para el siguiente nivel de dificultad.
- **`reiniciarJuego()`**: Restablece el estado inicial de la partida.

### Integración con API en el juego:
- **`iniciarNuevaPartida()`**: Registra el comienzo de una sesión de juego en el servidor.
- **`reportarScoreSiCorresponde()`**: Envía el puntaje acumulado al servidor durante la partida.
- **`finalizarEnHilo()`**: Envía los resultados finales de la partida en un hilo separado para no afectar el rendimiento visual.
- **`calcularTokensPorNivel()`**: Determina la recompensa en tokens según el nivel alcanzado.

### Funciones de Dibujo:
- **`dibujarHUD()`**: Muestra vidas, puntos y nivel actual.
- **`dibujarPantallaFinal()`**: Muestra el resultado (victoria/derrota) y opciones para reintentar o salir.
- **`dibujarBloques()`, `dibujarPelotas()`, `dibujarBarra()`**: Renderizan los elementos visuales del juego.
