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
    std::ifstream stream(filepath);//��ָ��·�����ļ���׼����ȡ����
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
 unsigned int Shader::CompileShader(unsigned int type, const std::string& source)//����Ҫ�������ɫ�����ͣ�����ɫ��Դ����
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
         std::cout << "Failed to compile shader!" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
         std::cout << message << std::endl;
         glDeleteShader(id);//ɾ�����ʧ�ܵ���ɫ������
         return 0;
     }
     return id;//���������ɫ����"���֤��"
 }
  unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)//���붥���Ƭ����ɫ����Դ����
 {
     GLCall(unsigned int program = glCreateProgram());//����һ���պ�������װ��������ɫ��������������ӵ����֤��
     unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);//������������ɫ�����������֤��
     unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
     glAttachShader(program, vs);//��������ɫ��װ������
     glAttachShader(program, fs);//
     glLinkProgram(program);//����������ɫ��������һ��
     glValidateProgram(program);//��֤��������Ƿ�Ϸ�
     glDeleteShader(vs);//ɾ����������ɫ������Ϊ�Ѿ������ڳ�������
     glDeleteShader(fs);
     return program;//������������������֤��
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
