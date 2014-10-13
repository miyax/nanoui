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
#  include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "nanovg.h"

#include <vector>
#include <deque>
#include <string>
#include <memory>
using namespace std;

#include "nanoui_matrix.h"

struct Position
{
	float x;
	float y;
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

enum eEvent
{
	WE_ON_CLICK =0,
	WE_MAX
};

class Widget;
typedef vector<shared_ptr<Widget>> vecWidget;
class Screen;

struct  EventCallBack {
   virtual void operator()(Widget * p) {
        
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
	void onHoverCursol( int x, int y);
	void onHoverMoveCursol( int x, int y);
	void onClick();
	void onButtonOn();
	void onDragMoveCursol( int x, int y );
	void onLeaveCursol();
	
	bool draggable;
	bool dragging;
	Position drag_point;
	
	
	
	
	shared_ptr<EventCallBack> cbks[WE_MAX];
	
	int isBlack(NVGcolor col)
	{
		if( col.r == 0.0f && col.g == 0.0f && col.b == 0.0f && col.a == 0.0f )
		{
			return 1;
		}
		return 0;
	}
	
	static char* cpToUTF8(int cp, char* str)
	{
		int n = 0;
		if (cp < 0x80) n = 1;
		else if (cp < 0x800) n = 2;
		else if (cp < 0x10000) n = 3;
		else if (cp < 0x200000) n = 4;
		else if (cp < 0x4000000) n = 5;
		else if (cp <= 0x7fffffff) n = 6;
		str[n] = '\0';
		switch (n) {
		case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
		case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
		case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
		case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
		case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
		case 1: str[0] = cp;
		}
		return str;
	}

public:	
	Position pos;
	Size size;
	Matrix4x4 matrix;


	Widget();
	virtual ~Widget();
	virtual void addWidget( shared_ptr<Widget> item );
	int connect( eEvent ev, shared_ptr<EventCallBack>  cb );
	bool onButtonEvnet( Screen * sp, float x, float y, eBtnState btnstate );
	virtual void draw( Screen * sp, NVGcontext* vg );
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
	virtual ~Button( );
	virtual void draw( Screen * sp, NVGcontext* vg );

};

typedef deque<Matrix4x4> queMatrix;

//----------------------------------------------------------------------
class Screen : public Widget
{
protected:
	NVGcontext* vg;
	int fontNormal, fontBold, fontIcons; 
	
public:
	queMatrix matrix;
	
public:
	Screen();
	virtual ~Screen();	
	int initNanoVg( NVGcontext* vg );
	bool onFrameMove( int time, int cx, int cy, eBtnState btn );
	int draw( int width, int height );
};


#endif // _NANOUI_H_
