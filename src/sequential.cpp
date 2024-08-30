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
    // Establece el color de la rosa de manera aleatoria
    SDL_SetRenderDrawColor(renderer, rosa.color.r, rosa.color.g, rosa.color.b, rosa.color.a);

    // Dibuja los puntos de la curva de Rosa Polar
    for (int i = 0; i < rosa.num_points_total; ++i) {
        // Calcula la posición de cada punto en la curva
        float theta = (i + rotation_angle) * (2.0f * M_PI / rosa.num_points);
        float r = rosa.scale * sin(rosa.k * theta);

        // Calcula las coordenadas del punto
        int x = static_cast<int>(r * cos(theta + rotation_angle)) + rosa.x_origin;
        int y = static_cast<int>(r * sin(theta + rotation_angle)) + rosa.y_origin;

        // Dibuja el punto
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
    bool q_provided = false;

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-q" && i + 1 < argc) {
            quantity = std::stoi(argv[++i]);
            q_provided = true;
        }
    }

    if (!q_provided) {
        std::cout << "No se proporcionó el argumento -q. Se usará el valor predeterminado: 1 rosa." << std::endl;
    }

    return quantity;
}

// Función para generar una rosa con parámetros aleatorios
RosaPolar generateRosaPolar() {
    RosaPolar rosa;
    int petalos_options[] = {3, 5, 7, 9, 11, 13, 15};
    rosa.k = petalos_options[rand() % 7];  // Selecciona aleatoriamente uno de los valores de pétalos
    rosa.scale = 75.0f + rand() % 76;  // Escala entre 75 y 150
    rosa.color = generateRandomColor();
    rosa.x_origin = rand() % SCREEN_WIDTH;
    rosa.y_origin = rand() % SCREEN_HEIGHT;
    rosa.num_points_total = 25 * ((rosa.k - 3) / 2) + 50;  // Según patrón identificado
    rosa.num_points = 2 * rosa.num_points_total;
    rosa.rotation_speed = 0.0001f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (0.0005f - 0.0001f)));  // Velocidad de rotación aleatoria
    return rosa;
}

int main(int argc, char* argv[]) {
    srand(time(nullptr)); // Inicialización de la semilla aleatoria

    // Procesa argumentos
    int quantity = parseArguments(argc, argv);

    // Inicialización de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL no pudo inicializarse! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Curvas de Rosa Polar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "La ventana no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "El renderizador no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
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