#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
// 最简单的顶点着色器
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor; \n"
"uniform vec4 ourColor; // 在OpenGL程序代码中设定这个变量\n"
"void main()\n"
"{\n"
"	FragColor = ourColor;//vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"} \0";


//窗口大小修改回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//处理输入按键事件
void processInput(GLFWwindow* window);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口对象
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	

	//初始化 glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//GL渲染窗口大小
	glViewport(0, 0, 800, 600);
	//注册回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	
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
	return 0;


}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}