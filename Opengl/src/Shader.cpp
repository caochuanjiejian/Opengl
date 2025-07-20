#include<unordered_map>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include "Shader.h"
#include"Renderer.h"


Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath),m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

 ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);//打开指定路径的文件，准备读取内容
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
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
 unsigned int Shader::CompileShader(unsigned int type, const std::string& source)//传入要编译的着色器类型，和着色器源代码
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
         std::cout << "Failed to compile shader!" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
         std::cout << message << std::endl;
         glDeleteShader(id);//删除这个失败的着色器对象
         return 0;
     }
     return id;//返回这个着色器的"身份证号"
 }
  unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)//传入顶点和片段着色器的源代码
 {
     GLCall(unsigned int program = glCreateProgram());//创建一个空盒子用来装这两个着色器，返回这个盒子的身份证号
     unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);//编译这两个着色器，返回身份证号
     unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
     glAttachShader(program, vs);//把两个着色器装进盒子
     glAttachShader(program, fs);//
     glLinkProgram(program);//把这两个着色器焊接在一起
     glValidateProgram(program);//验证这个程序是否合法
     glDeleteShader(vs);//删除这两个着色器，因为已经焊接在程序里了
     glDeleteShader(fs);
     return program;//返回这个这个程序的身份证号
 }

void Shader::Bind()const
{
    glUseProgram(m_RendererID);
}
void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}
void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
     int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cout << "Warning:uniform '" << name << "'doesn't exist!" << std::endl;
    
    m_UniformLocationCache[name] = location;
    return location;
}
