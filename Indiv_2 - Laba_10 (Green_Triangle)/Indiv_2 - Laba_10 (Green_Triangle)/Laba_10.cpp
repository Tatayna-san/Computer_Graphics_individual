#include <iostream>
#include <vector>
#include <fstream>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

// ID ��������� ���������
GLuint Program;
// ID ��������
GLint Attrib_vertex;
// ID Vertex Buffer Object
GLuint VBO;

struct Vertex {
    GLfloat x;
    GLfloat y;
};

// �������� ��� ���������� �������
const char* VertexShaderSource = R"(
#version 330 core
in vec2 coord;
void main() {
gl_Position = vec4(coord, 0.0, 1.0);
}
)";

// �������� ��� ������������ �������
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
    // �������
    InitShader();
    // ��������� �����
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
    // ������� ������ ������������
    Vertex triangle[3] = {
   { -0.5f, -0.5f },
   { 0.0f, 0.5f },
   { 0.5f, -0.5f }
    };
    // �������� ������� � �����
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    //checkOpenGLerror(); //������ ������� ���� � ������������
    // �������� ������ OpenGL, ���� ����, �� ����� � ������� ��� ������
}

void InitShader() {
    // ������� ��������� ������
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    // �������� �������� ���
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    // ����������� ������
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    // ������� ������ ���� �������
    ShaderLog(vShader);
    // ������� ����������� ������
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // �������� �������� ���
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    // ����������� ������
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    // ������� ������ ���� �������
    ShaderLog(fShader);
    // ������� ��������� � ����������� ������� � ���
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);
    // ������� ��������� ���������
    glLinkProgram(Program);
    // ��������� ������ ������
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        std::cout << "error attach shaders \n";
        return;
    }
    // ���������� ID �������� �� ��������� ���������
    const char* attr_name = "coord"; //��� � �������
    Attrib_vertex = glGetAttribLocation(Program, attr_name);
    if (Attrib_vertex == -1) {
        std::cout << "could not bind attrib " << attr_name << std::endl;
        return;
    }
    //checkOpenGLerror();
}

void Draw() {
    glUseProgram(Program); // ������������� ��������� ��������� �������

    glEnableVertexAttribArray(Attrib_vertex); // �������� ������ ���������
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // ���������� VBO
    // �������� pointer 0 ��� ������������ ������, �� ��������� ��� ������ � VBO
    glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // ��������� VBO
    glDrawArrays(GL_TRIANGLES, 0, 3); // �������� ������ �� ����������(������)
    glDisableVertexAttribArray(Attrib_vertex); // ��������� ������ ���������
    glUseProgram(0); // ��������� ��������� ���������
    //checkOpenGLerror();
}

void Release() {
    // �������
    ReleaseShader();
    // ��������� �����
    ReleaseVBO();
}

// ������������ ������
void ReleaseVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);
}

// ������������ ��������
void ReleaseShader() {
    // ��������� ����, �� ��������� ��������� ���������
    glUseProgram(0);
    // ������� ��������� ���������
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
    // ������ ����
    sf::Window window(sf::VideoMode(1200, 1200), "My OpenGL window", sf::Style::Default, sf::ContextSettings(32));
    // ������ ������ (���� � ��������� ��������� ��� ���������)
    SetIcon(window);
    // �������� ������������ ������������� (������������� ������� ��������� � �������� ������ ��������, ����� �������� �� �������, ������ ��� �� �����������)
    window.setVerticalSyncEnabled(true);

    // ���������� ����
    window.setActive(true);

    // �������������
    Init();

    // ������� ���� ����
    while (window.isOpen()) {
        sf::Event event;
        // ���� ��������� ������� ����, ��� ����� ������ ������� ������, �������� ����� � ������ �����
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // ���� �������, ������� �� �����
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                // ������ ������ ����, ���� �������� � ������ ������� Opengl ���������
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // ������� �������
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ������ �����
        Draw();

        // ������������ ���� - ������ �������� � ������ ����� �������
        window.display();
    }

    return 0;
}

// � ������ ������������� ������� ���������� �������� �������, ������� � ������ �����
void Init() {
    // ������� ������ ���� ����� ������
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// ���������� ���������� ��� �����, ��� ��� ������� ������ ��� �������
GLfloat rotate_z = 0;

// ������� ��������������� ��������� �����
void Draw() {
    rotate_z += 0.5;

    // ���������� ���������� ���������������� ��������� ��������� ������� ��������������
    glLoadIdentity();

    // ���������� ���������� ����������� glBegin-glEnd ��� ��������� ��������
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(0.0, 1.0, 0.0); glVertex2f(-0.5f, -0.5f);
    glColor3f(0.0, 1.0, 0.0); glVertex2f(0.0f, 0.5f);
    glColor3f(0.0, 1.0, 0.0); glVertex2f(0.5f, -0.5f);
    glEnd();

    // ���������� ����� ������ ��������������
    glFlush();
}


void SetIcon(sf::Window& wnd)
{
    sf::Image image;

    // ������ ��������� ��������, ����� ��������� ������ �� ����� (image.loadFromFile("icon.png"))
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