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
#include <time.h>


#ifdef NANOVG_GLEW
	#define GLEW_STATIC
	#include <GL/glew.h>
#else
	#define GL_GLEXT_PROTOTYPES
#endif

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include "nanovg.h"
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

class BigButton : public Button
{
protected:
	float scale;
	int animestate;
	bool repeat;
	int atime;
	float duration;
	float targesw;
	float targesh;
	float targesx;
	float targesy;
	float t;
public:
	BigButton( const char * name , const char * title, int x, int y, int width, int height  )
	{
		scale = 1.0f;
		repeat = true;
		this->name = name;
		this->title = title;
		pos.x = x;
		pos.y = y;
		size.w = width;
		size.h = height;
		atime=-1;
		duration = 1000.0f;
		animestate = -1;

	}

	virtual bool onFrameMove( Screen * sp, int time )
	{
			Matrix4x4 mtx;

			if( animestate == -1 )
			{
				return Button::onFrameMove(sp, time);
			}

			invalid = true;

			if( atime == -1 )
			{
					targesw = sp->getWidth()  / (float)size.w;
					targesh = sp->getHeight() / (float)size.h;
					targesx = sp->getWidth() / 2.0f - apos.x -size.w/2.0;
					targesy = sp->getHeight() / 2.0f  - apos.y -size.h/2.0;
					atime=time;
			}

			if( animestate == 0 )
			{
				float pos = (float)(time-atime)/(float)duration;
				float cx = 1.0f + targesw * easeInOutQuint(pos);
				float cy = 1.0f + targesh * easeInOutQuint(pos);
				float px = targesx * easeInOutQuint(pos);
				float py = targesy * easeInOutQuint(pos);
				//printf("time:%d,%f\n",time,scale);

				mtx.translate( px,py , 0.0f );
				mtx.translate( ((size.w/2)),((size.h/2)) , 0.0f );
				mtx.scale( cx, cy, 1.0f );
				mtx.translate( -((size.w/2)),-((size.h/2)) , 0.0f );

				this->animetion_mtx = mtx;

				if( pos >= 1.0f )
				{
					animestate = 1;
					atime = -1;
				}

			}else if( animestate == 1 )
			{
				float pos = (float)(time-atime)/(float)(duration*5);

				t = (float)(time-atime)/(float)duration;
				if( pos >= 1.0f )
				{
					animestate = 2;
					atime = -1;
				}
			}else if( animestate == 2 )
			{
					float pos = (float)(time-atime)/(float)duration;
					float cx = 1.0f + targesw * ( 1.0f - easeInOutQuint(pos));
					float cy = 1.0f + targesh * ( 1.0f - easeInOutQuint(pos));
					float px = targesx * ( 1.0f - easeInOutQuint(pos));
					float py = targesy * ( 1.0f - easeInOutQuint(pos));
					//printf("time:%d,%f\n",time,scale);

					mtx.translate( px,py , 0.0f );

					mtx.translate( ((size.w/2)),((size.h/2)) , 0.0f );
					mtx.scale( cx, cy, 1.0f );
					mtx.translate( -((size.w/2)),-((size.h/2)) , 0.0f );

					this->animetion_mtx = mtx;


					if( pos >= 1.0f )
					{
						animestate = -1;
						atime = -1;
						this->pos.z = 0.0f;
					}

			}

			return Button::onFrameMove(sp,time);
	}

	virtual void draw( Screen * sp, NVGcontext* vg )
	{
		Button::draw( sp, vg );
		if( animestate == 1 )
		{
			float cx = sp->getWidth()/2;
			float cy = sp->getHeight()/2;
			float r = 72;
			float a0 = 0.0f + t*6;
			float a1 = NVG_PI + t*6;
			float r0 = r;
			float r1 = r * 0.75f;
			float ax,ay, bx,by;
			NVGpaint paint;

			nvgSave(vg);
			nvgReset(vg);
			nvgBeginPath(vg);
			nvgArc(vg, cx,cy, r0, a0, a1, NVG_CW);
			nvgArc(vg, cx,cy, r1, a1, a0, NVG_CCW);
			nvgClosePath(vg);
			ax = cx + cosf(a0) * (r0+r1)*0.5f;
			ay = cy + sinf(a0) * (r0+r1)*0.5f;
			bx = cx + cosf(a1) * (r0+r1)*0.5f;
			by = cy + sinf(a1) * (r0+r1)*0.5f;
			paint = nvgLinearGradient(vg, ax,ay, bx,by, nvgRGBA(236,239,241,0), nvgRGBA(236,239,241,255));
			nvgFillPaint(vg, paint);
			nvgFill(vg);
			nvgRestore(vg);
		}

	}

	virtual void onClick( )
	{
		atime = -1;
		repeat = false;
		title = "";
		animestate = 0;
		this->pos.z = -1.0;

	}
};

class BigButton2 : public Button
{
protected:
	int animestate;
	int starttime;
	int duration;
	float aval;
	float alpha;
	int cx;
	int cy;
	float r;

public:
	BigButton2( const char * name , const char * title, int x, int y, int width, int height  )
	{
		animestate = 0;
		starttime = -1;
		duration = 1000*2;
		aval = 0.0f;
		alpha = 0.0f;
		r = -1.0f;

		this->name = name;
		this->title = title;
		pos.x = x;
		pos.y = y;
		size.w = width;
		size.h = height;

		hovering = false;
		atime = -1;
		scale = 0.0f;
	}

	virtual bool onFrameMove( Screen * sp, int time )
	{
 		// Animation
		if( animestate == 1 )
		{
			if(starttime == -1 )
			{
				starttime = time;
			}

			if( r < 0 )
			{
				r = sqrt( (double)sp->getWidth() * (double)sp->getWidth()
									+ (double)sp->getHeight() * (double)sp->getHeight());
			}

			float pos = (float)(time-starttime)/(float)duration;
			aval = r * ( easeInOutQuint(pos));
			alpha = easeOutQuint(pos);

			if( pos > 1.0f )
			{
					pos = 0.0f;
					animestate = 2;
					invalid = false;
					starttime = -1;
			}else{
				invalid = true;
			}
		// Keep
		}else if( animestate == 2 )
		{
			if(starttime == -1 )
			{
				starttime = time;
			}

			float pos = (float)(time-starttime)/(float)duration;
			aval = r; //* ( easeInOutQuint(pos));

			if( pos > 1.0f )
			{
					pos = 0.0f;
					animestate = 3;
					invalid = false;
					starttime = -1;
			}else{
				invalid = true;
			}
		// Back
		}else if( animestate == 3 )
		{
				if(starttime == -1 )
				{
					starttime = time;
				}

				float pos = (float)(time-starttime)/(float)duration;
				aval = r * ( 1.0f - easeInOutQuint(pos));
				alpha = 1.0f - easeInQuint(pos);

				if( pos > 1.0f )
				{
						pos = 0.0f;
						animestate = 0;
						invalid = false;
						starttime = -1;
				}else{
					invalid = true;
					this->pos.z = 0.0f;
				}
		}

		return Button::onFrameMove(sp,time);
	}

	virtual void draw( Screen * sp, NVGcontext* vg )
	{
		Button::draw(sp,vg);
		if( aval > 0.0f ){

			Matrix4x4 tmtx;
			nvgSave(vg);
			float m[6];
			tmtx = matrix * animetion_mtx;
			tmtx.getMatrix2x3( m );
			nvgTransform( vg, m[0],m[1],m[2],m[3],m[4],m[5] );

			nvgBeginPath(vg);
			nvgCircle(vg, cx, cy, aval );
			nvgFillColor(vg, nvgRGBA(0,96,128,255*alpha));
			nvgFill(vg);
			nvgRestore(vg);

			nvgSave(vg);
			nvgReset(vg);
			nvgFontSize(vg, 72.0f);
			nvgFontFace(vg, "sans");
			nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);

			nvgFillColor(vg, nvgRGBA(0,0,0,160*(aval/r)));
			nvgText(vg, sp->getWidth()/2 * (aval/r) ,sp->getHeight()/2 -1 ,"Searching...", NULL);

			nvgFillColor(vg, nvgRGBA(255,255,255,255*(aval/1200.0f)));
			nvgText(vg, sp->getWidth()/2 * (aval/r) ,sp->getHeight()/2,"Searching...", NULL);
			nvgRestore(vg);

		}
	}

	virtual void onClick( )
	{
		this->cx = size.w/2;
		this->cy = size.h/2;
		this->animestate = 1;
		this->pos.z = -1.0;
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
	NanoUiTest * parent;
	OnButtonXYcbk( NanoUiTest * parent ){ this->parent = parent; }
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
		NanoUiTest * parent;
		OnClickbck( NanoUiTest * parent ){ this->parent = parent; }
		virtual void  operator()(UiEvent * p )
		{
		}
	};

	int initWidgets()
	{
		shared_ptr<Panel> p(new Panel( "panel1","Panel Test", 10,10, 200,480 ));

			// preper callback function
			shared_ptr<OnClickbck> fp(new OnClickbck(this));

			shared_ptr<Button> btn1(new BigButton2("btn1", "OK", 0, 0, FIT_PARENT, WRAP_CONTENT));
			btn1->connect( WE_ON_CLICK, fp );
			p->addWidget(btn1);

			shared_ptr<BigButton> btn2(new BigButton( "btn2","OK2", 0,0, FIT_PARENT,WRAP_CONTENT ));
			//btn2->connect( WE_ON_CLICK, fp );
			p->addWidget(btn2);

			shared_ptr<OnButtonXYcbk> fpxy(new OnButtonXYcbk(this));

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

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	window = glfwCreateWindow(1920, 1080, "NanoUI Test", NULL, NULL);
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
	vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
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
		uitest.onFrameMove(dt, mx, my, g_btn);

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
