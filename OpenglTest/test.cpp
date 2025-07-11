#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
// ��򵥵Ķ�����ɫ��
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor; \n"
"uniform vec4 ourColor; // ��OpenGL����������趨�������\n"
"void main()\n"
"{\n"
"	FragColor = ourColor;//vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"} \0";


//���ڴ�С�޸Ļص�
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//�������밴���¼�
void processInput(GLFWwindow* window);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//�������ڶ���
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	

	//��ʼ�� glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//GL��Ⱦ���ڴ�С
	glViewport(0, 0, 800, 600);
	//ע��ص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	
	///////////////
	//������ƾ���ʱ
	float vertices[] = {
		0.5f, 0.5f, 0.0f,   // ���Ͻ�
		0.5f, -0.5f, 0.0f,  // ���½�
		-0.5f, -0.5f, 0.0f, // ���½�
		-0.5f, 0.5f, 0.0f   // ���Ͻ�
	};

	unsigned int indices[] = {
		// ע��������0��ʼ! 
		// ����������(0,1,2,3)���Ƕ�������vertices���±꣬
		// �����������±��������ϳɾ���

		0, 1, 3, // ��һ��������
		1, 2, 3  // �ڶ���������
	};



	//�������壬���㻺�����
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	//������������������ö�������ָ��ʱֻ��Ҫ����Ⱦѭ�������ִ��һ�Σ�
	//��������ʱֻ��Ҫ�󶨶�Ӧ��VAO����
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	//��VAO
	glBindVertexArray(VAO);

	//���´����Ļ���󶨵����㻺��������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//��ָ�����ݸ��Ƶ�ָ�����͵Ļ���󶨶���
	//���ĸ�����ָ��������ϣ���Կ���ι�����������ݡ�����������ʽ��
	//  GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
	//	GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
	//	GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//����Ԫ�ػ������
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	//��Ԫ�ػ������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//����������ɫ��
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//������ͷ����д����ɫ��Դ�븽�ӵ��ոմ�������ɫ�������ϣ�Ȼ�����
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//����ɹ�����⣺
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Ƭ����ɫ������
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

	//��ɫ�����򣬺ϲ���ɫ�������ӣ���������Ⱦʱ����
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//�����ɫ�������Ƿ����ӳɹ�
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}

	//��Ϊ������ɫ����Ƭ����ɫ���Ѿ������ӵ�����������ˣ������������Ѿ�����Ҫ�˿���ɾ����
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//֪ͨOpenGL����ν������㻺�����е�����
	/*
		glVertexAttribPointer������ù���
		��һ�������Ƕ������ԣ�֮ǰ����VBO�����㻺�����ʱ�趨������ֵlocation=0
			����Ϊ��һ�ֱ�ǣ���Ǻ�ͬ��ǵ�Ϊһ�����ϣ���������͸��𻮷��ĸ�������Ŀ��
		�ڶ��������Ƕ������Դ�С����vec?�еģ�
		�����������Ǹ������ݵ�����
		���ĸ����Ƿ�ϣ�����ݱ���׼��
		������ǲ���
		�������������ڻ����л��ڳ�ʼλ�õ�ƫ����
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	//////////
	

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//��������&&��Ⱦָ��
		//����ͼ��
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//״̬����
		glClear(GL_COLOR_BUFFER_BIT);//״̬ʹ�ã�ʹ�õ�ǰ���õ�״̬���ж���ִ��
		

		//����������
		glUseProgram(shaderProgram);

		//����uniform���Ͳ���ֵ
		//��ɫ��ʱ�佥��
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		//�󶨶����������
		glBindVertexArray(VAO);

		//ʹ�ü������ɫ�����󡢶���Ķ����������ú�ͨ��VAO��Ӱ󶨵�VBO�еĶ������ݽ���ͼԪ����
		//��һ��������ͼԪ���ͣ��ڶ����Ƕ����������ʼ����������������Ҫ�Ķ������
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		

		////����ģʽ
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//���ģʽ

		//��Ԫ�ػ�������������
		//�ڶ�������ʱ����������������ʱ�����������ͣ����һ������ʱEBO��ƫ����
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //��Ϊ��VAOʱҲ����ð�EBO���պÿ���ʡ��
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);
		
		//����
		processInput(window);


		//����Ƿ��лص��¼���������ö�Ӧ����
		glfwPollEvents();

		//˫���壬ǰ����Ϊ���������ͼ�񣻺󻺳帺�������Ⱦָ�������ɺ󽻻�ǰ�󻺳�
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