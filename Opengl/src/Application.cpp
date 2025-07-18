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



//������ɫ��
static unsigned int CompileShader(unsigned int type, const std::string& source)//����Ҫ�������ɫ�����ͣ�����ɫ��Դ����
{
    unsigned int id = glCreateShader(type);//������ɫ�����󣬷�������ID
    const char* src = source.c_str();//�Ѵ����C++�ַ���ת����C�����ַ���
    glShaderSource(id, 1, &src, nullptr);//��Դ�����ַ���ת������ɫ����Դ���룬1��ʾֻ������һ���ַ���
    glCompileShader(id);//������ɫ��
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);//��ѯ����״̬��result�ǽ�����ɹ���GL_TRUE,ʧ����GL_FALSE

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);//�õ�������Ϣ�ĳ���
        char* message = (char*)alloca(length * sizeof(char));//����һ����ڴ棬�������������Ϣ
        glGetShaderInfoLog(id, length, &length, message);//��ȡ������Ϣ�����浽message��
        std::cout << "Failed to compile shader!" << (type==GL_VERTEX_SHADER?"vertex":"fragment")<<std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);//ɾ�����ʧ�ܵ���ɫ������
        return 0;
    }
    return id;//���������ɫ����"���֤��"
}
//������ɫ��
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)//���붥���Ƭ����ɫ����Դ����
{
    unsigned int program = glCreateProgram();//����һ���պ�������װ��������ɫ��������������ӵ����֤��
    unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);//������������ɫ�����������֤��
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program, vs);//��������ɫ��װ������
    glAttachShader(program, fs);//
    glLinkProgram(program);//����������ɫ��������һ��
    glValidateProgram(program);//��֤��������Ƿ�Ϸ�
    glDeleteShader(vs);//ɾ����������ɫ������Ϊ�Ѿ������ڳ�������
    glDeleteShader(fs);
    return program;//������������������֤��
}
int main(void)
{   //����һ�����ڱ���
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
   

    /* Create a windowed mode window and its OpenGL context */
    //����һ�����ڣ���С��640*480��������"Hello World"
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    //�����������Ϊ��ǰ�� OpenGL �����ģ��൱�ڸ��� OpenGL����Ҫ����������ϻ�ͼ��
    glfwMakeContextCurrent(window);
    //��ʼ�� GLEW���������� OpenGL �ĸ��ֹ��ܺ�����
    if (glewInit() != GLEW_OK)
        std::cout << "chushihuashibai" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    //������������ά�㣬���һ�������Ρ���Щ��������� OpenGL �ı�׼����ϵ�� - 1 �� 1����
    
    float positions[6] = { -0.5f,-0.5f,
                           0.0f,0.5f,
                            0.5f,-0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);//����һ��������������������Ϊ���ڴ���ӡ���������������Щ�������ݡ�buffer ��������ӵı�š�
    glBindBuffer(GL_ARRAY_BUFFER, buffer);//����������������ӣ�׼��������Ŷ�����
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);//�� positions ��Ķ������ݸ��ƽ���������GL_STATIC_DRAW ��ʾ��Щ���ݲ���Ƶ���޸ġ�
    
    glEnableVertexAttribArray(0);//���õ� 0 ���������ԣ���������Ϊ������ OpenGL�����ж�������Ҫ���ˡ���
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);//���ö������Եĸ�ʽ��ÿ������ 2 �� float ֵ��x, y��������һ���������� 2 �� float����ƫ�� 0 ��ʼ����
    
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource,source.FragmentSource);//
    glUseProgram(shader);//ʹ�������ɫ�������൱�ڸ��� OpenGL����������Ҫ�������������ͼ�ˡ�����
   
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);//�����Ļ�����ڰ壩��
        glDrawArrays(GL_TRIANGLES, 0, 3);//��һ�������Σ��ӵ� 0 ���㿪ʼ��һ�� 3 ���㡣

        /* Swap front and back buffers */
        glfwSwapBuffers(window);//�ѻ��õĻ�����ʾ������˫���弼����

        /* Poll for and process events */
        glfwPollEvents();//�������¼����������رհ�ť����
    }
   glDeleteShader(shader);

    glfwTerminate();
    return 0;
}