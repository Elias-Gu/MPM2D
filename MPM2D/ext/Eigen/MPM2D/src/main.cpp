#include "constants.h"
#include "solver.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>


/* Declarations */
void initGLContext();
GLFWwindow* initGLFWContext();
Solver* Simulation;
int t_count = 0;

/* For video */
#if !WRITE_TO_FILE && RECORD_VIDEO
std::string str_cmd = "ffmpeg -r " + std::to_string(FPS) + " -f rawvideo -pix_fmt rgba -s "
+ std::to_string(X_WINDOW) + "x" + std::to_string(Y_WINDOW)
+ " -i - -threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip Output/movie.mp4";
const char* cmd = str_cmd.c_str();
FILE* ffmpeg = _popen(cmd, "wb");
int* buffer = new int[X_WINDOW * Y_WINDOW];
#endif



/* -----------------------------------------------------------------------
|					MATERIAL POINT METHOD ALGORITHM						 |
----------------------------------------------------------------------- */


void Initialization()
{
	std::vector<Border> inBorders = Border::InitializeBorders();
	std::vector<Node> inNodes = Node::InitializeNodes();
	std::vector<Material> inParticles = Material::InitializeParticles();

	Simulation = new Solver(inBorders, inNodes, inParticles);
}


void Update()
{
	Simulation->P2G();									// Transfer data from Particles to Grid Nodes
	Simulation->UpdateNodes();							// Update nodes data
	Simulation->G2P();									// Transfer data from Grid Nodes to Particles
	Simulation->UpdateParticles();						// Update particles data
}


void AddParticles()										// Add particle during the simulation
{							
	// DT_ROB gives the rate of insertion. Maximum number of particles
	if (t_count % DT_ROB == 0 && Simulation->particles.size() < 3000)
	{
		std::vector<Material> new_p = Material::AddParticles();
		for (size_t p = 0, plen = new_p.size(); p < plen; p++)
			Simulation->particles.push_back(new_p[p]);
	}
}



/* -----------------------------------------------------------------------
|								MAIN									 |
----------------------------------------------------------------------- */

int main(int argc, char** argv)
{
	/* Initialize Simulation */
	Initialization();

	/* [1] : output data to .ply file (to read in Houdini for example) */
	#if WRITE_TO_FILE
	int frame_count = 0;
	while (1)
	{
		AddParticles();
		Update();
		if (t_count % (int)(DT_render / DT) == 0)		// Record frame at desired rate
			Simulation->WriteToFile(frame_count++);
		Simulation->ResetGrid();
		t_count++;
	}
	
	#else
	/* [2] : show result on OpenGL window, and record an .mp4 if selected */
	GLFWwindow* window = initGLFWContext();				
	initGLContext();
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		AddParticles();
		Update();
		if (t_count % (int)(DT_render / DT) == 0)		// Display frame at desired rate
		{
			Simulation->Draw();
			glfwSwapBuffers(window);
			#if RECORD_VIDEO							
			glReadPixels(0, 0, X_WINDOW, Y_WINDOW, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			fwrite(buffer, sizeof(int)*X_WINDOW*Y_WINDOW, 1, ffmpeg);
			#endif
			glfwPollEvents();
		}
		Simulation->ResetGrid();
		t_count++;
		if (t_count == 5000)
			return 0;
	}

	#if RECORD_VIDEO
	_pclose(ffmpeg);
	#endif
	glfwTerminate();
	#endif

	return 0;
}



/* -----------------------------------------------------------------------
|						     	  OPENGL 								 |
----------------------------------------------------------------------- */


/* Define window. */
GLFWwindow* initGLFWContext()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	/* Create Window*/
	GLFWwindow* window = glfwCreateWindow(X_WINDOW, Y_WINDOW, "Simulation", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	return window;
}


/* Define view parameters */
void initGLContext()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, X_GRID, 0, Y_GRID, -1, 1);					// original
	glViewport(0, 0, (GLsizei)X_WINDOW, (GLsizei)Y_WINDOW);	// transfo

	glClearColor(.4f, .4f, .4f, .0f);
	glClear(GL_COLOR_BUFFER_BIT);
}