#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    enum class ShaderType
    {
        NONE = -1,VERTEX =0,FRAGMENT =1
    };
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else {
            ss[(int)type] << line << "\n";
        }

    }
    return { ss[0].str(), ss[1].str() };
}



//编译着色器
static unsigned int CompileShader(unsigned int type, const std::string& source)//传入要编译的着色器类型，和着色器源代码
{
    unsigned int id = glCreateShader(type);//创建着色器对象，返回它的ID
    const char* src = source.c_str();//把传入的C++字符串转换成C风格的字符串
    glShaderSource(id, 1, &src, nullptr);//把源代码字符串转换成着色器的源代码，1表示只传入了一个字符串
    glCompileShader(id);//编译着色器
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);//查询编译状态，result是结果，成功是GL_TRUE,失败是GL_FALSE

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);//得到错误信息的长度
        char* message = (char*)alloca(length * sizeof(char));//分配一块儿内存，用来保存错误信息
        glGetShaderInfoLog(id, length, &length, message);//获取错误信息，保存到message中
        std::cout << "Failed to compile shader!" << (type==GL_VERTEX_SHADER?"vertex":"fragment")<<std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);//删除这个失败的着色器对象
        return 0;
    }
    return id;//返回这个着色器的"身份证号"
}
//创建着色器
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)//传入顶点和片段着色器的源代码
{
    unsigned int program = glCreateProgram();//创建一个空盒子用来装这两个着色器，返回这个盒子的身份证号
    unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);//编译这两个着色器，返回身份证号
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program, vs);//把两个着色器装进盒子
    glAttachShader(program, fs);//
    glLinkProgram(program);//把这两个着色器焊接在一起
    glValidateProgram(program);//验证这个程序是否合法
    glDeleteShader(vs);//删除这两个着色器，因为已经焊接在程序里了
    glDeleteShader(fs);
    return program;//返回这个这个程序的身份证号
}
int main(void)
{   //定义一个窗口变量
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
   

    /* Create a windowed mode window and its OpenGL context */
    //创建一个窗口，大小是640*480，标题是"Hello World"
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    //把这个窗口设为当前的 OpenGL 上下文（相当于告诉 OpenGL：我要在这个窗口上画图）
    glfwMakeContextCurrent(window);
    //初始化 GLEW（用来管理 OpenGL 的各种功能函数）
    if (glewInit() != GLEW_OK)
        std::cout << "chushihuashibai" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    //定义了三个二维点，组成一个三角形。这些点的坐标是 OpenGL 的标准坐标系（ - 1 到 1）。
    
    float positions[6] = { -0.5f,-0.5f,
                           0.0f,0.5f,
                            0.5f,-0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);//生成一个缓冲区对象（你可以理解为“内存盒子”），用来保存这些顶点数据。buffer 是这个盒子的编号。
    glBindBuffer(GL_ARRAY_BUFFER, buffer);//激活这个缓冲区盒子，准备往里面放东西。
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);//把 positions 里的顶点数据复制进缓冲区。GL_STATIC_DRAW 表示这些数据不会频繁修改。
    
    glEnableVertexAttribArray(0);//启用第 0 个顶点属性（你可以理解为“告诉 OpenGL，我有顶点数据要用了”）
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);//设置顶点属性的格式：每个点有 2 个 float 值（x, y），不归一化，步长是 2 个 float，从偏移 0 开始读。
    
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource,source.FragmentSource);//
    glUseProgram(shader);//使用这个着色器程序（相当于告诉 OpenGL：“我现在要用这个程序来画图了”）。
   
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);//清空屏幕（擦黑板）。
        glDrawArrays(GL_TRIANGLES, 0, 3);//画一个三角形，从第 0 个点开始，一共 3 个点。

        /* Swap front and back buffers */
        glfwSwapBuffers(window);//把画好的画面显示出来（双缓冲技术）

        /* Poll for and process events */
        glfwPollEvents();//处理窗口事件（比如点击关闭按钮）。
    }
   glDeleteShader(shader);

    glfwTerminate();
    return 0;
}