
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Fonts/Fonts.h"
#include "cxxopts.hpp"

#include "ImGuiApp.h"
#include "maps.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

ImFont* fontAAP;


// Macro to disable console on Windows
#if defined(_WIN32) && defined(APP_NO_CONSOLE)
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif


#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


/// Macro to request high performance GPU in systems (usually laptops) with both dedicated and discrete GPUs
#if defined(_WIN32)
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 0;
extern "C" __declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0;
#endif


ImVec4 ClearColor{1.0f,1.0f,1.0f,1.0f};                    // background clear color

GLFWwindow* Window;                   // GLFW window handle
std::map<std::string, ImFont*> Fonts;  // font map
bool UsingDGPU;
ImFont *fontFuturama, *fontVerdana;



#pragma region GLFWSupport 

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


ImVec2 GetWindowSize() 
{
    int w, h;
    glfwGetWindowSize(Window, &w, &h);
    return ImVec2((float)w, (float)h);
}

ImVec2 GetWindowPos()
{
    int w, h;
    glfwGetWindowPos(Window, &w, &h);
    return ImVec2((float)w, (float)h);
}

void SetWindowSize(ImVec2 s)
{
    glfwSetWindowSize(Window, (int)s.x, (int)s.y);   
}

void SetWindowPos(ImVec2 p)
{
    glfwSetWindowPos(Window, (int)p.x, (int)p.y);
}

void SetWindowToClose()
{
    glfwSetWindowShouldClose(Window,1);
}





bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}




bool LoadMonoTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 45; //was 0
    int image_height = 45; //was 0

    //////////////////
    //char* filename = "C:\\airbus4545.flat";
    FILE* in_file = fopen(filename, "rb");
    if (!in_file) {
        puts("fopen");
        return false;
        //exit(EXIT_FAILURE);
    }
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        puts("stat");
        return false;
        //exit(EXIT_FAILURE);
    }


    void* file_contents = malloc(sb.st_size);
    unsigned int sizeread = (unsigned int)fread(file_contents, sb.st_size, 1, in_file);
    std::cout << "sb.st_size is " << sb.st_size << std::endl;
    fclose(in_file);

    unsigned char* image_data = (unsigned char*)file_contents;
    //free(file_contents);
    ///////////////////

    //unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 1);
    //if (image_data == NULL) return false;


    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, image_width, image_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, image_data);

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);

    //stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;
    free(file_contents);
    return true;
}


#pragma endregion GLFWSupport





static void InitializeApp(std::string title, int w, int h, int argc, char const* argv[]) 
{
    cxxopts::Options options(title);
    options.add_options()
        ("v,vsync", "Disable V-Sync")
        ("m,msaa", "Enable MSAA")
        ("w,width", "Window width override", cxxopts::value<int>())
        ("h,height", "Window height override", cxxopts::value<int>())
        ("g,gpu", "Use discrete GPU on hybrid laptops")
        ("help", "Show Help");

    auto result = options.parse(argc, argv);
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        std::exit(0);
    }

    if (result.count("width"))      w = result["width"].as<int>();
    if (result.count("height"))     h = result["height"].as<int>();

    const bool no_vsync = result["vsync"].as<bool>();
    const bool use_msaa = result["msaa"].as<bool>();
    
    NvOptimusEnablement = AmdPowerXpressRequestHighPerformance = result["gpu"].as<bool>();
    UsingDGPU = result["gpu"].as<bool>();


    #ifdef _DEBUG
        title += " - GLFW/OpenGL - Debug";
    #else
        title += " - GLFW/OpenGL";
    #endif

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())  abort();

    // Decide GL+GLSL versions
    #if __APPLE__
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);           // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //glfwWindowHint(GLFW_POSITION_X, 10);
        //glfwWindowHint(GLFW_POSITION_Y, 20);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);          // 3.0+ only
    #endif


    if (use_msaa) {
        title += " - 4X MSAA";
        glfwWindowHint(GLFW_SAMPLES, 4);
        puts("Using MSAA");
    }

    // Create window with graphics context
    //printf("Create windows with w/h: %d %d \r\n", w, h);
    Window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
    if (Window == NULL)
    {
        fprintf(stderr, "Failed to initialize GLFW window!\n");
        abort();
    }
    glfwMakeContextCurrent(Window);
    glfwSwapInterval(no_vsync ? 0 : 1);

    // Initialize OpenGL loader
    bool err = gladLoadGL() == 0;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        abort();
    }
    
    title = "RadarSites - " + title;
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);

    title += " - ";
    title += reinterpret_cast<char const*>(renderer);

    static const char* label = " - Build DTG- " __DATE__ " " __TIME__;
    title += label;

    glfwSetWindowTitle(Window, title.c_str());



    //setup app icon
    bool retVal = std::filesystem::exists("radar.png");
    if (retVal)
    {
        GLFWimage images[1]{};
        int comp;
        images[0].pixels = stbi_load("radar.png", &images[0].width, &images[0].height, &comp, STBI_rgb_alpha); //rgba channels 
        //images[0].pixels = stbi_load("radar.png", &images[0].width, &images[0].height, &comp, STBI_rgb); //rgba channels 
        //printf("width, height  %d %d\r\n", images[0].width, images[0].height);
        glfwSetWindowIcon(Window, 1, images);
        stbi_image_free(images[0].pixels);
    }



    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (use_msaa)
        glEnable(GL_MULTISAMPLE);

    ImGui::StyleColorsLight();


    //ADDED by BRAD for Docking Multi Viewports

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }


    //Brad G's style tweaks
    style.WindowRounding = 4.0f;
    style.ScrollbarRounding = 0.0f;
    style.FrameBorderSize = 1.0f;
    style.FrameRounding = 5.0f;

    //END of section ADDED by Brad


}


static void ShutDownApp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(Window);
    glfwTerminate();
}



static bool LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = 14.0f;
    icons_config.GlyphOffset = ImVec2(0, 0);
    icons_config.OversampleH = 1;
    icons_config.OversampleV = 1;
    icons_config.FontDataOwnedByAtlas = false;

    static const ImWchar fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    
    ImStrncpy(font_cfg.Name, "Roboto Regular 17", 40);
    Fonts[font_cfg.Name] = io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_len, 17.0f, &font_cfg);
    io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 16.0f, &icons_config, fa_ranges);


    ImStrncpy(font_cfg.Name, "Roboto Regular 15", 40);
    Fonts[font_cfg.Name] = io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_len, 15.0f, &font_cfg);
    io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 14.0f, &icons_config, fa_ranges);

    ImStrncpy(font_cfg.Name, "Roboto Regular 20", 40);
    Fonts[font_cfg.Name] = io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_len, 20.0f, &font_cfg);
    io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 19.0f, &icons_config, fa_ranges);
    
    ImStrncpy(font_cfg.Name, "Roboto Regular 25", 40);
    Fonts[font_cfg.Name] = io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_len, 25.0f, &font_cfg);
    io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 24.0f, &icons_config, fa_ranges);

   
    if (std::filesystem::exists("fonts//verdana.ttf"))
    {
        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//verdana.ttf", 15.0f);
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 14.0f, &icons_config, fa_ranges);

        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//verdana.ttf", 18.0f);
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 17.0f, &icons_config, fa_ranges);


        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//verdana.ttf", 20.0f);
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 19.0f, &icons_config, fa_ranges);


        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//verdana.ttf", 25.0f);
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 24.0f, &icons_config, fa_ranges);
    }
    if (std::filesystem::exists("fonts//16020_FUTURAM.ttf"))
    {
        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//16020_FUTURAM.ttf", 15.0f);
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 14.0f, &icons_config, fa_ranges);

        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//16020_FUTURAM.ttf", 18.0f);
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 17.0f, &icons_config, fa_ranges);
    }
    
    if (std::filesystem::exists("fonts//APP6A05.ttf"))
    {
        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//APP6A05.TTF", 25.0f);
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 14.0f, &icons_config, fa_ranges);
    }

    //fontAAP = io.Fonts->AddFontFromFileTTF("APP6A01.TTF", 17.0f);
    //IM_ASSERT(fontAAP != NULL);
    //Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("APP6A01.TTF", 15.0f);


    return false;//mo errors
}




int main(int argc, char const* argv[])
{
    Maps m_map; //m_map is an ImGuiApp

    InitializeApp("ImMaps", 960, 540, argc, argv);
    ImGui::StyleColorsLight();

    LoadFonts();

    m_map.InitializeApp();

    // Main loop
    while (!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_map.UpdateApp();
        
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(Window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //added by Brad G for multi viewports

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            //glfwMakeContextCurrent(Window);
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }




        glfwSwapBuffers(Window);
    }

    m_map.ShutDownApp();

    ShutDownApp();
}
