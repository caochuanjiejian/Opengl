#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include"Renderer.h"
#include"VertexBuffer.h"
#include"VertexBufferLayout.h"
#include"IndexBuffer.h"
#include"VertexArray.h"
#include"Shader.h"
#include"Texture.h"
#include "glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw_gl3.h"

int main(void)
{   //定义一个窗口变量
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
   




    //我要创建一个 OpenGL 版本为 3.3 的窗口，并且使用 现代核心模式（Core Profile），不用那些老的、被淘汰的功能。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);





   
    //创建一个窗口，大小是640*480，标题是"Hello World"
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    
    //把这个窗口设为当前的 OpenGL 上下文（相当于告诉 OpenGL：我要在这个窗口上画图）
    glfwMakeContextCurrent(window);



    //开启垂直同步
    glfwSwapInterval(1);





    //初始化 GLEW（用来管理 OpenGL 的各种功能函数）
    if (glewInit() != GLEW_OK)
        std::cout << "chushihuashibai" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;



    
    {
     

        float positions[] = { 100.0f,100.0f,0.0f,0.0f,//0
                               200.0f,100.0f,1.0f,0.0f,//1
                                200.0f,200.0f,1.0f,1.0f,//2
                                100.0f,200.0f,0.0f,1.0f//3

        };
        unsigned int indices[] = { 0,1,2,
                                  2,3,0 };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        
      


        VertexArray va;

        VertexBuffer vb(positions, 16* sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);


        va.AddBuffer(vb,layout);
       

       

        


        //
        
        IndexBuffer ib(indices, 6);

        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
        


        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
      


        Texture texture("res/textures/bobo.png");
        texture.Bind();

        shader.SetUniform1i("u_Texture", 0);

        //va.Unbind();
        //vb.Unbind();
        //ib.Unbind();
        //shader.Unbind();
        
        Renderer renderer;
        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        glm::vec3 translation(200, 200, 0);


        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
          
            renderer.Clear();
            ImGui_ImplGlfwGL3_NewFrame();
            glm::mat4 model = glm::translate(glm::mat4(1.0f),translation );
            glm::mat4 mvp = proj * view * model;
            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(va,ib,shader);
            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;
            {
                
                                        
                ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 960.0f);             
               
                

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
           
            glfwSwapBuffers(window);//把画好的画面显示出来（双缓冲技术）

            /* Poll for and process events */
            glfwPollEvents();//处理窗口事件（比如点击关闭按钮）。
        }
      
       
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}