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

/**
 * Función para dibujar los puntos calculados en la curva de Rosa Polar con movimiento.
 * 
 * @param renderer Renderizador de SDL
 * @param rosa Estructura con los parámetros de la rosa
 * @param rotation_angle Ángulo de rotación actual
 */
void drawMovingPoints(SDL_Renderer* renderer, const RosaPolar& rosa, float rotation_angle) 
{
    // Establece el color de la rosa en el renderizador
    SDL_SetRenderDrawColor(renderer, rosa.color.r, rosa.color.g, rosa.color.b, rosa.color.a);

    // Dibuja los puntos de la rosa en el renderizador en paralelo
    #pragma omp parallel for
    for (int i = 0; i < rosa.num_points_total; ++i) 
    {   
        // Calcula la posición de cada punto en la curva
        float theta = (i + rotation_angle) * (2.0f * M_PI / rosa.num_points);
        float r = rosa.scale * sin(rosa.k * theta);

        // Calcula las coordenadas del punto
        int x = static_cast<int>(r * cos(theta + rotation_angle)) + rosa.x_origin;
        int y = static_cast<int>(r * sin(theta + rotation_angle)) + rosa.y_origin;
        
        // Dibuja el punto en el renderizador
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

/**
 * Función para rellenar los pétalos de las rosas con color.
 * 
 * @param renderer Renderizador de SDL
 * @param rosa Estructura con los parámetros de la rosa
 * @param rotation_angle Ángulo de rotación actual
 */
void drawFilledPetals(SDL_Renderer* renderer, const RosaPolar& rosa, float rotation_angle) 
{
    SDL_SetRenderDrawColor(renderer, rosa.color.r, rosa.color.g, rosa.color.b, rosa.color.a);

    // Dibujar triángulos que parten del centro y cubren los pétalos en paralelo
    #pragma omp parallel for
    for (int i = 0; i < rosa.num_points; ++i) 
    {
        float theta = (i + rotation_angle) * (2.0f * M_PI / rosa.num_points);
        float next_theta = (i + 1 + rotation_angle) * (2.0f * M_PI / rosa.num_points);
        float r = rosa.scale * sin(rosa.k * theta);
        float next_r = rosa.scale * sin(rosa.k * next_theta);

        int x1 = static_cast<int>(r * cos(theta + rotation_angle)) + rosa.x_origin;
        int y1 = static_cast<int>(r * sin(theta + rotation_angle)) + rosa.y_origin;
        int x2 = static_cast<int>(next_r * cos(next_theta + rotation_angle)) + rosa.x_origin;
        int y2 = static_cast<int>(next_r * sin(next_theta + rotation_angle)) + rosa.y_origin;

        // Dibuja triángulos para rellenar el área entre los puntos
        SDL_RenderDrawLine(renderer, rosa.x_origin, rosa.y_origin, x1, y1);
        SDL_RenderDrawLine(renderer, rosa.x_origin, rosa.y_origin, x2, y2);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

/**
 * Función para generar un color aleatorio.
 */
SDL_Color generateRandomColor() 
{
    SDL_Color color;
    color.r = rand() % 256;
    color.g = rand() % 256;
    color.b = rand() % 256;
    color.a = 255;
    return color;
}

/**
 * Función para procesar los argumentos de la línea de comandos.
 */
int parseArguments(int argc, char* argv[]) 
{
    int quantity = 1; // Valor por defecto

    bool q_provided = false;

    for (int i = 1; i < argc; ++i) 
    {   
        if (std::string(argv[i]) == "-q" && i + 1 < argc) 
        {
            try 
            {   
                quantity = std::stoi(argv[++i]);
                q_provided = true;

                if (quantity <= 0) 
                {
                    std::cerr << "ERROR: El valor de -q debe ser un número mayor que 0." << std::endl;
                    exit(EXIT_FAILURE);
                }

            } 
            catch (std::invalid_argument&) 
            {
                std::cerr << "ERROR: Argumento inválido para -q." << std::endl;
                exit(EXIT_FAILURE);
            } 
            catch (std::out_of_range&) 
            {
                std::cerr << "ERROR: El valor de -q es demasiado grande." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    if (!q_provided) 
    {
        std::cerr << "ERROR: No se proporcionó el argumento -q." << std::endl;
        exit(EXIT_FAILURE);
    }

    return quantity;
}

/**
 * Función para generar una rosa polar con parámetros aleatorios.
 */
RosaPolar generateRosaPolar() 
{
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

/**
 * Función principal.
 */
int main(int argc, char* argv[]) 
{
    srand(time(nullptr));

    int quantity = parseArguments(argc, argv);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cerr << "ERROR: SDL no pudo inicializarse! SDL_Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    SDL_Window* window = SDL_CreateWindow("Curvas de Rosa Polar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) 
    {
        std::cerr << "ERROR: La ventana no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) 
    {
        std::cerr << "ERROR: El renderizador no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    std::vector<RosaPolar> rosas(quantity);
    std::vector<float> rotation_angles(quantity, 0.0f);

    #pragma omp parallel for
    for (int i = 0; i < quantity; ++i) 
    {
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

    while (!quit) 
    {
        // Verifica si han pasado 15 segundos
        if (SDL_GetTicks() - startTime > max_duration) 
        {
            quit = true;
        }

        while (SDL_PollEvent(&e) != 0) 
        {
            if (e.type == SDL_QUIT) 
            {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Paralelización del bucle que dibuja las rosas
        #pragma omp parallel for
        for (int i = 0; i < quantity; ++i) 
        {
            drawFilledPetals(renderer, rosas[i], rotation_angles[i]);
            rotation_angles[i] += rosas[i].rotation_speed;
        }

        SDL_RenderPresent(renderer);

        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - fpsStartTime >= 1000) 
        {
            float fps = frameCount / ((currentTime - fpsStartTime) / 1000.0f);
            fps = std::round(fps * 100) / 100.0f;
            fpsHistory.push_back(fps);
            std::string fpsTitle = "Curvas de Rosa Polar - FPS: " + std::to_string(fps);
            SDL_SetWindowTitle(window, fpsTitle.c_str());
            frameCount = 0;
            fpsStartTime = currentTime;
        }

        lastTime = currentTime;
    }

    // Código para generar el informe de FPS (como ya estaba en tu código)
    if (!fpsHistory.empty()) 
    {
        float avgFPS = std::accumulate(fpsHistory.begin(), fpsHistory.end(), 0.0f) / fpsHistory.size();
        float minFPS = *std::min_element(fpsHistory.begin(), fpsHistory.end());
        float maxFPS = *std::max_element(fpsHistory.begin(), fpsHistory.end());
        fpsHistory.sort();
        auto fps1PercentLow_it = std::next(fpsHistory.begin(), fpsHistory.size() / 100);
        float fps1PercentLow = *fps1PercentLow_it;

        std::ofstream fpsReport("reports/par_report.txt");
        if (fpsReport.is_open()) 
        {
            fpsReport << "Curvas de Rosa Polar Paralelizado con OpenMP" << std::endl;
            fpsReport << "Cantidad de Rosas: " << quantity << std::endl;
            fpsReport << "-------------------------------------" << std::endl;
            fpsReport << "Metrics Report:" << std::endl;
            fpsReport << "Average FPS: " << avgFPS << std::endl;
            fpsReport << "Minimum FPS: " << minFPS << std::endl;
            fpsReport << "Maximum FPS: " << maxFPS << std::endl;
            fpsReport << "1% Low FPS: " << fps1PercentLow << std::endl;
            fpsReport.close();
        } 
        else 
        {
            std::cerr << "ERROR: No se pudo abrir el archivo para guardar el informe de FPS." << std::endl;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
