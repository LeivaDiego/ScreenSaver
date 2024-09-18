#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <list>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <omp.h>
#include <vector>

// Parámetros de la pantalla
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Estructura para almacenar los parámetros de cada Rosa Polar
struct RosaPolar 
{
    float k;          // Número de pétalos
    float scale;      // Escala del tamaño
    SDL_Color color;  // Color de la rosa
    int x_origin;     // Coordenada x del origen
    int y_origin;     // Coordenada y del origen
    int num_points;   // Cantidad total de puntos en la curva
    int num_points_total;  // Cantidad de puntos que se moverán a lo largo de la curva
    float rotation_speed;  // Velocidad de rotación
};

// Estructura para almacenar los puntos a dibujar de cada rosa
struct Point {
    int x, y;
};

// Función para generar los puntos de una rosa polar
std::vector<Point> calculateRosePoints(const RosaPolar& rosa, float rotation_angle) {
    std::vector<Point> points(rosa.num_points_total);
    
    #pragma omp parallel for
    for (int i = 0; i < rosa.num_points_total; ++i) {
        float theta = (i + rotation_angle) * (2.0f * M_PI / rosa.num_points);
        float r = rosa.scale * sin(rosa.k * theta);
        
        points[i].x = static_cast<int>(r * cos(theta + rotation_angle)) + rosa.x_origin;
        points[i].y = static_cast<int>(r * sin(theta + rotation_angle)) + rosa.y_origin;
    }
    return points;
}

// Función simplificada para rellenar los pétalos
void fillPetalsWithLines(SDL_Renderer* renderer, const std::vector<Point>& points, int x_origin, int y_origin) {
    // Dibujamos líneas que conectan el centro con cada punto de la rosa
    for (size_t i = 0; i < points.size(); ++i) {
        SDL_RenderDrawLine(renderer, x_origin, y_origin, points[i].x, points[i].y);
    }
}

// Función para dibujar el contorno de las rosas
void drawRoseContour(SDL_Renderer* renderer, const std::vector<Point>& points) {
    for (size_t i = 0; i < points.size() - 1; ++i) {
        SDL_RenderDrawLine(renderer, points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
    }
    // Conecta el último punto con el primero para cerrar el contorno
    SDL_RenderDrawLine(renderer, points.back().x, points.back().y, points.front().x, points.front().y);
}

// Función para generar un color aleatorio.
SDL_Color generateRandomColor() {
    SDL_Color color;
    color.r = rand() % 256;
    color.g = rand() % 256;
    color.b = rand() % 256;
    color.a = 255;
    return color;
}

// Función para procesar los argumentos de la línea de comandos.
int parseArguments(int argc, char* argv[]) {
    int quantity = 1; // Valor por defecto
    bool q_provided = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-q" && i + 1 < argc) {
            try {
                quantity = std::stoi(argv[++i]);
                q_provided = true;
                if (quantity <= 0) {
                    std::cerr << "ERROR: El valor de -q debe ser un número mayor que 0." << std::endl;
                    exit(EXIT_FAILURE);
                }
            } catch (...) {
                std::cerr << "ERROR: Argumento inválido para -q." << std::endl;
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

// Función para generar una rosa polar con parámetros aleatorios.
RosaPolar generateRosaPolar() {
    RosaPolar rosa;
    int petalos_options[] = {3, 5, 7, 9, 11, 13, 15};
    rosa.k = petalos_options[rand() % 7];  
    rosa.scale = 25.0f + rand() % 76;
    rosa.color = generateRandomColor();
    rosa.x_origin = rand() % SCREEN_WIDTH;
    rosa.y_origin = rand() % SCREEN_HEIGHT;
    rosa.num_points_total = 25 * ((static_cast<int>(rosa.k) - 3) / 2) + 50;
    rosa.num_points = 2 * rosa.num_points_total;
    rosa.rotation_speed = 0.01f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (0.0005f - 0.0001f)));
    return rosa;
}

// Función principal.
int main(int argc, char* argv[]) {
    srand(time(nullptr));
    int quantity = parseArguments(argc, argv);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "ERROR: SDL no pudo inicializarse! SDL_Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    SDL_Window* window = SDL_CreateWindow("Curvas de Rosa Polar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "ERROR: La ventana no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "ERROR: El renderizador no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    std::vector<RosaPolar> rosas(quantity);
    std::vector<float> rotation_angles(quantity, 0.0f);
    
    #pragma omp parallel for
    for (int i = 0; i < quantity; ++i) {
        rosas[i] = generateRosaPolar();
    }

    bool quit = false;
    SDL_Event e;

    Uint32 frameCount = 0;
    Uint32 lastTime = SDL_GetTicks();
    Uint32 fpsStartTime = lastTime;
    std::list<float> fpsHistory;

    Uint32 startTime = SDL_GetTicks();  // Guardar el tiempo de inicio
    Uint32 max_duration = 15000;  // 15 segundos en milisegundos

    while (!quit) {
        // Verifica si han pasado 15 segundos
        if (SDL_GetTicks() - startTime > max_duration) {
            quit = true;
        }

        // Calcula el tiempo de ejecución en segundos
        Uint32 currentTime = SDL_GetTicks();
        float elapsedTime = (currentTime - startTime) / 1000.0f;  // Convertir a segundos

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Paralelización del cálculo de puntos en cada fotograma
        std::vector<std::vector<Point>> all_rose_points(quantity);

        #pragma omp parallel for
        for (int i = 0; i < quantity; ++i) {
            all_rose_points[i] = calculateRosePoints(rosas[i], rotation_angles[i]);
            rotation_angles[i] += rosas[i].rotation_speed;  // Rotación continua
        }

        // Renderizado en el hilo principal
        for (int i = 0; i < quantity; ++i) {
            SDL_SetRenderDrawColor(renderer, rosas[i].color.r, rosas[i].color.g, rosas[i].color.b, rosas[i].color.a);

            // Rellenar pétalos con líneas
            fillPetalsWithLines(renderer, all_rose_points[i], rosas[i].x_origin, rosas[i].y_origin);
            
            // Dibujar el contorno de la rosa
            drawRoseContour(renderer, all_rose_points[i]);
        }

        SDL_RenderPresent(renderer);

        frameCount++;
        currentTime = SDL_GetTicks();
        if (currentTime - fpsStartTime >= 1000) {
            float fps = frameCount / ((currentTime - fpsStartTime) / 1000.0f);
            fps = std::round(fps * 100) / 100.0f;
            fpsHistory.push_back(fps);

            // Actualizar el título de la ventana con FPS y tiempo transcurrido
            std::string fpsTitle = "Curvas de Rosa Polar - FPS: " + std::to_string(fps) +
                                " - Tiempo: " + std::to_string(elapsedTime) + "s";
            SDL_SetWindowTitle(window, fpsTitle.c_str());

            frameCount = 0;
            fpsStartTime = currentTime;
        }

        lastTime = currentTime;
    }


    // Generar informe de FPS (como ya estaba en tu código)
    if (!fpsHistory.empty()) {
        float avgFPS = std::accumulate(fpsHistory.begin(), fpsHistory.end(), 0.0f) / fpsHistory.size();
        float minFPS = *std::min_element(fpsHistory.begin(), fpsHistory.end());
        float maxFPS = *std::max_element(fpsHistory.begin(), fpsHistory.end());
        fpsHistory.sort();
        auto fps1PercentLow_it = std::next(fpsHistory.begin(), fpsHistory.size() / 100);
        float fps1PercentLow = *fps1PercentLow_it;

        std::ofstream fpsReport("reports/par_report.txt");
        if (fpsReport.is_open()) {
            fpsReport << "Curvas de Rosa Polar Paralelizado" << std::endl;
            fpsReport << "Cantidad de Rosas: " << quantity << std::endl;
            fpsReport << "-------------------------------------" << std::endl;
            fpsReport << "Metrics Report:" << std::endl;
            fpsReport << "Average FPS: " << avgFPS << std::endl;
            fpsReport << "Minimum FPS: " << minFPS << std::endl;
            fpsReport << "Maximum FPS: " << maxFPS << std::endl;
            fpsReport << "1% Low FPS: " << fps1PercentLow << std::endl;
            fpsReport.close();
        } else {
            std::cerr << "ERROR: No se pudo abrir el archivo para guardar el informe de FPS." << std::endl;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
