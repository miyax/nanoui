/*
Copyright (c) 2014 Shinya Miyamoto( smiyaxdev@gmail.com )

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>
#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GLES3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

#include "nanoui.h"

using namespace nanoui;

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

static void keycb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

static void charcb(GLFWwindow* window, unsigned int codepoint)
{
}

eBtnState g_btn=btnOFF;

static void buttoncb(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT ) {
		if (action == GLFW_PRESS) {
			g_btn = btnON;
		}
		if (action == GLFW_RELEASE) {
			g_btn = btnOFF;
		}
	}
}

class NanoUiTest : public Screen
{
	
public:
	NanoUiTest( )
	{
		
	}

	struct OnClickbck : public EventCallBack
	{
		virtual void  operator()(Widget * p )
		{
			printf("hit!!");
		}
	};
	
	int initWidgets()
	{
		shared_ptr<Panel> p(new Panel( "panel1","Panel Test", 10,10, 200,480 ));

			// preper callback function
			shared_ptr<OnClickbck> fp(new OnClickbck());
		  
			shared_ptr<Button> btn1(new Button( "btn1","OK", 0,0, FIT_PARENT,WRAP_CONTENT ));
			btn1->connect( WE_ON_CLICK, fp );
			p->addWidget(btn1);
		
			shared_ptr<Button> btn2(new Button( "btn2","OK2", 0,0, FIT_PARENT,WRAP_CONTENT ));
			btn2->connect( WE_ON_CLICK, fp );
			p->addWidget(btn2);  
	
			shared_ptr<CheckButton> btn3(new CheckButton( "btn3","CheckBoxTest", 0,0, FIT_PARENT,WRAP_CONTENT ));
			btn3->connect( WE_ON_CLICK, fp );
			p->addWidget(btn3);  
	
			shared_ptr<Slider> btn4(new Slider( "btn3","Slider", 0,0, FIT_PARENT,WRAP_CONTENT ));
			p->addWidget(btn4); 

			shared_ptr<Editbox> edit(new Editbox( "edt1","AbCdEfG", 0,0, FIT_PARENT,WRAP_CONTENT ));
			p->addWidget(edit); 
			
			
			
						
		addWidget(p);
		return 0;
	}
	

};

int main()
{
	GLFWwindow* window;
	NVGcontext* vg = NULL;
	double prevt = 0;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1000, 600, "NanoUI Test", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	
	glfwSetKeyCallback(window, keycb);
	glfwSetCharCallback(window, charcb);
    glfwSetMouseButtonCallback(window, buttoncb);
    
	glfwMakeContextCurrent(window);
	
	vg = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

	glfwSwapInterval(0);

	glfwSetTime(0);
	prevt = glfwGetTime();
	

	NanoUiTest uitest;
	
	if( uitest.initNanoVg( vg ) == -1 )
	{
		nvgDeleteGLES3(vg);
		glfwTerminate();
		return -1;
	}
	
	uitest.initWidgets();

	
	while (!glfwWindowShouldClose(window))
	{
		double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;
		
		t = glfwGetTime();
		dt = t - prevt;
		prevt = t;
		glfwGetCursorPos(window, &mx, &my);

	
		if( uitest.onFrameMove( dt, mx, my, g_btn ) == true )
		{
			glfwGetWindowSize(window, &winWidth, &winHeight);
			glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		
			uitest.draw( winWidth,winHeight);
		
			glfwSwapBuffers(window);
		}

		glfwPollEvents();
	}

	nvgDeleteGLES3(vg);
	glfwTerminate();

	return 0;
}
