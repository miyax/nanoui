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
#include "nanoui.h"

namespace nanoui {

#define ICON_SEARCH 0x1F50D
#define ICON_CIRCLED_CROSS 0x2716
#define ICON_CHEVRON_RIGHT 0xE75E
#define ICON_CHECK 0x2713
#define ICON_LOGIN 0xE740
#define ICON_TRASH 0xE729

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
	

Widget::Widget()
{
	invalid = true;
	state = stIDLE;
	
	draggable = true;
	dragging = false;
	//Position drag_point;

}

Widget::~Widget()
{
	
}
	
void Widget::onHoverCursol( int x, int y)
{
	//printf("onHoverCursol (%d,%d)\n",x,y);
}

void Widget::onHoverMoveCursol( int x, int y)
{
	//printf("onHoverMoveCursol (%d,%d)\n",x,y);
}

void Widget::onClick()
{
	if( cbks[WE_ON_CLICK] != NULL )
	{
		(*cbks[WE_ON_CLICK])(this);
	}
	printf("%s onClick\n", name.c_str());
}

void Widget::onButtonOn()
{
	printf("onButtonOn\n");
}

void Widget::onDragMoveCursol( int x, int y )
{
	//printf("onDragMoveCursol\n");
}

void Widget::onLeaveCursol()
{
	printf("onLeaveCursol\n");
}
	
bool Widget::onButtonEvnet( Screen * sp, float x, float y, eBtnState btnstate )
{
	Matrix4x4 tmtx;
	tmtx = sp->matrix.back() * this->matrix;
	sp->matrix.push_back( tmtx );
	for( int i=0; i<items.size(); i++ )
	{
		if( items[i]->onButtonEvnet( sp,x,y, btnstate) == true )
		{
			sp->matrix.pop_back();
			return true;
		}
	}
	sp->matrix.pop_back(); 
	
	float lefttop_x = 0;
	float lefttop_y = 0;
	float rightbottom_x = size.w;
	float rightbottom_y = size.h;
	
	tmtx.transform( lefttop_x, lefttop_y );
	tmtx.transform( rightbottom_x, rightbottom_y );
	
	// inside region ?
	if( x > lefttop_x && y > lefttop_y && x < rightbottom_x && y < rightbottom_y )
	{
		switch(btnstate)
		{
		case btnOFF:
			switch( state )
			{
			case stIDLE:
				state = stHOVER;
				onHoverCursol(x,y);
				break;
			case stHOVER:
				onHoverMoveCursol(x,y);
				break;
			case stON:
				state = stHOVER;
				onClick();
				break;
			}
			dragging = false;
			break;
		case btnON:
			switch( state )
			{
			case stIDLE:
				state = stON;
				onButtonOn();
				break;
			case stHOVER:
				state = stON;
				onButtonOn();
				break;
			case stON:
				onDragMoveCursol(x,y);
				
				if( dragging == false && draggable )
				{
					dragging = true;
					drag_point.x = x-this->pos.x;
					drag_point.y = y-this->pos.y;
				}
				if( dragging )
				{
					this->pos.x = x-drag_point.x;
					this->pos.y = y-drag_point.y; 
					this->matrix.loadIdentity();
					this->matrix.translate( this->pos.x, this->pos.y , 0.0f );
				}
				break;
			}
			break;
		}
		
		invalid = true;
		return invalid;
	
	// outside region	
	}else{
		
		switch(btnstate)
		{
		case btnOFF:
			switch( state )
			{
			case stIDLE:
				// do nothing
				break;
			case stHOVER:
				state = stIDLE;
				onLeaveCursol();
				break;
			case stON:
				state = stIDLE;
				onLeaveCursol();			
				break;
			}
			dragging = false;
			break;
		case btnON:
		
			if( dragging )
			{
				this->pos.x = x-drag_point.x;
				this->pos.y = y-drag_point.y;
				this->matrix.loadIdentity();
				this->matrix.translate( this->pos.x, this->pos.y , 0.0f );
				invalid = true; 
			}else{
				switch( state )
				{
				case stIDLE:
					// do nothing
					break;
				case stHOVER:
					state = stIDLE;
					onLeaveCursol();			
					break;
				case stON:
					state = stIDLE;
					onLeaveCursol();
					break;
				}
			}
			break;
		}
		
		
	}
	
	return invalid;
	
	
}
int Widget::connect( eEvent ev, shared_ptr<EventCallBack> cb )
{
	cbks[ev] = cb;
}

void Widget::addWidget( shared_ptr<Widget> item )
{ 
	items.push_back(item);
	invalid = true;
} 

void Widget::draw( Screen * sp, NVGcontext* vg )
{
	invalid=false;
	
	


	
	for( int i=0; i<items.size(); i++ )
	{
		items[i]->draw( sp,vg );
	}
	
}


Panel::Panel()
{
	
}

Panel::Panel( const char * name , const char * title, int x, int y, int width, int height )
{
	this->name = name;
	this->title = title;
	pos.x = x;
	pos.y = y;
	matrix.translate( x, y, 0.0f );
	size.w = width;
	size.h = height;
	row = 34;
}

Panel::~Panel()
{
	
}

void Panel::addWidget( shared_ptr<Widget> item )
{ 
	item->pos.x = 4;
	item->pos.y = row;
	item->matrix.translate( item->pos.x, item->pos.y, 0.0f );
	
	// 
	if( FIT_PARENT == item->size.w )
	{
		item->size.w = this->size.w - (4*2);
	}else if( WRAP_CONTENT == item->size.w ) 
	{
		// TODO
		item->size.w = this->size.w - (4*2);
	}
	 
	// 
	if( FIT_PARENT == item->size.h )
	{
		item->size.h = this->size.h - row - (4*2);
	}else if( WRAP_CONTENT == item->size.h ) 
	{
		item->size.h = 32;
	} 
	
	row += item->size.h + 4;
	items.push_back(item);
	invalid = true;
} 

void Panel::draw( Screen * sp, NVGcontext* vg  )
{
	float x = 0;
	float y = 0;
	float w = size.w;
	float h = size.h;
		
	float cornerRadius = 3.0f;
	NVGpaint shadowPaint;
	NVGpaint headerPaint;
	
	nvgSave(vg);
	float m[6];
	matrix.getMatrix2x3( m );
	nvgTransform( vg, m[0],m[1],m[2],m[3],m[4],m[5] ); 

	nvgSave(vg);
	
	// Window
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x,y, w,h, cornerRadius);
	nvgFillColor(vg, nvgRGBA(28,30,34,192));
	nvgFill(vg);

	// Drop shadow
	shadowPaint = nvgBoxGradient(vg, x,y+2, w,h, cornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
	nvgBeginPath(vg);
	nvgRect(vg, x-10,y-10, w+20,h+30);
	nvgRoundedRect(vg, x,y, w,h, cornerRadius);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, shadowPaint);
	nvgFill(vg);

	// Header
	headerPaint = nvgLinearGradient(vg, x,y,x,y+15, nvgRGBA(255,255,255,8), nvgRGBA(0,0,0,16));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+1,y+1, w-2,30, cornerRadius-1);
	nvgFillPaint(vg, headerPaint);
	nvgFill(vg);
	nvgBeginPath(vg);
	nvgMoveTo(vg, x+0.5f, y+0.5f+30);
	nvgLineTo(vg, x+0.5f+w-1, y+0.5f+30);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,32));
	nvgStroke(vg);

	nvgFontSize(vg, 18.0f);
	nvgFontFace(vg, "sans-bold");
	nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);

	nvgFontBlur(vg,2);
	nvgFillColor(vg, nvgRGBA(0,0,0,128));
	nvgText(vg, x+w/2,y+16+1, title.c_str(), NULL);

	nvgFontBlur(vg,0);
	nvgFillColor(vg, nvgRGBA(220,220,220,160));
	nvgText(vg, x+w/2,y+16, title.c_str(), NULL);

	nvgRestore(vg);
	invalid=false;
	
	Widget::draw(sp,vg);
	nvgRestore(vg);
}

Button::Button()
{
	draggable = false;
	preicon=0;

}

Button::~Button()
{
	
}

Button::Button( const char * name , const char * title, int x, int y, int width, int height  )
{
	draggable = false;
	preicon=0;
	colIdle = nvgRGBA(0,96,128,255);
	colActive = nvgRGBA(0,96*2,255,255);
	this->name = name;
	this->title = title;
	pos.x = x;
	pos.y = y;
	size.w = width;
	size.h = height;

}

void Button::draw( Screen * sp, NVGcontext* vg )
{
	NVGpaint bg;
	NVGcolor col;
	char icon[8];
	float cornerRadius = 4.0f;
	float tw = 0, iw = 0;
	
	float x = 0;
	float y = 0;
	float w = size.w;
	float h = size.h;

	nvgSave(vg);
	float m[6];
	matrix.getMatrix2x3( m );
	nvgTransform( vg, m[0],m[1],m[2],m[3],m[4],m[5] ); 
	
	if( state == stON )
	{
		col = colActive;
	}else{
		col = colIdle;
	}
	

	bg = nvgLinearGradient(vg, x,y,x,y+h, nvgRGBA(255,255,255,isBlack(col)?16:32), nvgRGBA(0,0,0,isBlack(col)?16:32));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+1,y+1, w-2,h-2, cornerRadius-1);
	if (!isBlack(col)) {
		nvgFillColor(vg, col);
		nvgFill(vg);
	}
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+0.5f,y+0.5f, w-1,h-1, cornerRadius-0.5f);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,48));
	nvgStroke(vg);

	nvgFontSize(vg, 20.0f);
	nvgFontFace(vg, "sans-bold");
	tw = nvgTextBounds(vg, 0,0, title.c_str(), NULL, NULL);
	if (preicon != 0) {
		nvgFontSize(vg, h*1.3f);
		nvgFontFace(vg, "icons");
		iw = nvgTextBounds(vg, 0,0, cpToUTF8(preicon,icon), NULL, NULL);
		iw += h*0.15f;
	}

	if (preicon != 0) {
		nvgFontSize(vg, h*1.3f);
		nvgFontFace(vg, "icons");
		nvgFillColor(vg, nvgRGBA(255,255,255,96));
		nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
		nvgText(vg, x+w*0.5f-tw*0.5f-iw*0.75f, y+h*0.5f, cpToUTF8(preicon,icon), NULL);
	}

	nvgFontSize(vg, 20.0f);
	nvgFontFace(vg, "sans-bold");
	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	nvgFillColor(vg, nvgRGBA(0,0,0,160));
	nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f-1,title.c_str(), NULL);
	nvgFillColor(vg, nvgRGBA(255,255,255,160));
	nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f,title.c_str(), NULL);
		
	Widget::draw(sp,vg);
	nvgRestore(vg);
	
}

CheckButton::CheckButton()
{
	
}
	
CheckButton::CheckButton( const char * name , const char * title, int x, int y, int width, int height  )
{
	draggable = false;
	this->name = name;
	this->title = title;
	pos.x = x;
	pos.y = y;
	size.w = width;
	size.h = height;	
	check_state = UnChecked;
}
	
CheckButton::~CheckButton( )
{

	
}

void CheckButton::onClick()
{
	switch(check_state)
	{
	case UnChecked:
		check_state = Checked;
		break;
	case Checked:
		check_state = UnChecked;
		break;
	}
	invalid=true;
}
	
void CheckButton::draw( Screen * sp, NVGcontext* vg )
{
	NVGpaint bg;
	char icon[8];

	
	float x = 0;
	float y = 0;
	float w = size.w;
	float h = size.h;
	
	NVG_NOTUSED(w);

	nvgSave(vg);
	float m[6];
	matrix.getMatrix2x3( m );
	nvgTransform( vg, m[0],m[1],m[2],m[3],m[4],m[5] ); 
	

	nvgFontSize(vg, 18.0f);
	nvgFontFace(vg, "sans");
	nvgFillColor(vg, nvgRGBA(255,255,255,160));

	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	nvgText(vg, x+28,y+h*0.5f,title.c_str(), NULL);

	bg = nvgBoxGradient(vg, x+1,y+(int)(h*0.5f)-9+1, 18,18, 3,3, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,92));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+1,y+(int)(h*0.5f)-9, 18,18, 3);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	if( Checked == check_state )
	{
		nvgFontSize(vg, 40);
		nvgFontFace(vg, "icons");
		nvgFillColor(vg, nvgRGBA(255,255,255,128));
		nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
		nvgText(vg, x+9+2, y+h*0.5f, cpToUTF8(ICON_CHECK,icon), NULL);
	}
	
	Widget::draw(sp,vg);
	nvgRestore(vg);	
}

Slider::Slider()
{
	draggable = false;
}
	
Slider::Slider( const char * name , const char * title, int x, int y, int width, int height  )
{
	draggable = false;
	this->name = name;
	pos.x = x;
	pos.y = y;
	size.w = width;
	size.h = height;
	slider_pos = 0.5f;	
}
	
Slider::~Slider( )
{

	
}

void Slider::draw( Screen * sp, NVGcontext* vg )
{
	float x = 0;
	float y = 0;
	float w = size.w;
	float h = size.h;
	
	NVG_NOTUSED(w);

	nvgSave(vg);
	float m[6];
	matrix.getMatrix2x3( m );
	nvgTransform( vg, m[0],m[1],m[2],m[3],m[4],m[5] ); 
	

	NVGpaint bg, knob;
	float cy = y+(int)(h*0.5f);
	float kr = (int)(h*0.25f);

	nvgSave(vg);
//	nvgClearState(vg);

	// Slot
	bg = nvgBoxGradient(vg, x,cy-2+1, w,4, 2,2, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,128));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x,cy-2, w,4, 2);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	// Knob Shadow
	bg = nvgRadialGradient(vg, x+(int)(slider_pos*w),cy+1, kr-3,kr+3, nvgRGBA(0,0,0,64), nvgRGBA(0,0,0,0));
	nvgBeginPath(vg);
	nvgRect(vg, x+(int)(slider_pos*w)-kr-5,cy-kr-5,kr*2+5+5,kr*2+5+5+3);
	nvgCircle(vg, x+(int)(slider_pos*w),cy, kr);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	// Knob
	knob = nvgLinearGradient(vg, x,cy-kr,x,cy+kr, nvgRGBA(255,255,255,16), nvgRGBA(0,0,0,16));
	nvgBeginPath(vg);
	nvgCircle(vg, x+(int)(slider_pos*w),cy, kr-1);
	nvgFillColor(vg, nvgRGBA(40,43,48,255));
	nvgFill(vg);
	nvgFillPaint(vg, knob);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, x+(int)(slider_pos*w),cy, kr-0.5f);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,92));
	nvgStroke(vg);

	nvgRestore(vg);
	
	Widget::draw(sp,vg);
	nvgRestore(vg);	
}

Screen::Screen()
{
	
}

Screen::~Screen()
{
	
}

int Screen::initNanoVg( NVGcontext* vg )
{
	this->vg = vg;
	fontIcons = nvgCreateFont(vg, "icons", "../res/entypo.ttf");
	if (fontIcons == -1) {
		//printf("Could not add font icons.\n");
		return -1;
	}
	fontNormal = nvgCreateFont(vg, "sans", "../res/Roboto-Regular.ttf");
	if (fontNormal == -1) {
		//printf("Could not add font italic.\n");
		return -1;
	}
	fontBold = nvgCreateFont(vg, "sans-bold", "../res/Roboto-Bold.ttf");
	if (fontBold == -1) {
		//printf("Could not add font bold.\n");
		return -1;
	}
	return 0;
}

bool Screen::onFrameMove( int time, int cx, int cy, eBtnState btn )
{
	Matrix4x4 tmx;
	matrix.clear();
	matrix.push_back( tmx );
	for( int i=0; i<items.size(); i++ )
	{
		if( items[i]->onButtonEvnet( this, cx,cy, btn) == true )
		{
			return true;
		}
	}
	return invalid;
}
	
int Screen::draw( int width, int height )
{

	glViewport(0, 0, width, height);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
		
	nvgBeginFrame(vg, width, height, 1.0);
	
	Matrix4x4 tmx;
	matrix.clear();
	matrix.push_back( tmx );
		
	for( int i=0; i<items.size(); i++ )
	{
		items[i]->draw(this,vg);
	}
		
	nvgEndFrame(vg);
	glEnable(GL_DEPTH_TEST);
				
	return 0;
		
}

}
	
