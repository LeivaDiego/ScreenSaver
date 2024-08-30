#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>

// Parámetros de la pantalla
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int NUM_POINTS = 125;  // Cantidad de puntos que se moverán a lo largo de la curva
const float ROTATION_SPEED = 0.0003f;  // Velocidad de rotación de la flor

// Función para dibujar puntos en la curva de Rosa Polar con rotación
void drawMovingPoints(SDL_Renderer* renderer, float k, float scale, int num_points, float rotation_angle, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int i = 0; i < NUM_POINTS; ++i) {
        // Calcula el ángulo theta para cada punto
        float theta = (i + rotation_angle) * (2.0f * M_PI / num_points);
        // Calcula el radio r usando la función de Rosa Polar
        float r = scale * sin(k * theta);

        // Calcula las coordenadas x e y con rotación
        int x = static_cast<int>(r * cos(theta + rotation_angle)) + SCREEN_WIDTH / 2;
        int y = static_cast<int>(r * sin(theta + rotation_angle)) + SCREEN_HEIGHT / 2;

        // Dibuja el punto
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

int main(int argc, char* args[]) {
    // Inicialización de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL no pudo inicializarse! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Curva de Rosa Polar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "La ventana no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "El renderizador no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    float k = 9.0f;  // Valor de k para una flor de 5 pétalos
    float scale = 150.0f;  // Escala del tamaño de la flor
    float rotation_angle = 0.0f;  // Ángulo de rotación inicial
    int num_points = 2 * NUM_POINTS;  // Cantidad total de puntos en la curva
    SDL_Color color = {255, 0, 0, 255}; // Color rojo

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Limpia la pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dibuja los puntos en movimiento a lo largo de la curva de Rosa Polar
        drawMovingPoints(renderer, k, scale, num_points, rotation_angle, color);

        // Presenta la escena
        SDL_RenderPresent(renderer);

        // Incrementa el ángulo de rotación para crear el efecto de giro
        rotation_angle += ROTATION_SPEED;
    }

    // Limpia y cierra
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

