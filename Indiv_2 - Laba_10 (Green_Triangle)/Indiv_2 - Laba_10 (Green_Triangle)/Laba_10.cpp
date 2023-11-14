#include <iostream>
#include <vector>
#include <fstream>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

// ID шейдерной программы
GLuint Program;
// ID атрибута
GLint Attrib_vertex;
// ID Vertex Buffer Object
GLuint VBO;

struct Vertex {
    GLfloat x;
    GLfloat y;
};

// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
#version 330 core
in vec2 coord;
void main() {
gl_Position = vec4(coord, 0.0, 1.0);
}
)";

// Исходный код фрагментного шейдера
const char* FragShaderSource = R"(
#version 330 core
out vec4 color;
void main() {
color = vec4(0, 1, 0, 1);
}
)";

void InitShader();
void InitVBO();
void ReleaseShader();
void ReleaseVBO();


void Init() {
    // Шейдеры
    InitShader();
    // Вершинный буфер
    InitVBO();
}

void ShaderLog(unsigned int shader)
{
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1)
    {
        int charsWritten = 0;
        std::vector<char> infoLog(infologLen);
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
        std::cout << "InfoLog: " << infoLog.data() << std::endl;
    }
}

void InitVBO() {
    glGenBuffers(1, &VBO);
    // Вершины нашего треугольника
    Vertex triangle[3] = {
   { -0.5f, -0.5f },
   { 0.0f, 0.5f },
   { 0.5f, -0.5f }
    };
    // Передаем вершины в буфер
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    //checkOpenGLerror(); //Пример функции есть в лабораторной
    // Проверка ошибок OpenGL, если есть, то вывод в консоль тип ошибки
}

void InitShader() {
    // Создаем вершинный шейдер
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    // Передаем исходный код
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    // Компилируем шейдер
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    // Функция печати лога шейдера
    ShaderLog(vShader);
    // Создаем фрагментный шейдер
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Передаем исходный код
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    // Компилируем шейдер
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    // Функция печати лога шейдера
    ShaderLog(fShader);
    // Создаем программу и прикрепляем шейдеры к ней
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);
    // Линкуем шейдерную программу
    glLinkProgram(Program);
    // Проверяем статус сборки
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        std::cout << "error attach shaders \n";
        return;
    }
    // Вытягиваем ID атрибута из собранной программы
    const char* attr_name = "coord"; //имя в шейдере
    Attrib_vertex = glGetAttribLocation(Program, attr_name);
    if (Attrib_vertex == -1) {
        std::cout << "could not bind attrib " << attr_name << std::endl;
        return;
    }
    //checkOpenGLerror();
}

void Draw() {
    glUseProgram(Program); // Устанавливаем шейдерную программу текущей

    glEnableVertexAttribArray(Attrib_vertex); // Включаем массив атрибутов
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Подключаем VBO
    // Указывая pointer 0 при подключенном буфере, мы указываем что данные в VBO
    glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Отключаем VBO
    glDrawArrays(GL_TRIANGLES, 0, 3); // Передаем данные на видеокарту(рисуем)
    glDisableVertexAttribArray(Attrib_vertex); // Отключаем массив атрибутов
    glUseProgram(0); // Отключаем шейдерную программу
    //checkOpenGLerror();
}

void Release() {
    // Шейдеры
    ReleaseShader();
    // Вершинный буфер
    ReleaseVBO();
}

// Освобождение буфера
void ReleaseVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);
}

// Освобождение шейдеров
void ReleaseShader() {
    // Передавая ноль, мы отключаем шейдерную программу
    glUseProgram(0);
    // Удаляем шейдерную программу
    glDeleteProgram(Program);
}

int main() {

    sf::Window window(sf::VideoMode(800, 800), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);
    window.setActive(true);
    glewInit();
    Init();
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); }
            else if (event.type == sf::Event::Resized) { glViewport(0, 0, event.size.width, event.size.height); }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw();
        window.display();
    }
    Release();
    return 0;
}

/*
void SetIcon(sf::Window& wnd);
void Init();
void Draw();


int main() {
    // Создаём окно
    sf::Window window(sf::VideoMode(1200, 1200), "My OpenGL window", sf::Style::Default, sf::ContextSettings(32));
    // Ставим иконку (окна с дефолтной картинкой это некрасиво)
    SetIcon(window);
    // Включаем вертикальную синхронизацию (синхронизация частоты отрисовки с частотой кадров монитора, чтобы картинка не фризила, делать это не обязательно)
    window.setVerticalSyncEnabled(true);

    // Активируем окно
    window.setActive(true);

    // Инициализация
    Init();

    // Главный цикл окна
    while (window.isOpen()) {
        sf::Event event;
        // Цикл обработки событий окна, тут можно ловить нажатия клавиш, движения мышки и всякое такое
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // Окно закрыто, выходим из цикла
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                // Изменён размер окна, надо поменять и размер области Opengl отрисовки
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // Очистка буферов
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Рисуем сцену
        Draw();

        // Отрисовывает кадр - меняет передний и задний буфер местами
        window.display();
    }

    return 0;
}

// В момент инициализации разумно произвести загрузку текстур, моделей и других вещей
void Init() {
    // Очистка буфера тёмно жёлтым цветом
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Глобальные переменные это плохо, тут это сделано просто для примера
GLfloat rotate_z = 0;

// Функция непосредственно отрисовки сцены
void Draw() {
    rotate_z += 0.5;

    // Используем устаревшую функциональность установки единичной матрицы преобразования
    glLoadIdentity();

    // Используем устаревшую конструкцию glBegin-glEnd для рисования квадрата
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(0.0, 1.0, 0.0); glVertex2f(-0.5f, -0.5f);
    glColor3f(0.0, 1.0, 0.0); glVertex2f(0.0f, 0.5f);
    glColor3f(0.0, 1.0, 0.0); glVertex2f(0.5f, -0.5f);
    glEnd();

    // Отправляем набор команд отрисовываться
    glFlush();
}


void SetIcon(sf::Window& wnd)
{
    sf::Image image;

    // Вместо рисования пикселей, можно загрузить иконку из файла (image.loadFromFile("icon.png"))
    image.create(16, 16);
    for (int i = 0; i < 16; ++i)
        for (int j = 0; j < 16; ++j)
            image.setPixel(i, j, {
                (sf::Uint8)(i * 16), (sf::Uint8)(j * 16), 0 });

    wnd.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
}
*/


/*
using namespace sf;

int main()
{

    RenderWindow window(VideoMode(400, 400), L"Laba10", Style::Default);

    window.setVerticalSyncEnabled(true);

    CircleShape shape(100.f, 3);
    shape.setPosition(100, 100);
    shape.setFillColor(Color::Green);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear(Color::Black);
        window.draw(shape);
        window.display();
    }
    return 0;
}
*/