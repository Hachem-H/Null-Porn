#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_PRIVATE

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <nuklear.h>

#include <DDoS.h>
#include <Log.h>

#define MAX_VERTEX_MEMORY  512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

typedef struct Vertex_t
{
    float position[2];
    float uv[2];
    uint8_t color[4];
} Vertex;

typedef struct Device_t
{
    struct nk_buffer commands;
    struct nk_draw_null_texture nullTexture;

    uint32_t vbo, vao, ebo;
    uint32_t fontTexture;

    uint32_t fragmentShader;
    uint32_t vertexShader;
    uint32_t program;

    int positionAttribute;
    int uvAttribute;
    int colorAttribute;

    int textureUniform;
    int projectionUniform;
} Device;

static inline void InitializeDevice(Device* device)
{
    int status;

    static const char* vertexShaderSource = 
        "#version 330 core\n"
        ""
        "uniform mat4 u_ProjectionMatrix;"
        ""
        "in vec2 a_Position;"
        "in vec2 a_TexCoord;"
        "in vec4 a_Color;"
        ""
        "out vec2 a_FragUV;"
        "out vec4 a_FragColor;"
        ""
        "void main()"
        "{"
        "   a_FragUV = a_TexCoord;"
        "   a_FragColor = a_Color;"
        "   gl_Position = u_ProjectionMatrix * vec4(a_Position.xy, 0, 1);"
        "}";

    static const char* fragmentShaderSource = 
        "#version 330 core\n"
        ""
        "precision mediump float;"
        ""
        "uniform sampler2D u_Texture;"
        ""
        "in vec2 a_FragUV;"
        "in vec4 a_FragColor;"
        ""
        "out vec4 OutColor;"
        ""
        "void main()"
        "{"
        "   OutColor = a_FragColor * texture(u_Texture, a_FragUV.st);"
        "}";

    device->program        = glCreateProgram();
    device->vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    device->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    nk_buffer_init_default(&device->commands);
    glShaderSource(device->vertexShader,   1, &vertexShaderSource,   0);
    glShaderSource(device->fragmentShader, 1, &fragmentShaderSource, 0);
    glCompileShader(device->vertexShader);
    glCompileShader(device->fragmentShader);

    glGetShaderiv(device->vertexShader,   GL_COMPILE_STATUS, &status); assert(status == GL_TRUE);
    glGetShaderiv(device->fragmentShader, GL_COMPILE_STATUS, &status); assert(status == GL_TRUE);

    glAttachShader(device->program, device->vertexShader);
    glAttachShader(device->program, device->fragmentShader);
    glLinkProgram(device->program);

    glGetProgramiv(device->program, GL_LINK_STATUS, &status); assert(status == GL_TRUE);

    device->textureUniform    = glGetUniformLocation(device->program, "u_Texture");
    device->projectionUniform = glGetUniformLocation(device->program, "u_ProjectionMatrix");
    device->positionAttribute = glGetAttribLocation(device->program, "a_Position");
    device->uvAttribute       = glGetAttribLocation(device->program, "a_TexCoord");
    device->colorAttribute    = glGetAttribLocation(device->program, "a_Color");

    size_t vertexPosition = offsetof(Vertex, position);
    size_t vertexTexture  = offsetof(Vertex, uv);
    size_t vertexColor    = offsetof(Vertex, color);

    glGenBuffers(     1, &device->vbo);
    glGenBuffers(     1, &device->ebo);
    glGenVertexArrays(1, &device->vao);
    
    glBindVertexArray(device->vao);
    glBindBuffer(GL_ARRAY_BUFFER, device->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, device->ebo);

    glEnableVertexAttribArray((uint32_t)device->positionAttribute);
    glEnableVertexAttribArray((uint32_t)device->colorAttribute);
    glEnableVertexAttribArray((uint32_t)device->uvAttribute);

    glVertexAttribPointer((uint32_t)device->positionAttribute, 2, GL_FLOAT,         false, sizeof(Vertex), (void*) vertexPosition);
    glVertexAttribPointer((uint32_t)device->uvAttribute,       2, GL_FLOAT,         false, sizeof(Vertex), (void*) vertexTexture);
    glVertexAttribPointer((uint32_t)device->colorAttribute,    4, GL_UNSIGNED_BYTE, true,  sizeof(Vertex), (void*) vertexColor);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static inline void ShutdownDevice(Device* device)
{
    glDetachShader(device->program, device->vertexShader);
    glDetachShader(device->program, device->fragmentShader);
    glDeleteShader(device->vertexShader);
    glDeleteShader(device->fragmentShader);
    glDeleteProgram(device->program);

    glDeleteTextures(1, &device->fontTexture);
    glDeleteBuffers(1, &device->vbo);
    glDeleteBuffers(1, &device->ebo);

    nk_buffer_free(&device->commands);
}

static inline void Device_UploadAtlas(Device* device, const void* image, int width, int height)
{
    glGenTextures(1, &device->fontTexture);
    glBindTexture(GL_TEXTURE_2D, device->fontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

static inline void Device_OnRender(Device* device, struct nk_context* context, int width, int height, enum nk_anti_aliasing AA)
{
    float orthographicMatrix[4][4] =
    {
        {  2.0f,  0.0f,  0.0f, 0.0f },
        {  0.0f, -2.0f,  0.0f, 0.0f },
        {  0.0f,  0.0f, -1.0f, 0.0f },
        { -1.0f,  1.0f,  0.0f, 1.0f },
    };

    orthographicMatrix[0][0] /= (float)width;
    orthographicMatrix[1][1] /= (float)height;

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    glUseProgram(device->program);
    glUniform1i(device->textureUniform, 0);
    glUniformMatrix4fv(device->projectionUniform, 1, false, &orthographicMatrix[0][0]);

    glBindVertexArray(device->vao);
    glBindBuffer(GL_ARRAY_BUFFER,         device->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, device->ebo);

    glBufferData(GL_ARRAY_BUFFER,         MAX_VERTEX_MEMORY, NULL, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_VERTEX_MEMORY, NULL, GL_STREAM_DRAW);

    void* vertices = glMapBuffer(GL_ARRAY_BUFFER,         GL_WRITE_ONLY);
    void* elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    
    static const struct nk_draw_vertex_layout_element vertexLayout[] =
    {
        { NK_VERTEX_POSITION, NK_FORMAT_FLOAT,    NK_OFFSETOF(Vertex, position) },
        { NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT,    NK_OFFSETOF(Vertex, uv)       },
        { NK_VERTEX_COLOR,    NK_FORMAT_R8G8B8A8, NK_OFFSETOF(Vertex, color)    },
        { NK_VERTEX_LAYOUT_END                                                  }
    };

    struct nk_convert_config config;
    NK_MEMSET(&config, 0, sizeof(config));

    config.vertex_layout        = vertexLayout;
    config.vertex_size          = sizeof(Vertex);
    config.vertex_alignment     = NK_ALIGNOF(Vertex);
    config.tex_null             = device->nullTexture;
    config.circle_segment_count = 22;
    config.curve_segment_count  = 22;
    config.arc_segment_count    = 22;
    config.global_alpha         = 1.0f;
    config.shape_AA             = AA;
    config.line_AA              = AA;

    struct nk_buffer verticesBuffer;
    struct nk_buffer elementsBuffer;

    nk_buffer_init_fixed(&verticesBuffer, vertices, MAX_VERTEX_MEMORY);
    nk_buffer_init_fixed(&elementsBuffer, elements, MAX_ELEMENT_MEMORY);
    nk_convert(context, &device->commands, &verticesBuffer, &elementsBuffer, &config);

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    
    const struct nk_draw_command* command;
    const nk_draw_index* offset = NULL;
    nk_draw_foreach(command, context, &device->commands)
    {
        if (!command->elem_count)
            continue;

        glBindTexture(GL_TEXTURE_2D, (uint32_t) command->texture.id);
        glScissor((int)(command->clip_rect.x),
                  (int)((height - (int)(command->clip_rect.y + command->clip_rect.h))),
                  (int)(command->clip_rect.w),
                  (int)(command->clip_rect.h));
        glDrawElements(GL_TRIANGLES, (int)command->elem_count, GL_UNSIGNED_SHORT, offset);
        offset += command->elem_count;
    }

    nk_clear(context);
    nk_buffer_clear(&device->commands);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

static inline void ErrorCallback(int error, const char* description)      { ERRROR("[GLFW ERROR %d]: %s.", error, description);                                              }
static inline void TextInput(GLFWwindow* window, unsigned int codepoint)  { nk_input_unicode((struct nk_context*)glfwGetWindowUserPointer(window), codepoint);               }
static inline void ScrollInput(GLFWwindow* window, double _, double yOff) { nk_input_scroll( (struct nk_context*)glfwGetWindowUserPointer(window), nk_vec2(0, (float)yOff)); }

static inline void DispatchInput(struct nk_context* context, GLFWwindow* window)
{
    nk_input_begin(context);
    glfwPollEvents();

    nk_input_key(context, NK_KEY_DEL,       glfwGetKey(window, GLFW_KEY_DELETE)    == GLFW_PRESS);
    nk_input_key(context, NK_KEY_ENTER,     glfwGetKey(window, GLFW_KEY_ENTER)     == GLFW_PRESS);
    nk_input_key(context, NK_KEY_TAB,       glfwGetKey(window, GLFW_KEY_TAB)       == GLFW_PRESS);
    nk_input_key(context, NK_KEY_BACKSPACE, glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
    nk_input_key(context, NK_KEY_LEFT,      glfwGetKey(window, GLFW_KEY_LEFT)      == GLFW_PRESS);
    nk_input_key(context, NK_KEY_RIGHT,     glfwGetKey(window, GLFW_KEY_RIGHT)     == GLFW_PRESS);
    nk_input_key(context, NK_KEY_UP,        glfwGetKey(window, GLFW_KEY_UP)        == GLFW_PRESS);
    nk_input_key(context, NK_KEY_DOWN,      glfwGetKey(window, GLFW_KEY_DOWN)      == GLFW_PRESS);

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)  == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) 
    {
        nk_input_key(context, NK_KEY_COPY,  glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);
        nk_input_key(context, NK_KEY_PASTE, glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);
        nk_input_key(context, NK_KEY_CUT,   glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS);
        nk_input_key(context, NK_KEY_CUT,   glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);
        nk_input_key(context, NK_KEY_SHIFT, 1);
    } 
    else 
    {
        nk_input_key(context, NK_KEY_COPY,  0);
        nk_input_key(context, NK_KEY_PASTE, 0);
        nk_input_key(context, NK_KEY_CUT,   0);
        nk_input_key(context, NK_KEY_SHIFT, 0);
    }

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    nk_input_motion(context,                   (int)x, (int)y);
    nk_input_button(context, NK_BUTTON_LEFT,   (int)x, (int)y, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)   == GLFW_PRESS);
    nk_input_button(context, NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    nk_input_button(context, NK_BUTTON_RIGHT,  (int)x, (int)y, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)  == GLFW_PRESS);

    nk_input_end(context);
}

int main()
{
    if (!glfwInit())
    {
        FATAL("Failed to initialize GLFW.");
        return -1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                                          "NullPorn", NULL, NULL);
    int width      = 0, height      = 0;
    int imageWidth = 0, imageHeight = 0;
    glfwMakeContextCurrent(window);

    struct nk_font_atlas fontAtlas;
    struct nk_context context;
    Device device;

    glfwSetErrorCallback(ErrorCallback);
    glfwSetWindowUserPointer(window, &context);
    glfwSetCharCallback(window, TextInput);
    glfwSetScrollCallback(window, ScrollInput);
    glfwGetWindowSize(window, &width, &height);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (glewInit() != GLEW_OK)
    {
        FATAL("Failed to initialize GLEW.");
        return -1;
    } 
    
    InitializeDevice(&device);
    nk_font_atlas_init_default(&fontAtlas);
    nk_font_atlas_begin(&fontAtlas);
    struct nk_font* font = nk_font_atlas_add_default(&fontAtlas, 13, 0);
    const void* image = nk_font_atlas_bake(&fontAtlas, &imageWidth, &imageHeight, NK_FONT_ATLAS_RGBA32);
    Device_UploadAtlas(&device, image, imageWidth, imageHeight);
    nk_font_atlas_end(&fontAtlas, nk_handle_id((int)device.fontTexture), &device.nullTexture);
    nk_init_default(&context, &font->handle);

    glEnable(GL_TEXTURE_2D);
    while (!glfwWindowShouldClose(window))
    {
        DispatchInput(&context, window);

        if (nk_begin(&context, "NullPorn", nk_rect(0, 0, width, height),
                     NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR))
        {
        } nk_end(&context);

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        Device_OnRender(&device, &context, width, height, NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(window);
    }

    nk_font_atlas_clear(&fontAtlas);
    nk_free(&context);

    ShutdownDevice(&device); 
    glfwDestroyWindow(window);
    glfwTerminate();
}
