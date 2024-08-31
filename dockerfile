# Usa una imagen base de Ubuntu
FROM ubuntu:latest

# Establece el directorio de trabajo dentro del contenedor
WORKDIR /usr/src/app

# Actualiza los paquetes y instala las dependencias necesarias
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    software-properties-common \
    build-essential \
    libsdl2-dev \
    x11-apps \
    libx11-dev
    
# Configura el runtime para X11
ENV XDG_RUNTIME_DIR=/tmp

# Instalación de Docker dentro del contenedor (si es necesario)
RUN curl -fsSL https://download.docker.com/linux/ubuntu/gpg | gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg && \
    echo "deb [arch=amd64 signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" | tee /etc/apt/sources.list.d/docker.list > /dev/null && \
    apt-get update && \
    apt-get install -y docker-ce docker-ce-cli containerd.io