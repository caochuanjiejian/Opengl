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
{   //����һ�����ڱ���
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
   




    //��Ҫ����һ�� OpenGL �汾Ϊ 3.3 �Ĵ��ڣ�����ʹ�� �ִ�����ģʽ��Core Profile����������Щ�ϵġ�����̭�Ĺ��ܡ�
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);





   
    //����һ�����ڣ���С��640*480��������"Hello World"
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    
    //�����������Ϊ��ǰ�� OpenGL �����ģ��൱�ڸ��� OpenGL����Ҫ����������ϻ�ͼ��
    glfwMakeContextCurrent(window);



    //������ֱͬ��
    glfwSwapInterval(1);





    //��ʼ�� GLEW���������� OpenGL �ĸ��ֹ��ܺ�����
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
           
            glfwSwapBuffers(window);//�ѻ��õĻ�����ʾ������˫���弼����

            /* Poll for and process events */
            glfwPollEvents();//�������¼����������رհ�ť����
        }
      
       
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}