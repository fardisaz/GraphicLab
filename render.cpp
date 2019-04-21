
#include "render.h"



namespace visualisation
{
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow *window);

	// settings
	const unsigned int SCR_WIDTH = 1280;
	const unsigned int SCR_HEIGHT = 720;

	// camera
	Camera camera;
	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;
	bool firstMouse = true;

	// timing
	float deltaTime = 0.0f;	// time between current frame and last frame
	float lastFrame = 0.0f;

	render::render(shape* room) {
		this->room = room;
		getSize();
		setCameraPosition();
		visualise();
	}

	render::~render() {

	}

	void render::getSize() {
		for (int i = 0; i < room->children.size(); ++i) {
			if (room->children[i]->getType() == "floor") {
				w = room->children[i]->getWidth();
				l = room->children[i]->getLength();
			}
			else if (room->children[i]->getType() == "wall") {
				h = room->children[i]->getLength();
			}
		}
	}

	void render::setCameraPosition() {
		camera.Position = glm::vec3(-w * 0.7, h, w);
	}

	void render::loadFurnitures() {
		for (int i = 0; i < room->children.size(); ++i)
		{
			if (room->children[i]->getType() == "floor")
			{
				for (int j = 0; j < room->children[i]->children.size(); ++j)
				{
					modelPosition.push_back(room->children[i]->children[j]->getPosition());
					modelScale.push_back(room->children[i]->children[j]->getScale());
					modelAngle.push_back(room->children[i]->children[j]->getAngle());
					Model mod(room->children[i]->children[j]->getModelName());
					models.push_back(mod);

				}
			}
		}
	}

	GLFWwindow* render::intiframework() {
		// glfw: initialize and configure
		// ------------------------------
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
		#endif

			// glfw window creation
			// --------------------
		GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Final Project", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			// return -1;
		}
		glfwMakeContextCurrent(window);

		// Set the required callback functions
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		// tell GLFW to capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			// return -1;
		}

		// configure global opengl state
		// -----------------------------
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		return window;
	}

	Shader render::setupshaders() {
		// build and compile shaders
		// -------------------------
		Shader shader("vertex.vs", "fragment.fs");
		return shader;
	}

	void render::loadDoorAndWindow() {
		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float back[] = {
			//Door
			//positions        //normals          //texture coords
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f
		};
		float front[] = {
			//Front side
			//positions         //normals          //texture coords
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f
		};
		float left[] = {
			//Left side
			//positions         //normals         //texture coords
			-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
		};
		float right[] = {
			//Right side
			//positions         //normals         //texture coords
			 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
		};
		for (int i = 0; i < room->children.size(); ++i)
		{
			if (room->children[i]->getType() == "wall" && room->children[i]->children.size() > 0)
			{//the children of wall can be door ,window,poster
				for (int j = 0; j < room->children[i]->children.size(); ++j)
				{
					if (room->children[i]->children[j]->getType() == "door")
					{
						unsigned int door;
						roomTextures.push_back(door);
						imgDir.push_back(room->children[i]->children[j]->getTexture());

						if (room->children[i]->getName() == "fwall")
						{
							roomPosition.push_back(glm::vec3(room->children[i]->getWidth() / 4, -(h / 2 - (h*0.7) / 2), (l / 2) - 0.01));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.7, 0));
							setRoomVertices(front, sizeof(front));
						}
						else if (room->children[i]->getName() == "bwall")
						{
							roomPosition.push_back(glm::vec3(-room->children[i]->getWidth() / 4, -(h / 2 - (h*0.7) / 2), (-l / 2) + 0.01));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.7, 0));
							setRoomVertices(back, sizeof(back));
						}
						else if (room->children[i]->getName() == "lwall")
						{
							roomPosition.push_back(glm::vec3((-w / 2) + 0.01, -(h / 2 - (h*0.7) / 2), room->children[i]->getWidth() / 4));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.7, 0));
							setRoomVertices(left, sizeof(left));
						}
						else if (room->children[i]->getName() == "rwall")
						{
							roomPosition.push_back(glm::vec3((w / 2) - 0.01, -(h / 2 - (h*0.7) / 2), -room->children[i]->getWidth() / 4));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.7, 0));
							setRoomVertices(right, sizeof(right));
						}
					}
					else if (room->children[i]->children[j]->getType() == "window")
					{
						unsigned int window;
						roomTextures.push_back(window);
						imgDir.push_back(room->children[i]->children[j]->getTexture());

						if (room->children[i]->getName() == "fwall")
						{
							roomPosition.push_back(glm::vec3(-room->children[i]->getWidth() / 4, 0, (l / 2) - 0.01));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.4, 0));
							setRoomVertices(front, sizeof(front));
						}
						else if (room->children[i]->getName() == "bwall")
						{
							roomPosition.push_back(glm::vec3(-room->children[i]->getWidth() / 4, 0, (-l / 2) + 0.01));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.4, 0));
							setRoomVertices(back, sizeof(back));
						}
						else if (room->children[i]->getName() == "lwall")
						{
							roomPosition.push_back(glm::vec3((-w / 2) + 0.01, 0, -room->children[i]->getWidth() / 4));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.4, 0));
							setRoomVertices(left, sizeof(left));
						}
						else if (room->children[i]->getName() == "rwall")
						{
							roomPosition.push_back(glm::vec3((w / 2) + 0.01, 0, room->children[i]->getWidth() / 4));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.4, 0));
							setRoomVertices(right, sizeof(right));
						}
					}
					//////////////**********************poster
					else if (room->children[i]->children[j]->getType() == "poster")
					{
						unsigned int poster;
						roomTextures.push_back(poster);
						imgDir.push_back(room->children[i]->children[j]->getTexture());

						if (room->children[i]->getName() == "fwall")
						{
							roomPosition.push_back(glm::vec3(-room->children[i]->getWidth() / 4, 0, (l / 2) - 0.01));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.4, 0));
							setRoomVertices(front, sizeof(front));
						}
						else if (room->children[i]->getName() == "bwall")
						{
							roomPosition.push_back(glm::vec3(room->children[i]->getWidth() / 4, 0, (-l / 2) + 0.01));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 8, h*0.4, 0));
							setRoomVertices(back, sizeof(back));
						}
						else if (room->children[i]->getName() == "lwall")
						{
							roomPosition.push_back(glm::vec3((-w / 2) + 0.01, 0, -room->children[i]->getWidth() / 4));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.4, 0));
							setRoomVertices(left, sizeof(left));
						}
						else if (room->children[i]->getName() == "rwall")
						{
							roomPosition.push_back(glm::vec3((w / 2) + 0.01, 0, room->children[i]->getWidth() / 4));
							roomScale.push_back(glm::vec3(room->children[i]->getWidth() / 4, h*0.4, 0));
							setRoomVertices(right, sizeof(right));
						}
					}
				}
			}
		}

	}

	void render::loadRoom() {
		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float wallVertices[] = {
			//Back side
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			//Front side
			// -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			//  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			//  0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			//  0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			// -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			// -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			///////////////////////////********you can also comment the left wall
			//Left side
			 -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			 -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			 //Right side
			  0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			  0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			  0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		};
		float floorVertices[] = {
			//Floor
		   -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		   -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		   -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		};
		float ceilingVertices[] = {
			//Ceiling
			-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
		};

		bool wall = true;
		bool ceiling = false;
		for (int i = 0; i < room->children.size(); ++i)
		{
			if (room->children[i]->getType() == "ceiling" && ceiling)
			{
				unsigned int ceiling;
				roomTextures.push_back(ceiling);
				imgDir.push_back(room->children[i]->getTexture());
				roomPosition.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
				roomScale.push_back(glm::vec3(w, h, l));
				setRoomVertices(ceilingVertices, sizeof(ceilingVertices));

			}
			else if (room->children[i]->getType() == "floor")
			{
				unsigned int floor;
				roomTextures.push_back(floor);
				imgDir.push_back(room->children[i]->getTexture());
				roomPosition.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
				roomScale.push_back(glm::vec3(w, h, l));
				setRoomVertices(floorVertices, sizeof(floorVertices));

			}
			else if (room->children[i]->getType() == "wall" && wall)
			{
				unsigned int wall;
				roomTextures.push_back(wall);
				imgDir.push_back(room->children[i]->getTexture());
				roomPosition.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
				roomScale.push_back(glm::vec3(w, h, l));
				setRoomVertices(wallVertices, sizeof(wallVertices));
				wall = false;
			}
		}
	}

	void render::setRoomVertices(float vertices[], int size) {

		unsigned int VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size, &vertices[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex normals	
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		roomVAO.push_back(VAO);
	}

	void render::loadTextures(Shader shader) {
		shader.use();
		for (int i = 0; i < roomTextures.size(); ++i)
		{
			// load and create a texture 
			// -------------------------
			glGenTextures(1, &roomTextures[i]);
			glBindTexture(GL_TEXTURE_2D, roomTextures[i]);
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load image, create texture and generate mipmaps
			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
			unsigned char *data = stbi_load(imgDir[i], &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);
			shader.setInt("room", i);
		}
	}

	void render::visualise() {
		GLFWwindow* window = this->intiframework();

		// build and compile shaders
		// -------------------------
		Shader shader("vertex.vs", "fragment.fs");

		// load room
		// ---------
		loadRoom();
		loadDoorAndWindow();

		// load and create a texture 
		// -------------------------
		loadTextures(shader);

		// load models
		// -----------
		loadFurnitures();

		// render loop
		// -----------
		while (!glfwWindowShouldClose(window))
		{
			// per-frame time logic
			// --------------------
			float currentFrame = glfwGetTime();
			deltaTime = 2 * (currentFrame - lastFrame);
			lastFrame = currentFrame;

			// input
			// -----
			glfwPollEvents();
			processInput(window);

			// render
			// ------
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shader.use();

			// Transformation matrices
			// -----------------------
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
			glm::mat4 view = camera.GetViewMatrix();
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);


			// draw room
			// ---------
			for (int i = 0; i < roomTextures.size(); ++i)
			{
				// bind textures on corresponding texture units
				glActiveTexture(GL_TEXTURE + i);
				glBindTexture(GL_TEXTURE_2D, roomTextures[i]);
				// render boxes
				glBindVertexArray(roomVAO[i]);
				// calculate the model matrix for each object and pass it to shader before drawing
				glm::mat4 model;
				model = glm::translate(model, roomPosition[i]);
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
				model = glm::scale(model, roomScale[i]);
				shader.setMat4("model", model);

				glDrawArrays(GL_TRIANGLES, 0, 18);
			}

			// draw furnitures
			// ---------------
			for (int i = 0; i < models.size(); ++i)
			{
				glm::mat4 modelObj;
				modelObj = glm::translate(modelObj, modelPosition[i]); // Translate it down a bit so it's at the center of the scene
				modelObj = glm::rotate(modelObj, glm::radians((float)modelAngle[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				modelObj = glm::scale(modelObj, modelScale[i]);	// It's a bit too big for our scene, so scale it down
				shader.setMat4("model", modelObj);
				models[i].Draw(shader);
			}

			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		// optional: de-allocate all resources once they've outlived their purpose:
		// ------------------------------------------------------------------------
		glDeleteVertexArrays(1, &wallVAO);
		// glDeleteBuffers(1, &VBO);

		glfwTerminate();

	}

	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
	void processInput(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}

	// glfw: whenever the mouse moves, this callback is called
	// -------------------------------------------------------
	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;


		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.ProcessMouseScroll(yoffset);


	}

}