# 自学路径
## OpenGL
https://learnopengl-cn.github.io/
### 环境配置（ 2025.7.10）
- 下载对应本机OpenGL版本（一般都选取最新版本没什么问题）的GLFW、CMake、Glad、minGW、Visual Studio即可。
- 注意管理好GLFW的include文件和lib文件夹，再vs中设置好即可
    - 直接放进vs用的包含文件路径和库文件路径
    - 另建文件，再vs项目-属性-VS c++和链接器中设置好路径即可
- 测试方式：新建c++空项目，新建cpp文件，包含如下头，不报错即可


```cpp
    #include<glad/glad.h>
    #include<GLFW/glfw3.h>
```
### 入门 （2025.7.10）
- OpenGL被看作Api,但实际上是一种规范，定义函数如何执行和输出结果。具体开发实现细节由显卡生产商决定
- 鼓励开发者再核心模式下开发，分水岭版本为3.3
- 扩展由类似if-else实现
- OpenGL的状态也被称为上下文
    - 状态设置函数
    - 状态执行函数
- 类c

---

### 窗口入门（2025.7.11）
#### 准备工作(包含头文件后)
##### 窗口版本设置：
```cpp
glfwInit();
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
```
##### 创建窗口对象
```cpp
//创建窗口对象
GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
if (window == NULL)
{
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window);
```
##### 初始化glad
glad是用于管理OpenGL函数指针的，在使用gl函数之前都要先初始化glad（避免冗长函数前缀、版本确定等）
```cpp
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
{
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
}
```
##### gl窗口设置
1. 我们需要之一前面设置的窗口不是gl的，一半应用会有内外框（我是这么理解的），另外关于gl窗口可能需要处理一些用户输入，比如鼠标移动、键盘反馈、手柄输入反馈等等，所以gl窗口是有操作回调/检测函数的：
    ```cpp
    //窗口大小修改回调
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    //GL渲染窗口大小
    glViewport(0, 0, 800, 600);
    //注册回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    ```
2. 除了注册回调函数之外，还有一种检测方案：\
通常关于这种按键输入在项目运行时非常重要（用户交互体验），因此我们通常需要在运行时循环检测\
    ```cpp
    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
    ```
3. 渲染循环初识：\
我们的项目（游戏、网页、VR等）通常肯定不是由一张静态图片构成的，因此项目渲染出来的画面一定是有变化的，变化即涉及到程序循环，只要我们不想关闭这个窗口/项目，就一直在渲染：
    ```cpp

    // 渲染循环
    while(!glfwWindowShouldClose(window))//这就是刚刚提到的按键检测
    {
        //清楚前一桢内容，这里以固定颜色为例：
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//状态设置函数
        //此处的颜色四向量值即RGB+Alpha值/透明度
        glClear(GL_COLOR_BUFFER_BIT);//状态转移函数

        // 渲染指令
        ...

        // 检查并调用用户输入、额外事件，交换缓冲
        processInput(window);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    ```

以上我们便做好了入门的程序准备工作，可以进入我们的图形内容了

-----

### 着色器初识（2025.7.11）
#### 什么是渲染管线
    - 我们知道现实世界是3D的，OpenGL中的世界也是3d的，物体存放在3d空间中，但是我们的屏幕肯定是2d平面
    - 我们的世界中的物体应该是连续的，OpenGL中的物体大多也以连续出现，比如三角形有三条直的线段围成，这一个平面他是连续的；但是我们的屏幕由像素构成，像素与像素之间是离散的。
    - 如何将3d坐标转换成2d坐标，如何将连续的2d图形转换成像素并确定颜色，就是图形渲染管线负责的
#### 什么是着色器
    因为图形渲染管线的功能职责由明确的逻辑阶段，具有并行特性，相当适合利用GPU这种高度专门化、并行化计算，显卡上成千上万的小核心在渲染管线各阶段运行的小程序就是着色器。
#### GLSL:OpenGL Shading Language
    编写OpenGL着色器的语言
#### 图形渲染管线抽象阶段认识：
0. 输入：顶点数据（坐标、属性）
2. 顶点着色器：\
        转换3d坐标，同时可对顶点属性进行一些调整
3. 几何着色器：\
        顶点着色器可以选择性将输出数据部分给几何着色器，几何着色器通过输入的坐标集合（可生成图元）以及可生成的新的点构成新的图元。
4. 图元装配：\
        读取从顶点着色器和几何着色器读取的数据生成图元即形状
5. 光栅化阶段\
        将图元映射到屏幕像素，生成片段
6. 裁切，丢器视图外的片段
7. 片段着色器：\
        计算像素颜色，根据场景数据、物体数据以及深度、遮挡关系、透明度等计算颜色
    **我们必须定义至少一个顶点着色器和一个片段着色器，几何着色器可选**
#### 渲染部分：
对于以下代码，只需要按两部分看就好，一部分是顶点着色器和片段着色器的定义：
- 顶点着色器：
    ```cpp
    const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
    ```
    对应在程序中：
    ```cpp
    //创建顶点着色器
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//将代码头部编写的着色器源码附加到刚刚创建的着色器对象上，然后编译
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//编译成功与否检测：
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
    ```
- 片段着色器：
    ```cpp
    const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor; \n"
    "uniform vec4 ourColor; // 在OpenGL程序代码中设定这个变量\n"
    "void main()\n"
    "{\n"
    "	FragColor = ourColor;//vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "} \0";
    ```
    对应在程序中：
    ```cpp
    //片段着色器创建
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
    ```
    为了方便调用怎么办呢？引入一个着色器程序负责合并编译好的着色器，并进行链接：
    ```cpp
    //着色器程序，合并着色器并链接，供后续渲染时调用
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//检测着色器程序是否链接成功
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}

	//因为顶点着色器和片段着色器已经被链接到程序对象中了，所需俩对象已经不需要了可以删除了
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
    ```
- 另一部分就是顶点初始化和绘制部分了,浓缩后是这样的：
    ```cpp
    // ..:: 初始化代码 :: ..
    // 1. 绑定顶点数组对象
    glBindVertexArray(VAO);
    // 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. 设定顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    [...]

    // ..:: 绘制代码（渲染循环中） :: ..
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    ```
- 完整版：
    ```cpp
    ///////////////
        //当想绘制矩形时
        float vertices[] = {
            0.5f, 0.5f, 0.0f,   // 右上角
            0.5f, -0.5f, 0.0f,  // 右下角
            -0.5f, -0.5f, 0.0f, // 左下角
            -0.5f, 0.5f, 0.0f   // 左上角
        };

        unsigned int indices[] = {
            // 注意索引从0开始! 
            // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
            // 这样可以由下标代表顶点组合成矩形

            0, 1, 3, // 第一个三角形
            1, 2, 3  // 第二个三角形
        };



        //创建缓冲，顶点缓冲对象
        unsigned int VBO;
        glGenBuffers(1, &VBO);

        //创建顶点数组对象，配置顶点属性指针时只需要在渲染循环外调用执行一次，
        //绘制物体时只需要绑定对应的VAO就行
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);

        //绑VAO
        glBindVertexArray(VAO);

        //将新创建的缓冲绑定到顶点缓冲类型上
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //将指定数据复制到指定类型的缓冲绑定对象
        //第四个参数指定了我们希望显卡如何管理给定的数据。它有三种形式：
        //  GL_STATIC_DRAW ：数据不会或几乎不会改变。
        //	GL_DYNAMIC_DRAW：数据会被改变很多。
        //	GL_STREAM_DRAW ：数据每次绘制时都会改变。
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        //创建元素缓冲对象
        unsigned int EBO;
        glGenBuffers(1, &EBO);

        //绑定元素缓冲对象
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


        //创建顶点着色器
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        //将代码头部编写的着色器源码附加到刚刚创建的着色器对象上，然后编译
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        //编译成功与否检测：
        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        //片段着色器创建
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        //着色器程序，合并着色器并链接，供后续渲染时调用
        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        //检测着色器程序是否链接成功
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
        }

        //因为顶点着色器和片段着色器已经被链接到程序对象中了，所需俩对象已经不需要了可以删除了
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);


        //通知OpenGL该如何解析顶点缓冲区中的数据
        /*
            glVertexAttribPointer即负责该功能
            第一个参数是顶点属性，之前设置VBO即顶点缓冲对象时设定了属性值location=0
                我认为是一种标记？标记后同标记的为一个集合，这个参数就负责划分哪个集合是目标
            第二个参数是顶点属性大小，即vec?中的？
            第三个参数是给定数据的类型
            第四个是是否希望数据被标准化
            第五个是步长
            第六个是数据在缓冲中基于初始位置的偏移量
        */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);


        //////////
        

        //渲染循环
        while (!glfwWindowShouldClose(window))
        {
            //绘制物体&&渲染指令
            //清理图像
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//状态设置
            glClear(GL_COLOR_BUFFER_BIT);//状态使用，使用当前设置的状态进行动作执行
            

            //激活程序对象
            glUseProgram(shaderProgram);

            //更新uniform类型参数值
            //颜色随时间渐变
            float timeValue = glfwGetTime();
            float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
            int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
            glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

            //绑定顶点数组对象
            glBindVertexArray(VAO);

            //使用激活的着色器对象、定义的顶点属性配置和通过VAO间接绑定的VBO中的顶点数据进行图元绘制
            //第一个参数是图元类型，第二个是顶点数组的起始索引，第三个是需要的顶点个数
            //glDrawArrays(GL_TRIANGLES, 0, 3);
            

            ////线条模式
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//填充模式

            //用元素缓冲代替数组输出
            //第二个参数时顶点数量，第三个时索引数据类型，最后一个参数时EBO中偏移量
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //因为绑定VAO时也会调用绑定EBO，刚好可以省掉
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
            glBindVertexArray(0);
            
            //输入
            processInput(window);


            //检测是否有回调事件，有则调用对应函数
            glfwPollEvents();

            //双缓冲，前缓冲为最终输出的图像；后缓冲负责计算渲染指令，所有完成后交换前后缓冲
            glfwSwapBuffers(window);

        }

        glfwTerminate();
    ```

### 着色器进一步（2025.7.11-2025.7.12）
根据之前学习

        