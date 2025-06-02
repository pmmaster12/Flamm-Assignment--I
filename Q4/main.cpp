#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

// Shader sources
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(normalMatrix) * aNormal;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* sunFragmentShader = R"(
#version 330 core
out vec4 FragColor;

uniform float time;
in vec3 FragPos;
in vec3 Normal;

void main() {
    // Pulsing glow effect
    float pulse = 0.8 + 0.2 * sin(time * 3.0);
    
    // Distance from center for radial gradient
    float dist = length(Normal);
    float glow = 1.0 - smoothstep(0.0, 1.0, dist);
    
    vec3 sunColor = vec3(1.0, 0.8, 0.2) * pulse;
    FragColor = vec4(sunColor * (0.9 + glow * 0.5), 1.0);
}
)";

const char* planetFragmentShader = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 planetColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Ambient lighting
    vec3 ambient = 0.3 * planetColor;
    
    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * planetColor;
    
    // Simple specular highlight
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = vec3(0.3) * spec;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)";

class Shader {
public:
    unsigned int ID;
    
    Shader(const char* vertexSource, const char* fragmentSource) {
        // Compile vertex shader
        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexSource, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        
        // Compile fragment shader
        unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentSource, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        // Link shaders
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    void use() {
        glUseProgram(ID);
    }
    
    void setMat4(const std::string &name, const glm::mat4 &mat) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setVec3(const std::string &name, const glm::vec3 &value) {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    
    void setFloat(const std::string &name, float value) {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

private:
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }
};

class Sphere {
public:
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    Sphere(float radius = 1.0f, int sectors = 36, int stacks = 18) {
        generateSphere(radius, sectors, stacks);
        setupMesh();
    }
    
    void draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    ~Sphere() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

private:
    void generateSphere(float radius, int sectors, int stacks) {
        float x, y, z, xy;
        float nx, ny, nz, lengthInv = 1.0f / radius;
        float sectorStep = 2 * M_PI / sectors;
        float stackStep = M_PI / stacks;
        float sectorAngle, stackAngle;
        
        for (int i = 0; i <= stacks; ++i) {
            stackAngle = M_PI / 2 - i * stackStep;
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);
            
            for (int j = 0; j <= sectors; ++j) {
                sectorAngle = j * sectorStep;
                
                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);
                
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
                
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);
            }
        }
        
        int k1, k2;
        for (int i = 0; i < stacks; ++i) {
            k1 = i * (sectors + 1);
            k2 = k1 + sectors + 1;
            
            for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                
                if (i != (stacks - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
    }
    
    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
    }
};

struct CelestialBody {
    glm::vec3 position;
    glm::vec3 color;
    float radius;
    float orbitRadius;
    float orbitSpeed;
    float rotationSpeed;
    float orbitAngle;
    float rotationAngle;
    
    CelestialBody(glm::vec3 col, float r, float orbR, float orbSpd, float rotSpd) 
        : color(col), radius(r), orbitRadius(orbR), orbitSpeed(orbSpd), 
          rotationSpeed(rotSpd), orbitAngle(0), rotationAngle(0) {}
    
    void update(float deltaTime) {
        orbitAngle += orbitSpeed * deltaTime;
        rotationAngle += rotationSpeed * deltaTime;
        
        position.x = orbitRadius * cos(orbitAngle);
        position.z = orbitRadius * sin(orbitAngle);
        position.y = 0.0f;
    }
};

class SolarSystem {
private:
    GLFWwindow* window;
    Shader* sunShader;
    Shader* planetShader;
    Sphere* sphere;
    
    // Camera
    glm::vec3 cameraPos;
    glm::vec3 cameraTarget;
    glm::vec3 cameraUp;
    float cameraDistance;
    float cameraAngleX, cameraAngleY;
    
    // Mouse control
    bool firstMouse;
    double lastX, lastY;
    bool mousePressed;
    
    // Celestial bodies
    std::vector<CelestialBody> planets;
    CelestialBody* moon;
    
    // Time
    float currentTime;
    float deltaTime;
    float lastFrame;

public:
    SolarSystem() : firstMouse(true), mousePressed(false), cameraDistance(15.0f), 
                    cameraAngleX(0.0f), cameraAngleY(0.0f), currentTime(0.0f), 
                    deltaTime(0.0f), lastFrame(0.0f) {
        
        cameraPos = glm::vec3(0.0f, 5.0f, 15.0f);
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        
        // Initialize planets
        planets.push_back(CelestialBody(glm::vec3(0.8f, 0.3f, 0.3f), 0.8f, 4.0f, 2.0f, 5.0f)); // Mars-like
        planets.push_back(CelestialBody(glm::vec3(0.3f, 0.5f, 0.8f), 1.2f, 7.0f, 1.0f, 3.0f)); // Earth-like
        
        // Moon for the second planet (Earth-like)
        moon = new CelestialBody(glm::vec3(0.7f, 0.7f, 0.7f), 0.3f, 2.0f, 8.0f, 10.0f);
    }
    
    bool initialize() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        window = glfwCreateWindow(1200, 800, "Solar System OpenGL", NULL, NULL);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        
        // Set callbacks
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetScrollCallback(window, scrollCallback);
        
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            return false;
        }
        
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
        
        // Create shaders
        sunShader = new Shader(vertexShaderSource, sunFragmentShader);
        planetShader = new Shader(vertexShaderSource, planetFragmentShader);
        
        // Create sphere mesh
        sphere = new Sphere();
        
        return true;
    }
    
    void run() {
        while (!glfwWindowShouldClose(window)) {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            currentTime = currentFrame;
            
            processInput();
            update();
            render();
            
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    
    void cleanup() {
        delete sunShader;
        delete planetShader;
        delete sphere;
        delete moon;
        glfwTerminate();
    }

private:
    void processInput() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
    
    void update() {
        // Update planets
        for (auto& planet : planets) {
            planet.update(deltaTime);
        }
        
        // Update moon to orbit around the second planet (Earth-like)
        if (planets.size() > 1) {
            moon->orbitAngle += moon->orbitSpeed * deltaTime;
            moon->rotationAngle += moon->rotationSpeed * deltaTime;
            
            glm::vec3 planetPos = planets[1].position;
            moon->position.x = planetPos.x + moon->orbitRadius * cos(moon->orbitAngle);
            moon->position.z = planetPos.z + moon->orbitRadius * sin(moon->orbitAngle);
            moon->position.y = planetPos.y;
        }
        
        // Update camera position based on angles
        updateCamera();
    }
    
    void updateCamera() {
        cameraPos.x = cameraDistance * cos(cameraAngleY) * cos(cameraAngleX);
        cameraPos.y = cameraDistance * sin(cameraAngleX);
        cameraPos.z = cameraDistance * sin(cameraAngleY) * cos(cameraAngleX);
    }
    
    void render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set up matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                              (float)1200 / (float)800, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        
        // Render Sun
        sunShader->use();
        sunShader->setMat4("projection", projection);
        sunShader->setMat4("view", view);
        sunShader->setFloat("time", currentTime);
        
        glm::mat4 sunModel = glm::mat4(1.0f);
        sunModel = glm::scale(sunModel, glm::vec3(1.5f)); // Sun size
        sunShader->setMat4("model", sunModel);
        sunShader->setMat4("normalMatrix", glm::transpose(glm::inverse(sunModel)));
        
        sphere->draw();
        
        // Render Planets
        planetShader->use();
        planetShader->setMat4("projection", projection);
        planetShader->setMat4("view", view);
        planetShader->setVec3("lightPos", glm::vec3(0.0f, 0.0f, 0.0f)); // Sun position
        planetShader->setVec3("viewPos", cameraPos);
        
        for (const auto& planet : planets) {
            glm::mat4 planetModel = glm::mat4(1.0f);
            planetModel = glm::translate(planetModel, planet.position);
            planetModel = glm::rotate(planetModel, planet.rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
            planetModel = glm::scale(planetModel, glm::vec3(planet.radius));
            
            planetShader->setMat4("model", planetModel);
            planetShader->setMat4("normalMatrix", glm::transpose(glm::inverse(planetModel)));
            planetShader->setVec3("planetColor", planet.color);
            
            sphere->draw();
        }
        
        // Render Moon
        glm::mat4 moonModel = glm::mat4(1.0f);
        moonModel = glm::translate(moonModel, moon->position);
        moonModel = glm::rotate(moonModel, moon->rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        moonModel = glm::scale(moonModel, glm::vec3(moon->radius));
        
        planetShader->setMat4("model", moonModel);
        planetShader->setMat4("normalMatrix", glm::transpose(glm::inverse(moonModel)));
        planetShader->setVec3("planetColor", moon->color);
        
        sphere->draw();
    }
    
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
        SolarSystem* app = static_cast<SolarSystem*>(glfwGetWindowUserPointer(window));
        
        if (app->firstMouse) {
            app->lastX = xpos;
            app->lastY = ypos;
            app->firstMouse = false;
        }
        
        if (app->mousePressed) {
            double xoffset = xpos - app->lastX;
            double yoffset = app->lastY - ypos;
            
            app->lastX = xpos;
            app->lastY = ypos;
            
            float sensitivity = 0.01f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;
            
            app->cameraAngleY += xoffset;
            app->cameraAngleX += yoffset;
            
            // Constrain pitch
            if (app->cameraAngleX > 1.5f) app->cameraAngleX = 1.5f;
            if (app->cameraAngleX < -1.5f) app->cameraAngleX = -1.5f;
        }
    }
    
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        SolarSystem* app = static_cast<SolarSystem*>(glfwGetWindowUserPointer(window));
        
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                app->mousePressed = true;
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                app->lastX = xpos;
                app->lastY = ypos;
            } else if (action == GLFW_RELEASE) {
                app->mousePressed = false;
            }
        }
    }
    
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        SolarSystem* app = static_cast<SolarSystem*>(glfwGetWindowUserPointer(window));
        
        app->cameraDistance -= yoffset * 0.5f;
        if (app->cameraDistance < 3.0f) app->cameraDistance = 3.0f;
        if (app->cameraDistance > 50.0f) app->cameraDistance = 50.0f;
    }
};

int main() {
    SolarSystem app;
    
    if (!app.initialize()) {
        return -1;
    }
    
    app.run();
    app.cleanup();
    
    return 0;
}