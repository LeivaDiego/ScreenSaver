#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// Parámetros de la pantalla
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Estructura para almacenar los parámetros de cada Rosa Polar
struct RosaPolar {
    float k;          // Número de pétalos
    float scale;      // Escala del tamaño
    SDL_Color color;  // Color de la rosa
    int x_origin;     // Coordenada x del origen
    int y_origin;     // Coordenada y del origen
    int num_points;   // Cantidad total de puntos en la curva
    int num_points_total;  // Cantidad de puntos que se moverán a lo largo de la curva
    float rotation_speed;  // Velocidad de rotación
};

// Función para dibujar puntos en la curva de Rosa Polar con rotación
void drawMovingPoints(SDL_Renderer* renderer, const RosaPolar& rosa, float rotation_angle) {
    SDL_SetRenderDrawColor(renderer, rosa.color.r, rosa.color.g, rosa.color.b, rosa.color.a);

    for (int i = 0; i < rosa.num_points_total; ++i) {
        float theta = (i + rotation_angle) * (2.0f * M_PI / rosa.num_points);
        float r = rosa.scale * sin(rosa.k * theta);

        int x = static_cast<int>(r * cos(theta + rotation_angle)) + rosa.x_origin;
        int y = static_cast<int>(r * sin(theta + rotation_angle)) + rosa.y_origin;

        SDL_RenderDrawPoint(renderer, x, y);
    }
}

// Función para generar un color aleatorio
SDL_Color generateRandomColor() {
    SDL_Color color;
    color.r = rand() % 256;
    color.g = rand() % 256;
    color.b = rand() % 256;
    color.a = 255; // Opacidad completa
    return color;
}

// Función para procesar argumentos y devolver la cantidad de rosas
int parseArguments(int argc, char* argv[]) {
    int quantity = 1; // Valor por defecto
    bool q_provided = false; // Bandera para verificar si se proporcionó -q

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-q" && i + 1 < argc) {
            try {
                quantity = std::stoi(argv[++i]);
                q_provided = true; // Se encontró -q y se asignó un valor

                if (quantity <= 0) {
                    std::cerr << "ERROR: El valor de -q debe ser un número mayor que 0." << std::endl;
                    exit(EXIT_FAILURE);
                }
            } catch (std::invalid_argument& e) {
                std::cerr << "ERROR: Argumento inválido para -q." << std::endl;
                exit(EXIT_FAILURE);
            } catch (std::out_of_range& e) {
                std::cerr << "ERROR: El valor de -q es demasiado grande." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    if (!q_provided) {
        std::cerr << "ERROR: No se proporcionó el argumento -q." << std::endl;
        exit(EXIT_FAILURE);
    }

    return quantity;
}

// Función para generar una rosa con parámetros aleatorios
// Función para generar una rosa con parámetros aleatorios
RosaPolar generateRosaPolar() {
    RosaPolar rosa;
    int petalos_options[] = {3, 5, 7, 9, 11, 13, 15};
    rosa.k = petalos_options[rand() % 7];  // Selecciona aleatoriamente uno de los valores de pétalos

    // Convertimos rosa.k a int antes de aplicar el operador %
    int k_int = static_cast<int>(rosa.k);
    if (k_int < 3 || k_int > 15 || (k_int % 2 == 0 && k_int != 3)) {
        std::cerr << "WARNING: Se generó un valor inesperado para k. Se establecerá k en 5 por defecto." << std::endl;
        rosa.k = 5;
    }

    rosa.scale = 75.0f + rand() % 76;  // Escala entre 75 y 150
    if (rosa.scale < 75.0f || rosa.scale > 150.0f) {
        std::cerr << "WARNING: Se generó un valor inesperado para la escala. Se establecerá la escala en 100 por defecto." << std::endl;
        rosa.scale = 100.0f;
    }

    rosa.color = generateRandomColor();
    rosa.x_origin = rand() % SCREEN_WIDTH;
    rosa.y_origin = rand() % SCREEN_HEIGHT;
    rosa.num_points_total = 25 * ((k_int - 3) / 2) + 50;  // Según patrón identificado
    rosa.num_points = 2 * rosa.num_points_total;

    rosa.rotation_speed = 0.0001f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (0.0005f - 0.0001f)));
    if (rosa.rotation_speed < 0.0001f || rosa.rotation_speed > 0.0005f) {
        std::cerr << "WARNING: Se generó un valor inesperado para la velocidad de rotación. Se establecerá la velocidad en 0.0003f por defecto." << std::endl;
        rosa.rotation_speed = 0.0003f;
    }

    return rosa;
}


int main(int argc, char* argv[]) {
    srand(time(nullptr)); // Inicialización de la semilla aleatoria

    // Procesa argumentos
    int quantity = parseArguments(argc, argv);

    // Inicialización de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "ERROR: SDL no pudo inicializarse! SDL_Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    SDL_Window* window = SDL_CreateWindow("Curvas de Rosa Polar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "ERROR: La ventana no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit(); // Liberar recursos de SDL
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "ERROR: El renderizador no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window); // Liberar recursos de la ventana
        SDL_Quit(); // Liberar recursos de SDL
        exit(EXIT_FAILURE);
    }

    // Genera las rosas según la cantidad especificada
    std::vector<RosaPolar> rosas;
    for (int i = 0; i < quantity; ++i) {
        rosas.push_back(generateRosaPolar());
    }

    bool quit = false;
    SDL_Event e;
    std::vector<float> rotation_angles(quantity, 0.0f);  // Ángulos de rotación iniciales

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Limpia la pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dibuja cada rosa en la pantalla
        for (int i = 0; i < quantity; ++i) {
            drawMovingPoints(renderer, rosas[i], rotation_angles[i]);
            rotation_angles[i] += rosas[i].rotation_speed;  // Actualiza el ángulo de rotación para la rosa
        }

        // Presenta la escena
        SDL_RenderPresent(renderer);
    }

    // Limpia y cierra
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
