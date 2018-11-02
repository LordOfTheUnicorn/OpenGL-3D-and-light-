#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include <set>

#include <iostream>

//Обьявляем фун-ии
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

// Разрешение
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Данные для камеры
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Время
double deltaTime = 0.0f;
double lastFrame = 0.0f;

int main()
{
	// glfw: задаём версию и профиль
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw создаем окно	
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TASK", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // возращаем окно, контекст которого является текущим, или NULL если контекст окна не является текущим
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// GLFW зхаватываем курсор
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: загружаем все фун-ии 
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
		
	// Переменные
	//------------------------------
	Model all[28]
	{
		Model("obj/letters/LetterA.3ds",'a'),
		Model("obj/letters/LetterB.3ds",'b'),
		Model("obj/letters/LetterC.3ds",'c'),
		Model("obj/letters/LetterD.3ds",'d'),
		Model("obj/letters/LetterE.3ds",'e'),
		Model("obj/letters/LetterF.3ds",'f'),
		Model("obj/letters/LetterG.3ds",'g'),
		Model("obj/letters/LetterH.3ds",'h'),
		Model("obj/letters/LetterI.3ds",'i'),
		Model("obj/letters/LetterJ.3ds",'j'),
		Model("obj/letters/LetterK.3ds",'k'),
		Model("obj/letters/LetterL.3ds",'l'),
		Model("obj/letters/LetterM.3ds",'m'),
		Model("obj/letters/LetterN.3ds",'n'),
		Model("obj/letters/LetterO.3ds",'o'),
		Model("obj/letters/LetterP.3ds",'p'),
		Model("obj/letters/LetterQ.3ds",'q'),
		Model("obj/letters/LetterR.3ds",'r'),
		Model("obj/letters/LetterS.3ds",'s'),
		Model("obj/letters/LetterT.3ds",'t'),
		Model("obj/letters/LetterU.3ds",'u'),
		Model("obj/letters/LetterV.3ds",'v'),
		Model("obj/letters/LetterW.3ds",'w'),
		Model("obj/letters/LetterX.3ds",'x'),
		Model("obj/letters/LetterY.3ds",'y'),
		Model("obj/letters/LetterZ.3ds",'z'),
		Model("obj/marks/MarkComma.3ds",','),
		Model("obj/marks/MarkDot.3ds",'.')
	};

	double currentFrame;

	double x = -1.0;
	double y = 1.0;
	double z = 1.0;
	char c;

	set<char> symb;	// Символы котрые надо загрузить 
	vector<int> num;	
	ifstream file("text.txt");	
	//------------------------------

	//Загружаем символы в set
	if (file.is_open())
	{		
		while (!file.eof())
		{
			file >> c;
			symb.insert(c);
		}
	}	
	else
		cout << "ERROR LOADING FILE!" << endl;	

		
	// загружаем наши 3D символы и получаем их номера в массиве			
		for (int i = 0; i < 28; i++)	
		if (symb.find(all[i].symb) != symb.end())
		{
			all[i].loadModel(all[i].path);
			num.push_back(i);
		}
		
	
	// изменяем положение и размер 3D объекта
	auto lamda = [&x,&y,&z](char &c)
	{
		glm::mat4 model;

		if (c == '\n')
		{
			model = glm::translate(model, glm::vec3(x, y, z));
			x = -1.0;
			y -= 0.23;
		}
		else if (c == ' ')
		{
			x += 0.12;
			model = glm::translate(model, glm::vec3(x, y, z));
		}
		else
		{
			if (c == tolower(c))
			{
				model = glm::translate(model, glm::vec3(x, y, z));
				x += 0.08;

				model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
			}
			else
			{
				model = glm::translate(model, glm::vec3(x, y, z));
				x += 0.12;
				model = glm::scale(model, glm::vec3(0.15, 0.15, 0.15));
				c = tolower(c);
			}

		}
	
		return model;
	};
	

	// Включить проверку глубины (для оси z)
	glEnable(GL_DEPTH_TEST);
	// Принять фрагмент, если он ближе к камере, чем предыдущий
	glDepthFunc(GL_LESS);

	// Отбраковка треугольников, Нормаль которых не направлена к камере
	glEnable(GL_CULL_FACE);	

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Утанеавлием указатель в начало файла 	
	file.clear();
	file.seekg(0, ios_base::beg);
	
	Shader lightingShader("light.vs", "light.fs");

	while (!glfwWindowShouldClose(window))
	{
		x = -1.0;
		y = 1.0;
		z = 1.0;

		// Для камеры/управления 		
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;		
		processInput(window);

		// Чистим буфферы		
		glClearColor(0.128f, 0.0f, 0.128f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
		
		lightingShader.use();
		//позиция
		lightingShader.setVec3("light.position", camera.Position);
		//направление
		lightingShader.setVec3("light.direction", camera.Front);
		//область света
		lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));

		lightingShader.setVec3("viewPos", camera.Position);

		// light properties
		lightingShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
		lightingShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("light.constant", 1.0f);
		lightingShader.setFloat("light.linear", 0.09f);
		lightingShader.setFloat("light.quadratic", 0.032f);

		// material properties
		lightingShader.setFloat("material.shininess", 32.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// render
		while (file.get(c))
		{
			lightingShader.setMat4("model", lamda(c));

				for (auto i: num)
					if (c == all[i].symb)					
						all[i].Draw(lightingShader);
				
		}		
				
		
		
		file.clear();
		file.seekg(0, ios_base::beg);

		// Свапаем буфферы 
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
		
	// Освобождаем память
	glfwTerminate();
	return 0;
}

//Обрабатывать все вводимые данные : запрашивать GLFW нажаты / отпущены ли соответствующие клавиши в этом кадре
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime+0.2);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime + 0.2);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime + 0.2);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime + 0.2);
}

// glfw: всякий раз, когда размер окна изменяется (по ОС или изменению размера пользователя), эта функция обратного вызова выполняется
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{		
	glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // отменено с Y-координаты идут снизу вверх

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


