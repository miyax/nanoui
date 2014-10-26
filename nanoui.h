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

#ifndef _NANOUI_H_
#define _NANOUI_H_

#ifdef NANOVG_GLEW
	#include <GL/glew.h>
#else
	#define GL_GLEXT_PROTOTYPES
#endif

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include "nanovg.h"

#include <vector>
#include <deque>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>

using namespace std;

namespace nanoui {

#include "nanoui_matrix.h"

class Widget;
typedef vector<shared_ptr<Widget>> vecWidget;
class Screen;


struct Position
{
	Position()
	{
		x=y=z=0.0f;
	}
	float x;
	float y;
	float z;
};

struct Size
{
	float w;
	float h;
};

struct Rect
{
	Position pos;
	Size size;
};

struct vec2
{
	float x;
	float y;
};

struct vec3
{
	float x;
	float y;
	float z;
};

struct vec4
{
	float x;
	float y;
	float z;
	float w;
};



enum eBtnState
{
	btnOFF=0,
	btnON,
};

enum eWidgetState
{
	stIDLE = 0,
	stHOVER,
	stON
};

const int FIT_PARENT = -1;
const int WRAP_CONTENT = -2;

//-------------------------------------------------------------
enum eEvent
{
	WE_ON_HOVER_CURSOL,
	WE_ON_HOVER_MOVE_CURSOL,
	WE_ON_BUTTON_ON,
	WE_ON_CLICK,
	WE_ON_DRAG_MOVE_CURSOL,
	WE_ON_LEAVE_CURSOL,
	WE_MAX
};

struct UiEvent
{
		virtual ~UiEvent() {};
		eEvent ev;
		Widget * pObj;
};

struct UiEventXY : public UiEvent
{
		int x;
		int y;
};

struct  EventCallBack {
	virtual void operator()( UiEvent * p) {

	}
};

//----------------------------------------------------------------------
class Widget
{
protected:
	string name;
	vecWidget items;

	NVGcontext* vg;

	bool invalid;

	eWidgetState state;

	// CursolEvent
	virtual void onHoverCursol( int x, int y);
	virtual void onHoverMoveCursol( int x, int y);
	virtual void onClick();
	virtual void onButtonOn( int x, int y );
	virtual void onDragMoveCursol( int x, int y );
	virtual void onLeaveCursol();

	bool draggable;
	bool dragging;
	Position drag_point;

	shared_ptr<EventCallBack> cbks[WE_MAX];


public:
	Widget * parent;
	Position pos;
	Position apos;
	Size size;
	Matrix4x4 matrix;
	Matrix4x4 animetion_mtx;
	float margin;

	Widget();
	virtual ~Widget();
	virtual void addWidget( shared_ptr<Widget> item );
	virtual bool onFrameMove( Screen * sp, int time );
	virtual void draw( Screen * sp, NVGcontext* vg );

	int connect( eEvent ev, shared_ptr<EventCallBack>  cb );
	bool onButtonEvnet( Screen * sp, float x, float y, eBtnState btnstate );

	virtual float getMargin(){ return margin; }
	virtual void setMargin( float margin ){ this->margin = margin; }

	virtual void getPosAndSize( float & x, float & y, float & w, float & h )
	{
		x = pos.x;
		y = pos.y;
		w = size.w;
		h = size.h;
	}

	virtual void setPosAndSize( float x, float y, float w, float h )
	{
		pos.x = x;
		pos.y = y;
		size.w = w;
		size.h = h;
		matrix.translate( pos.x, pos.y, 0.0f ); //Todo move
	}

	float easeInQuint( float pos) {
		return pow(pos, 5);
	};

	float easeOutQuint( float pos) {
		return (pow((pos-1), 5) +1);
	};

	float easeInOutQuint( float pos) {
		if ((pos/=0.5) < 1) return 0.5*pow(pos,5);
		return 0.5 * (pow((pos-2),5) + 2);
	};

};

//----------------------------------------------------------------------
class Panel : public Widget
{
protected:
	string title;
	int row;

public:
	Panel();
	Panel( const char * name , const char * title, int x, int y, int width, int height  );
	virtual ~Panel( );
	virtual void addWidget( shared_ptr<Widget> item );
	virtual void draw( Screen * sp, NVGcontext* vg );
};

//----------------------------------------------------------------------
class Button : public Widget
{
protected:
	string title;
	int preicon;
	NVGcolor colIdle;
	NVGcolor colActive;

public:
	Button();
	Button( const char * name , const char * title, int x, int y, int width, int height  );
	void initBase();
	virtual ~Button( );
	virtual void draw( Screen * sp, NVGcontext* vg );

};

//----------------------------------------------------------------------
class CheckButton : public Widget
{
public:
	enum eCheckState
	{
		Checked,
		UnChecked,
	};

protected:
	string title;
	eCheckState check_state;
public:

	eCheckState getCheckState(){ return check_state; }

	CheckButton();
	CheckButton( const char * name , const char * title, int x, int y, int width, int height  );
	virtual ~CheckButton( );
	virtual void draw( Screen * sp, NVGcontext* vg );
	virtual void onClick();

};

//----------------------------------------------------------------------
class Editbox : public Widget
{
public:

protected:
	string text;
	void drawinside( Screen * sp, NVGcontext* vg,float x, float y, float w, float h );

public:
	Editbox();
	Editbox( const char * name , const char * text, int x, int y, int width, int height  );
	virtual ~Editbox( );

	virtual void draw( Screen * sp, NVGcontext* vg );
};

//----------------------------------------------------------------------
class EditboxNum : public Editbox
{
public:

protected:
	float val;
	string style;
	string unit;

public:
	EditboxNum();
	EditboxNum( const char * name , float val, const char * style, const char * unit, int x, int y, int width, int height );
	virtual ~EditboxNum();
	virtual void draw( Screen * sp, NVGcontext* vg );

	void setVal( float v ){ this->val = v; }
};


//----------------------------------------------------------------------
class Slider : public Widget
{
public:

protected:
	float width_rate;
	float min;
	float max;
	float slider_pos;
	shared_ptr<EditboxNum> num;

public:
	Slider();
	Slider( const char * name , const char * title, int x, int y, int width, int height  );
	void setMinMax( float min, float max ){ this->min = min; this->max = max; }
	virtual ~Slider( );
	virtual void draw( Screen * sp, NVGcontext* vg );

	virtual void onButtonOn( int x, int y );
	virtual void onDragMoveCursol( int x, int y );

	virtual void getPosAndSize( float & x, float & y, float & w, float & h );
	virtual void setPosAndSize( float x, float y, float w, float h );

};


//----------------------------------------------------------------------
class Label : public Widget
{
public:

protected:
	string text;

public:
	Label();
	Label( const char * name , const char * text, int x, int y, int width, int height  );
	virtual ~Label( );
	virtual void draw( Screen * sp, NVGcontext* vg );
};

typedef deque<Matrix4x4> queMatrix;

//----------------------------------------------------------------------
class Screen : public Widget
{
protected:
	NVGcontext* vg;
	int fontNormal, fontBold, fontIcons;
	int width;
	int height;


public:
	queMatrix matrix;

public:
	Screen();
	virtual ~Screen();
	int initNanoVg( NVGcontext* vg );
	bool onFrameMove( int time, int cx, int cy, eBtnState btn );
	int draw( int width, int height );
	bool isInvalid(){ return invalid; };
	void addInvalidRect( Rect & rect );
	int getWidth(){ return width; }
	int getHeight(){ return height; }
/* Event Operation is it nesseray???
	typedef deque<UiEvent> queEvent;
	queEvent events;
	void procEvent();

	int postEvent( const UiEvent & ev );
	virtual int handleEvent( const UiEvent & ev );
*/

};

} // namespace nanoui

#endif // _NANOUI_H_
