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
#if defined(NANOVG_GLEW)
#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#endif

#include "nanovg.h"
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

#include "nanoui.h"

#include <time.h>

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

class AnimateButton : public Button
{
protected:
	float scale;
	bool running;
	bool repeat;
	int atime;
public:
	AnimateButton( const char * name , const char * title, int x, int y, int width, int height  )
	{
		scale = 1.0f;
		repeat = true;
		this->name = name;
		this->title = title;
		pos.x = x;
		pos.y = y;
		size.w = width;
		size.h = height;
		running = false;
		atime=-1;
	}

	virtual bool onFrameMove( Screen * sp, int time )
	{
			Matrix4x4 mtx;

			if( running == false )
			{
					return true;
			}

			if( atime == -1 )
			{
						atime=time;
			}

			float t = (float)((time-atime)%1000)/1000.0f;
			scale = 1.0f + (sin(t*M_PI*2.0)+1.0f)*0.10;

			//printf("time:%d,%f\n",time,scale);

			mtx.translate( ((size.w/2)),((size.h/2)) , 0.0f );
			mtx.scale( scale, scale, 1.0f );
			mtx.translate( -((size.w/2)),-((size.h/2)) , 0.0f );
			this->animetion_mtx = mtx;

			if( scale == 1.0f && repeat == false )
			{
				running = false;
			}

			return true;
	}

	virtual void onHoverCursol( int x, int y)
	{
		running = true;
		repeat = true;
		atime = -1;
		Button::onHoverCursol(x,y);
	}

	virtual void onLeaveCursol()
	{
		repeat = false;
		Button::onLeaveCursol();
	}

};

class NanoUiTest : public Screen
{

public:
	NanoUiTest( )
	{

	}

	struct OnButtonXYcbk : public EventCallBack
	{
		virtual void  operator()(UiEvent * p )
		{
			UiEventXY * pxy = dynamic_cast<UiEventXY *>(p);
			if(pxy)
			{
					printf("OnButtonXY!! (%d,%d)\n", pxy->x, pxy->y );
			}
		}
	};

	struct OnClickbck : public EventCallBack
	{
		virtual void  operator()(UiEvent * p )
		{
				printf("OnClickbck!!");
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

			shared_ptr<AnimateButton> btn2(new AnimateButton( "btn2","OK2", 0,0, FIT_PARENT,WRAP_CONTENT ));
			btn2->connect( WE_ON_CLICK, fp );
			p->addWidget(btn2);

			shared_ptr<OnButtonXYcbk> fpxy(new OnButtonXYcbk());

			shared_ptr<CheckButton> btn3(new CheckButton( "btn3","CheckBoxTest", 0,0, FIT_PARENT,WRAP_CONTENT ));
			btn3->connect( WE_ON_BUTTON_ON, fpxy );
			p->addWidget(btn3);

			shared_ptr<Slider> btn4(new Slider( "btn3","Slider", 0,0, FIT_PARENT,WRAP_CONTENT ));
			p->addWidget(btn4);

			shared_ptr<Editbox> edit(new Editbox( "edt1","AbCdEfG", 0,0, FIT_PARENT,WRAP_CONTENT ));
			p->addWidget(edit);

			shared_ptr<Label> lb(new Label( "llbb","AbCdEfG", 0,0, FIT_PARENT,WRAP_CONTENT ));
			p->addWidget(lb);

/*
			AnimationTranslate pat = new AnimationTranslate( trigger, startval, endval, duration );
			pat->setRepeat(0);
			pat->setOverrride(true);

			AnimationScale pat = new AnimationScale( trigger, startval, endval, duration );

*/
		addWidget(p);
		return 0;
	}


};


int getClock_ms()
{
#if defined(WIN32)
	return clock();
#else
	int u4SysTime = 0;				/* システム時間[msec] */
	struct timespec sClockTime;		/* 経過起動時間 */

	memset( &sClockTime, 0x00, sizeof(struct timespec) );

	/* 経過起動時間を取得(成功した場合) */
	if( 0 == clock_gettime( CLOCK_MONOTONIC, &sClockTime ) ) {
		/* [秒→ミリ秒] + [ナノ秒→ミリ秒] */
		u4SysTime = (sClockTime.tv_sec * 1000)
					+ (sClockTime.tv_nsec / 1000000);
	}
	return u4SysTime;
#endif
}

int main()
{
	GLFWwindow* window;
	NVGcontext* vg = NULL;
	double prevt = 0;

	// Setup OpenGL
	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}
#if defined(NANOVG_GLES3_IMPLEMENTATION)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif	
	window = glfwCreateWindow(1000, 600, "NanoUI Test", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// set up callbacks
	glfwSetKeyCallback(window, keycb);
	glfwSetCharCallback(window, charcb);
  glfwSetMouseButtonCallback(window, buttoncb);

	// set up opengl
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

#ifdef NANOVG_GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();
#endif

	// create nanovg objects
#if defined(NANOVG_GL3_IMPLEMENTATION)
	vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#elif defined(NANOVG_GLES3_IMPLEMENTATION)
	vg = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

	// create nanoui
	NanoUiTest uitest;

	// initialize nanoui using nanovg
	if( uitest.initNanoVg( vg ) == -1 )
	{
		goto CLEANUP;
	}

	// setup widgets
	uitest.initWidgets();

	glfwSetTime(0);
	prevt = getClock_ms();
	while (!glfwWindowShouldClose(window))
	{
		double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;

		t = getClock_ms();
		dt = t - prevt;
		//prevt = t;
		glfwGetCursorPos(window, &mx, &my);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

		// update frame infomation
		uitest.onFrameMove( dt, mx, my, g_btn );

		// need to draw??
		if(uitest.isInvalid()){

			// draw
			uitest.draw( winWidth,winHeight);

			// show draw bufer
			glfwSwapBuffers(window);

		}

		glfwPollEvents();
	}

CLEANUP:
#if defined(NANOVG_GL3_IMPLEMENTATION)
	nvgDeleteGL3(vg);
#elif defined(NANOVG_GLES3_IMPLEMENTATION)
	nvgDeleteGLES3(vg);
#endif
	glfwTerminate();

	return 0;
}
