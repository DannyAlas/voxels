#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include "render/renderer.h"
#include "render/camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

void checkErrors() {
        GLenum code;
        while ((code = glGetError()) != GL_NO_ERROR) {
            std::cerr << "GLERROR[" << code << "] ";
            switch (code) {
            case GL_INVALID_ENUM:
                std::cerr << "Invalid enum";
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "Invalid operation";
                break;

            default:
                // display the actual code
                std::cerr << "Unknown error";
                break;
            }
            std::cerr << std::endl;
            exit(EXIT_FAILURE);
        }
};


class App {
private:
    GLFWwindow* m_window;
    std::unique_ptr<Renderer> m_renderer;
    double m_lastTime;
    double m_currentTime;
    ImGuiIO io;
    bool firstMouse = true;
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;


public:
    App() {
        m_window = initWindow("OpenGL", SCR_WIDTH, SCR_HEIGHT);
        m_renderer = std::make_unique<Renderer>();
       };
    ~App() {    };

    GLFWwindow* initWindow(const std::string &title, int width, int height) {
        
        if (!glfwInit()) {
            std::cerr << "GLFW failed to initialize." << std::endl;
            exit(EXIT_FAILURE);
        }

        glfwSetErrorCallback([](int code, const char *error) {
            std::cerr << "[" << code << "] " << error << std::endl;
        });

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // vsync 
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
            App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
            app->processMouseMovement(window, xpos, ypos);
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
            app->processMouseClick(button, action);
        });

        glfwSetWindowUserPointer(window, this);


        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            exit(EXIT_FAILURE);
        }
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 450");
        bool show_demo_window = true;


        std::cout << glGetString(GL_VERSION) << std::endl;
        return window;
    };

    void processInput(GLFWwindow *window, float deltaTime) {
        if (io.WantCaptureKeyboard) {
            return;
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            // release mouse or unreleased mouse
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_renderer->camera()->processKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_renderer->camera()->processKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_renderer->camera()->processKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_renderer->camera()->processKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            m_renderer->camera()->processKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            m_renderer->camera()->processKeyboard(DOWN, deltaTime);
        

    };

    void processMouseMovement(GLFWwindow* window, double xpos, double ypos) {
        if (io.WantCaptureMouse) {
            return;
        }
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; 
        lastX = xpos;
        lastY = ypos;
        m_renderer->camera()->processMouseMovement(xoffset, yoffset);
    };

    void processMouseClick(int button, int action) {
        if (io.WantCaptureMouse) {
            return;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            
        }
    };
    
    void imguiRender() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Render Data");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        // camera data pos is  glm::vec3
        ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", m_renderer->camera()->position.x, m_renderer->camera()->position.y, m_renderer->camera()->position.z);
        ImGui::Text("Camera Direction: (%.2f, %.2f, %.2f)", m_renderer->camera()->front.x, m_renderer->camera()->front.y, m_renderer->camera()->front.z);
        ImGui::Text("Camera Up: (%.2f, %.2f, %.2f)", m_renderer->camera()->up.x, m_renderer->camera()->up.y, m_renderer->camera()->up.z);
        ImGui::Text("Camera Right: (%.2f, %.2f, %.2f)", m_renderer->camera()->right.x, m_renderer->camera()->right.y, m_renderer->camera()->right.z);
        ImGui::Text("Camera Yaw: %.2f", m_renderer->camera()->yaw);
        ImGui::Text("Camera Pitch: %.2f", m_renderer->camera()->pitch);
        ImGui::Text("Camera Movement Speed: %.2f", m_renderer->camera()->movementSpeed);
        ImGui::Text("Camera Mouse Sensitivity: %.2f", m_renderer->camera()->mouseSensitivity);
        ImGui::End();
        ImGui::Render();
    };

    void run() {
        m_lastTime = glfwGetTime();
        while (!glfwWindowShouldClose(m_window)) {
            m_currentTime = glfwGetTime();
            float deltaTime = m_currentTime - m_lastTime;
            m_lastTime = m_currentTime;
            glfwPollEvents();
            processInput(m_window, deltaTime);
            imguiRender();
            m_renderer->render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(m_window);
            checkErrors();
        }
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(m_window);
        glfwTerminate();
        exit(EXIT_SUCCESS);
    };
    
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    };
};


int main() {
    auto app = App();
    app.run();
    return 0;
}

