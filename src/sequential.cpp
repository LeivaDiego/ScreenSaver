#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>

// Parámetros de la pantalla
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Función para dibujar la curva de Rosa Polar
void drawRosaPolar(SDL_Renderer* renderer, float k, int num_points, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    for (int i = 0; i < num_points; ++i) {
        float theta = i * (2.0f * M_PI / num_points);
        float r = 150 * sin(k * theta); // Escala del radio
        
        int x = static_cast<int>(r * cos(theta)) + SCREEN_WIDTH / 2;
        int y = static_cast<int>(r * sin(theta)) + SCREEN_HEIGHT / 2;
        
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
    float k = 5.0f;  // Valor inicial de k
    int num_points = 1000;
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

        // Dibuja la curva de Rosa Polar
        drawRosaPolar(renderer, k, num_points, color);

        // Presenta la escena
        SDL_RenderPresent(renderer);

        // Incrementa k para animación
        k += 0.01f;
    }

    // Limpia y cierra
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
