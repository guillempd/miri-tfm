#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

static void error_callback(int error_code, const char* description)
{
    std::cerr << "[glfw] E(" << error_code << "): " << description << std::endl;
}

void SetupFullScreenQuadBuffers(GLuint& vao, GLuint& vbo)
{
    float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW); // TODO: Is this with &quadVertices (?)

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // TODO: Be careful with the offset
}

void SetupShaders(GLuint& program)
{
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    out vec2 texCoord;
    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        texCoord = 0.5 * (aPos.xy + vec2(1.0));
    }
)";

    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 texCoord;
    out vec4 FragColor;
    uniform sampler2D hdrImage;
    void main()
    {
        FragColor = texture(hdrImage, texCoord);
    }
)";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[256];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << infoLog << std::endl;
    }
}

void LoadImageTexture(GLuint& texture)
{
    int x, y, n;
    float* data = stbi_loadf("D:/Descargas/drackenstein_quarry_4k.hdr", &x, &y, &n, 0);

    if (data)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "[stbi] E: Could not load image." << std::endl;
    }
}

void Render(GLuint vao, GLuint program, GLuint texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "hdrImage"), 0);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main()
{
    // Initialize glfw
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        return -1;
    }

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "miri-tfm", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize glad
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "[glad] E: Failed to initalize." << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 1024, 768);

    // Initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    stbi_set_flip_vertically_on_load(true);

    GLuint vao, vbo;
    SetupFullScreenQuadBuffers(vao, vbo);
    GLuint program;
    SetupShaders(program);
    GLuint texture;
    LoadImageTexture(texture);

    float clearColor[3] = { 1.0f, 1.0f, 1.0f };

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // Render code
        ImGui::ShowDemoWindow();

        if (ImGui::Begin("Color picker"))
        {
            ImGui::ColorEdit3("Clear Color", clearColor);
        }
        ImGui::End();

        glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Render(vao, program, texture);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }


    // Terminate
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
