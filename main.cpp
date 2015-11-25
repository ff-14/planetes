#include "fltkopt.h"
#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <math.h>
#include <string>
#include <utility>

class Fl_Draw_Box : public Fl_Window{
	
	public: 
	float X,Y,R;
	int w,h;
	int dy;
	bool first;
	bool red;
	Fl_Draw_Box(int w, int h, const char* test) : Fl_Window(w,h,test) 
	{
		this->w = w;
		this->h = h;
		X = w/2;
		Y = h/2;
		R = 20;
		dy=5;
		first =true;
		red = true;
	}

	virtual void draw() {
		Fl_Window::draw();

		uchar  rgb[] = {255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0};
		fl_draw_image(rgb, 0, 0,2, 2, 3, 0);
		
		fl_color(255,255,255);
		fl_circle(X,Y,R);
		
		if (Y +dy < 0+R)
		{
			dy =-dy;
		}	

		if (Y +dy > h-R)
		{
			dy =-dy;
		}	
		Y+=dy;
		if (red) {
		fl_color(255,0,0);
		} else {
		fl_color(0,255,255);
		}
		fl_circle(X,Y,R);
	};

	virtual int handle(int ev)
	{
	  switch(ev) {
		case FL_KEYDOWN:
		  if (Fl::event_key() == 'x') {
			red = false;
			redraw();
			return 1;
		  } else {
			return 0;
		  }
		case FL_KEYUP:
		  if (Fl::event_key() == 'x') {
			red = true;
			redraw();
			return 1;
		  } else {
			return 0;
		  }
		default:
		  return Fl_Window::handle(ev);
	  }
	}
	
};


void callback(void* winp)
{
Fl_Draw_Box * win=(Fl_Draw_Box *)winp;

win->redraw();

//Fl::repeat_timeout(0.04,callback, win);
Fl::repeat_timeout(1,callback, win);
}

class Pair {
public:
	Fl_Input* input;
	Fl_Button* button;
	
	Pair(Fl_Input* i, Fl_Button* b)
	{
		input = i;
		button = b;
	}
};


void button_callback(Fl_Widget* widget, void* data) {
	Pair* pair = (Pair *) data;
	Fl_Button* label = pair->button;
	Fl_Input* input = pair->input;
	label->label(input->value());
	
	printf(input->value());
	
}




int main(int argc, char * argv[]) 
{
	Fl_Draw_Box* window = new Fl_Draw_Box(600, 400, "test");

	Fl_Button* button = new Fl_Button (20,20, 100, 30, "PUSH ME");
	Fl_Button* label = new Fl_Button (20,60, 100, 30, 0);
	label->label("HELLO");
	Fl_Input* input = new Fl_Input (20,100, 100, 30, 0);
	Pair * pair = new Pair(input, label);
	button->callback(button_callback, pair); 
	
	window->show(argc, argv);
	Fl::add_timeout(1.0, callback, window);

	return Fl::run();
}

