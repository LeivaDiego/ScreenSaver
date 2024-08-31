#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <list>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <numeric>
#include <algorithm>

// Parámetros de la pantalla
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

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
 * Apoyadose en sdl para dibujar los puntos.
 * 
 * @param renderer Renderizador de SDL
 * @param rosa Estructura con los parámetros de la rosa
 * @param rotation_angle Ángulo de rotación actual
 * @return void
 */
void drawMovingPoints(SDL_Renderer* renderer, const RosaPolar& rosa, float rotation_angle) 
{
    // Establece el color de la rosa en el renderizador
    SDL_SetRenderDrawColor(renderer, rosa.color.r, rosa.color.g, rosa.color.b, rosa.color.a);

    // Dibuja los puntos de la rosa en el renderizador
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
 * Función para generar un color aleatorio.
 * 
 * @return SDL_Color Estructura con los valores de color generados aleatoriamente
 */
SDL_Color generateRandomColor() 
{
    // Instancia un color de SDL
    SDL_Color color;

    // Genera valores aleatorios para los componentes de color
    color.r = rand() % 256;
    color.g = rand() % 256;
    color.b = rand() % 256;
    color.a = 255; // Opacidad completa

    // Devuelve el color generado
    return color;
}

/**
 * Función para procesar los argumentos de la línea de comandos.
 * 
 * @param argc Cantidad de argumentos
 * @param argv Arreglo de argumentos
 * @return int Cantidad de rosas a generar
 */
int parseArguments(int argc, char* argv[]) 
{
    // Cantidad de rosas a generar
    int quantity = 1; // Valor por defecto

    bool q_provided = false; // Bandera para verificar si se proporcionó -q

    // Recorre los argumentos para buscar la cantidad de rosas
    for (int i = 1; i < argc; ++i) 
    {   
        // Verifica si se proporcionó -q y asigna el valor
        if (std::string(argv[i]) == "-q" && i + 1 < argc) 
        {
            try 
            {   
                // Convierte el valor a entero
                quantity = std::stoi(argv[++i]);
                q_provided = true; // Se encontró -q y se asignó un valor

                // Verifica que la cantidad sea válida (mayor a 0)
                if (quantity <= 0) 
                {
                    std::cerr << "ERROR: El valor de -q debe ser un número mayor que 0." << std::endl;
                    exit(EXIT_FAILURE);
                }

            } 
            catch (std::invalid_argument& e) 
            {
                std::cerr << "ERROR: Argumento inválido para -q." << std::endl;
                exit(EXIT_FAILURE);
            } 
            catch (std::out_of_range& e) 
            {
                std::cerr << "ERROR: El valor de -q es demasiado grande." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    // Verifica si se proporcionó -q y se asignó un valor
    if (!q_provided) 
    {
        std::cerr << "ERROR: No se proporcionó el argumento -q." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Devuelve la cantidad de rosas a generar
    return quantity;
}

/**
 * Función para generar una rosa polar con parámetros aleatorios.
 * Genera valores aleatorios para los parámetros de la rosa, como 
 * el número de pétalos, la escala, el color, el origen y la velocidad de rotación.
 * 
 * @return RosaPolar Estructura con los parámetros generados aleatoriamente
 */
RosaPolar generateRosaPolar() 
{
    // Instancia una rosa polar
    RosaPolar rosa;

    // Valores posibles para el número de pétalos
    int petalos_options[] = {3, 5, 7, 9, 11, 13, 15};
    // Selecciona aleatoriamente uno de los valores de pétalos
    rosa.k = petalos_options[rand() % 7];  

    // Verifica que el valor de k sea válido 
    int k_int = static_cast<int>(rosa.k);
    if (k_int < 3 || k_int > 15 || (k_int % 2 == 0 && k_int != 3)) 
    {
        std::cerr << "WARNING: Se generó un valor inesperado para k. Se establecerá k en 5 por defecto." << std::endl;
        rosa.k = 5;
    }

    // Genera un valor aleatorio para la escala entre 25 y 100
    rosa.scale = 25.0f + rand() % 76;
    
    // Verifica que el valor de la escala sea válido
    if (rosa.scale < 25.0f || rosa.scale > 100.0f) 
    {
        std::cerr << "WARNING: Se generó un valor inesperado para la escala. Se establecerá la escala en 50 por defecto." << std::endl;
        rosa.scale = 50.0f;
    }

    // Genera un color aleatorio para la rosa a partir 
    // de la función auxiliar generateRandomColor
    rosa.color = generateRandomColor();
    
    // Genera valores aleatorios para el origen de la rosa
    rosa.x_origin = rand() % SCREEN_WIDTH;
    rosa.y_origin = rand() % SCREEN_HEIGHT;

    // Calcula la cantidad total de puntos en la curva según un patrón identificado
    rosa.num_points_total = 25 * ((k_int - 3) / 2) + 50;
    rosa.num_points = 2 * rosa.num_points_total;
    
    // Genera un valor aleatorio para la velocidad de rotación Entre 0.0001f y 0.0005f
    rosa.rotation_speed = 0.0001f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (0.0005f - 0.0001f)));
    
    // Verifica que el valor de la velocidad de rotación sea válido
    if (rosa.rotation_speed < 0.0001f || rosa.rotation_speed > 0.0005f) 
    {
        std::cerr << "WARNING: Se generó un valor inesperado para la velocidad de rotación. Se establecerá la velocidad en 0.0003f por defecto." << std::endl;
        rosa.rotation_speed = 0.0003f;
    }

    // Devuelve la rosa generada
    return rosa;
}


/**
 * Función principal.
 * Inicializa SDL, procesa argumentos, genera las rosas y las dibuja en la pantalla
 * con movimiento y velocidad de rotación de forma secuencial.
 * 
 * @param argc Cantidad de argumentos
 * @param argv Arreglo de argumentos
 * @return int Estado de salida
 */
int main(int argc, char* argv[]) 
{
    srand(time(nullptr)); // Inicialización de la semilla aleatoria

    // Procesa argumentos
    int quantity = parseArguments(argc, argv);

    // Inicialización de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cerr << "ERROR: SDL no pudo inicializarse! SDL_Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Crea la ventana y el renderizador de SDL
    SDL_Window* window = SDL_CreateWindow("Curvas de Rosa Polar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) 
    {
        std::cerr << "ERROR: La ventana no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit(); // Liberar recursos de SDL
        exit(EXIT_FAILURE);
    }

    // Crea el renderizador de SDL
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) 
    {
        std::cerr << "ERROR: El renderizador no pudo crearse! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window); // Liberar recursos de la ventana
        SDL_Quit(); // Liberar recursos de SDL
        exit(EXIT_FAILURE);
    }

    // Genera las rosas según la cantidad especificada y los ángulos de rotación iniciales
    std::list<RosaPolar> rosas;
    for (int i = 0; i < quantity; ++i) 
    {
        rosas.push_back(generateRosaPolar());
    }

    // Bandera para salir del ciclo principal
    bool quit = false;

    // Evento de SDL
    SDL_Event e;

    // Lista de ángulos de rotación iniciales
    std::list<float> rotation_angles(quantity, 0.0f);

    // Variables para calcular los FPS
    Uint32 frameCount = 0;              // Contador de frames
    Uint32 lastTime = SDL_GetTicks();   // Tiempo de inicio
    Uint32 fpsStartTime = lastTime;     // Inicializa el tiempo de inicio para FPS
    std::list<float> fpsHistory;        // Historial de FPS

    // Ciclo principal
    while (!quit) 
    {
        // Mientras haya eventos en la cola de eventos
        while (SDL_PollEvent(&e) != 0) 
        {
            // Verifica si se cerró la ventana
            if (e.type == SDL_QUIT) 
            {
                // Establece la bandera para salir del ciclo
                quit = true;
            }
        }

        // Limpia la pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dibuja cada rosa en la pantalla
        auto angle_it = rotation_angles.begin();

        for (auto& rosa : rosas) 
        {
            drawMovingPoints(renderer, rosa, *angle_it);
            *angle_it += rosa.rotation_speed;  // Actualiza el ángulo de rotación para la rosa
            ++angle_it;
        }

        // Presenta la escena
        SDL_RenderPresent(renderer);

        // Calcula y muestra los FPS
        frameCount++;
        Uint32 currentTime = SDL_GetTicks();

        // Actualiza los FPS cada segundo
        if (currentTime - fpsStartTime >= 1000) 
        {
            float fps = frameCount / ((currentTime - fpsStartTime) / 1000.0f);  // Calcula los FPS
            fps = std::round(fps * 100) / 100.0f;  // Aproximar a dos decimales

            fpsHistory.push_back(fps);  // Guarda el FPS procesado en el historial

            // Actualiza el título de la ventana con los FPS
            std::string fpsTitle = "Curvas de Rosa Polar - FPS: " + std::to_string(fps);
            SDL_SetWindowTitle(window, fpsTitle.c_str());

            // Reinicia el contador de frames y el tiempo de inicio
            frameCount = 0;
            fpsStartTime = currentTime;
        }

        // Actualiza el tiempo
        lastTime = currentTime;
    }

    // Generar métricas al final
    if (!fpsHistory.empty()) 
    {
        // Calcula las métricas de FPS
        float avgFPS = std::accumulate(fpsHistory.begin(), fpsHistory.end(), 0.0f) / fpsHistory.size();
        float minFPS = *std::min_element(fpsHistory.begin(), fpsHistory.end());
        float maxFPS = *std::max_element(fpsHistory.begin(), fpsHistory.end());
        fpsHistory.sort();
        auto fps1PercentLow_it = std::next(fpsHistory.begin(), fpsHistory.size() / 100); // 1% low
        float fps1PercentLow = *fps1PercentLow_it;

        // Guarda el informe de métricas en un archivo
        std::ofstream fpsReport("reports/seq_report.txt");
        
        // Verifica si se pudo abrir el archivo
        if (fpsReport.is_open()) 
        {
            // Escribe el informe de métricas en el archivo
            fpsReport << "Curvas de Rosa Polar Secuencial" << std::endl;
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
        std::cout << "Reporte de métricas guardado en seq_report.txt." << std::endl;
    }

    // Limpia y cierra
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
