# Proyecto - Screen Saver
**Universidad del Valle de Guatemala**\
**Facultad de Ingeniería**\
**Departamento de Ciencias de la Computación**\
**Computación Paralela y Distribuida**

---

## Autores
- Diego Leiva
- Pablo Orellana

---
## Descripción
Este proyecto consiste en el desarrollo de un protector de pantalla (screen saver) que genera patrones visuales basados en las curvas polares conocidas como Rosa Polar o Curva Rhodoneas. El proyecto incluye dos versiones del algoritmo: una implementación secuencial y otra paralela, utilizando la biblioteca SDL para el renderizado gráfico y OpenMP para la paralelización.

## Objetivos
- Implementar y diseñar un programa para la paralelización de procesos con memoria compartida usando OpenMP
- Aplicar el método PCAM y los conceptos de patrones de descomposición y programación para modificar un programa secuencial y volverlo paralelo
- Realizar mejoras y modificaciones iterativas al programa para obtener mejores versiones.

---
## Ejemplo del Descansa Pantallas
![alt text](preview.png)

## Configuración e Instalación
Antes de ejecutar el proyecto, es importante asegurarse de que tu entorno esté correctamente configurado. A continuación se detallan los requisitos necesarios según el sistema operativo y el método de ejecución que seleccione (Windows o Linux).

### 1. Clonar el Repositorio
Comience por clonar el repositorio de GitHub:
```bash
https://github.com/LeivaDiego/ScreenSaver.git
```

### 2. Sistema Windows
Si estás utilizando Windows y deseas ejecutar este proyecto, a continuación se presentan los pasos detallados para configurar un entorno en tu máquina utilizando WSL2.

#### 2.1. Prerequisitos
- `WSL2`: (Requerido) Para poder utilizar este proyecto desde su equpipo windows, y que su experiencia sea más fluida y sin problemas, es necesario tener WSL2 instalado en tu equipo.

    ##### 2.1.1. Instalando WSL2
    Abra PowerShell o el símbolo del sistema de Windows como administrador; para ello, haga clic con el botón derecho y seleccione "**Ejecutar como administrador**", escriba el comando:
    ```powershell
    wsl --install
    ```
    Una vez completado el proceso reinicie su máquina.

    ##### 2.1.2. Configurando WSL2
    Una vez instalado, configure su distribución de Linux (por ejemplo, Ubuntu) como predeterminada si no lo está. Puede hacer esto con el comando:
    ```powershell
    wsl --set-default <nombre_de_la_distribución>
    ```
    Sustituya `<nombre_de_la_distribucion>` por el nombre de la distribución que estés utilizando (como "Ubuntu").
    </br>
    
    **Verificar la instalación**
    Abra la terminal de WSL y ejecute:
    ```powershell
    wsl --list --verbose
    ```
    Esto debería mostrar las distribuciones instaladas y su versión (asegúrese de que sea WSL2).

    ##### 2.1.3. Dependencias
    - `build-essential`: Herramientas de desarrollo esenciales para compilar y construir el proyecto.
    - `libsdl2-dev`: La biblioteca necesaria para manejar gráficos en la aplicación
  
    


#### 2.2. Ejecutar el Proyecto en WSL2
Navegue al directorio del proyecto en la terminal de WSL navegue hasta el directorio `app` y ejecute los comandos:

##### 2.2.1 Version Secuencial    
```bash
./sequential -q <cantidad_de_rosas>
```

##### 2.2.2 Version Paralela
```bash
./parallel -q <cantidad_de_rosas>
```

Asegurese de colocar el argumento `-q` (quantity) y sustituya `<cantidad_de_rosas>` con la cantidad de rosas que desee mostrar en el descansa pantallas. 

### 3. Sistema Linux
Para usuarios de Linux que prefieren no utilizar contenedores y tienen acceso directo a un sistema Linux, la configuración es bastante sencilla. 

#### 3.1 Dependencias
- `build-essential`: Herramientas de desarrollo esenciales para compilar y construir el proyecto.
- `libsdl2-dev`: La biblioteca necesaria para manejar gráficos en la aplicación

    ##### 3.1.1. Instalando dependencias
    **Actualizar el sistema**
    Antes de instalar cualquier paquete, es recomendable actualizar la lista de paquetes:
    ```bash
    sudo apt-get update
    ```
    **Instalar herramientas de desarrollo**
    Instale las herramientas de desarrollo esenciales con el siguiente comando:
    ```bash
    sudo apt-get install -y build-essential
    ```
    **Instalar la biblioteca SDL2**
    Instale la bilbioteca SDL2 con el siguiente comando:
    ```bash
    sudo apt-get install -y libsdl2-dev
    ```

### 4. Ejecutando el Proyecto
El proyecto cuenta con 2 archivos con implementaciones paralela y secuencial del mismo descansa pantallas. 

#### 4.1. Navegar al directorio del proyecto:
Abra una terminal y navegue al directorio donde se encuentra su proyecto:
```bash
cd /ruta/a/su/proyecto
```

#### 4.2. Compilar el proyecto
Asegurese de compilar los archivos `parallel.cpp` y `sequential.cpp` antes de ejecutarlos:
```bash
g++ -fopenmp  <archivo>.cpp -o <nombre_del_ejecutable> -lSDL2
```

#### 4.3 Ejecutar la aplicación
Una vez compilado su archivo seleccionado, ejecute la aplicación con el siguiente comando:
```bash
./<nombre_del_ejecutable> -q <cantidad_de_rosas>
```
Es necesario que ingrese el argumento `-q` (quantity) para definir la cantidad de Rosas Polares desea que se generen en el descansa pantallas. Sino lo define el programa no se ejecutara.


### 5. Recomendaciones
Dependiendo de la potencia de su equipo se recomienda que experimente con valores pequeños para la cantidad de rosas a graficar, esto para evitar que su equipo se congele debido a la carga de procesamiento.
