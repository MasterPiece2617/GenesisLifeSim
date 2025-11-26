# Project Simulator

Es un proyecto sobre simular el ciclo de vida y evolución de distintas especies de seres digitales con características únicas-

## Herramientas de trabajo

-   C++ será el lenguaje de programación utilizado.
-   GitHub será la plataforma de control de versiones y colaboración.
-   Discord será la plataforma principal de comunicación del equipo, por favor estar al pendiente.
-   Utilizaremos visual estudio 2022 como IDE, el proyecto ya ha sido configurado para su compilación inmediata en sistemas Windows de 64bits.
-   SFML será la bibliotecas gráfica utilizada.
-   Usaremos distintos archivos.md en la carpeta docs para documentar el proyecto y sus módulos, esto es importante para mantener el órden y entendimiento de las diferentes funcionalidades.
-   El proyecto cuenta con un archivo CMakeList para poder ser compilado y ejecutado en otros sistemas operativos, las bibliotecas estan incluidas ya, solo se necesita el proyecto completo y nada más.


## Estilo de código

-   Se usarán 4 espacios para la indentación del código.
-   Los includes se organizarán en órden alfabético con el siguiente órden:
    -   Los includes de bibliotecas estándar de C++ se colocarán primero.
    -   Luego, los includes de bibliotecas externas como SFML.
    -   Finalmente, los includes de archivos locales del proyecto.
        Dejar espacios entre cada grupo de includes.
-   Para las declaraciones de variables y funciones, se utilizará la convención de nombres en minúsculas con guiones bajos para separar palabras. Por ejemplo: `mi_variable`, `mi_funcion()`.
-   Las clases se nombrarán en mayúsculas y minúsculas, comenzando con mayúscula. Por ejemplo: `MiClase`.
-   Las llaves se abrirán en la línea de abajo de la declaración del bloque y se cerrarán debajo de la última línea del bloque. Por ejemplo:

```cpp
if (condition)
{
	// Código aquí
}
```

-   Los argumentos en los constructores llevarán una barra baja al inicio del nombre para diferenciarlos de las variables locales. Por ejemplo:

```cpp
MiClase(int _mi_variable) : mi_variable(_mi_variable)
{
	// Código aquí
}
```

## Estructura del proyecto y sus módulos

Dentro de la carpeta raíz del proyecto, se encuentra la siguiente estructura de carpetas:

```
Root/
├── docs/ <-- Documentación del proyecto
├── libs/ <-- Bibliotecas externas utilizadas en el proyecto
	├── SFML/ <-- Biblioteca SFML
    ├── imgui
    ├── implot
    ├──FastNoiseLite
├── resources/ <-- Recursos del proyecto (imágenes, sonidos, etc.)
    ├── textures/ <-- Texturas del proyecto
    ├── maps/ <-- Mapas para simular
├── simulator/
│   ├── include/ <-- Archivos de cabecera del proyecto (.hpp)
│   ├── src/ <-- Archivos fuente del proyecto (.cpp)
│   ├── main.cpp <-- Archivo main del proyecto
```
## Compilación y Ejecución

### Utilizando CMake

 Ejecutar el siguiente comando para descargar los siguientes paquetes necesarios para continuar (Derivados de Debian):
 ```Bash
    sudo apt update && sudo apt install build-essential cmake
 ```
 Una vez hecho esto, ubicarse en el directorio raiz donde esta el proyecto y ejecutar los siguientes comandos en orden:
```Bash
    mkdir build && cd build
    cmake ..
    make
    ./my_app
 ```
Y de esta manera, compilaras y ejecutaras el proyecto en Linux si  problemas (Probado en WSL2 con Debian 12)

### Utilizando Visual Studio 2022

 Descargar el repositorio remoto y abrirlo en Visual Studio, está ya diseñado para ejecutarse una vez abierto en este IDE.

[Documentación](docs/intro_estructura.md)