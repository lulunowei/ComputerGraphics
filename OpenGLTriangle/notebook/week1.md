# 0513-0525

```
#include <glad/glad.h>     // 先包含 glad，必须第一位
#include <GLFW/glfw3.h>    // 然后再是 GLFW（或 SDL、GLUT 等）
#include <iostream>        // 其他标准库或自定义头文件最后

```

 **`glad` 是一个 OpenGL 加载器（loader）**，它的作用是在运行时加载 OpenGL 函数指针。必须在最前面包含

只需要在激活OPENGL上下文后加载一次就行：

```c++
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(800, 600, "GLModel", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return ;
    }
    glfwMakeContextCurrent(m_window);//openGl上下文
    loadGlad();
    viewPort();//渲染窗口大小
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);//窗口resize回调
```

===========================================================================================

在链接问题中：

```c++
namespace vertexShader {
    extern const char* vertexShaderSource;
}

namespace fragmentShader {
    extern const char* fragmentShaderSource;
}
```

这段在.h头文件申明，不能定义，否则会造成重复定义

```c++
namespace vertexShader {
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
}

namespace fragmentShader {
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";
}
```

实现放在.cpp

===========================================================================================

在shader语法中：

向量数据类型允许进行一些有趣且灵活的分量选择，称为 swizzling。Swizzling 允许我们使用这种语法

```c
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;


vec2 vect = vec2(0.5, 0.7);
vec4 result = vec4(vect, 0.0, 0.0);
vec4 otherResult = vec4(result.xyz, 1.0);
```

如果我们想从一个着色器向另一个着色器发送数据，我们需要在发送着色器中声明一个输出，并在接收着色器中声明一个类似的输入。当两边的类型和名称都相等时，OpenGL 会将这些变量连接起来，然后就可以在着色器之间发送数据（这通常在链接程序对象时完成）。

vertexShader

```
#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
  
out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
}
```

fragmentShader

```
#version 330 core
out vec4 FragColor;
  
in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

void main()
{
    FragColor = vertexColor;
} 
```

===========================================================================================

文件流

读文件：

```c++
std::vector<char> FileUtils::readFile(const std::string& filename) {
    // 用二进制模式打开文件，并且把读取位置设置到文件尾部（ate = at end）
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        // 打不开文件就抛出异常
        throw std::runtime_error("failed to open file!");
    }
    // 读取当前文件指针位置，也就是文件大小（因为指针在文件尾部）
    size_t fileSize = (size_t)file.tellg();
    // 创建一个 vector<char> 缓冲区，大小就是文件大小，也就是读取到内存中
    std::vector<char> buffer(fileSize);
    // 把文件指针重新定位到文件开头
    file.seekg(0);
    // 读取整个文件内容到 buffer 中
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer; // 返回读取好的文件内容
}
```

===========================================================================================

强换类型

string->const char*->const GLchar *->const char GLchar *

```c++
GLchar* source = vertexCode.c_str();//vertexCode 是stirng类型
const source//给source添加了const修饰
&source//继续取地址，则会得到const char*const *类型
```

致命问题：返回了局部变量

```c++
        std::ifstream shaderFile(path, std::ios::ate);
        if (!shaderFile.is_open()) {
            throw std::runtime_error("Failed to open shader file: " + path);
        }
        size_t fileSize = (size_t)shaderFile.tellg();
        std::string buffer(fileSize, '\0');
        shaderFile.seekg(0);//文件指针到头部
        shaderFile.read(&buffer[0], fileSize);//读取文件到内存中
        shaderFile.close();
        const GLchar* glBuffer=buffer.c_str();
        return glBuffer;
```

这里的buffer是局部变量，当函数作用域结束就是释放，同时glBuffer成了悬浮指针

===========================================================================================

局部静态变量

```c++
class A {
public:
    static A& getInstance() {
        static A instance;
        return instance;
    }

    void doSomething();

private:
    A() = default;
    ~A() = default;
    A(const A&) = delete;
    A& operator=(const A&) = delete;
};

```

通过类A申明局部静态变量的初始化方法能够让别的类使用它的时候不必但是是否有没有初始化

```c++
A::getInstance().doSomething();
```

===========================================================================================

如果一个类中有引用类型作为成员变量，则类中的赋值重载函数不能调用默认的拷贝；

需要重写拷贝，重载：

```c++
class MyClass {
private:
    int &ref;

public:
    MyClass(int &value) : ref(value) {}

    // 拷贝构造（保持原有引用的行为）
    MyClass(const MyClass &other) : ref(other.ref) {}

    // 赋值运算符
    MyClass& operator=(const MyClass &other) {
        // 不是改变引用绑定，而是赋值给引用的对象
        ref = other.ref;
        return *this;
    }
};

```

默认赋值相当于把当前这个引用指向了别处，违反了引用一经赋值就不能修改的规定

===========================================================================================

如果想要将拷贝和赋值函数不被调用，也就是独一对象，可以将它们放到private申明中，并且不在cpp内定义：

```c++
class Uncopyable{
	protected:
    Uncopyable(){}
    ~Uncopyable(){}
    private：
        Uncopyable(const Uncopyable&);
    	Unconpyable&(const Uncopyable&);
}
class derived:
	private Uncopyable{
};
```

===========================================================================================

带有多态性质的base classed应该申明virtual 析构函数，或者任意带有virtual 函数的类也需要申明虚构函数；

换言之，如果不是为了被继承，那么就不需要析构函数

===========================================================================================

永远不要在析构函数中抛出异常！

===========================================================================================

若需要传递两个参数：

```c++
void process(std::shared_ptr<Widget> pw,int priority);

std::shared_ptr<Widget> pw(new Widget);
//调用process
process(pw,priority);
```

最好分开传递需要进行内存管理的数据，否则造成未定义形式的内存管理形式

===========================================================================================