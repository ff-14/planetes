#include "fltkopt.h"
#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_draw.H>
#include <math.h>
#include <string>
#include <utility>

struct game_object
{
	int w;
	int h;
	int type;
	Fl_PNG_Image* image_1;
	Fl_PNG_Image* image_2;
	Fl_PNG_Image* image_3;
	Fl_PNG_Image* image_4;
};

#define STATE_NORMAL 1
#define STATE_DEAD 2
#define STATE_ON_GROUND 4

struct object_instance
{
	int x;
	int y;
	int px;
	int py;
	int state;
	Fl_PNG_Image* image;
	struct game_object* parent;
};

struct list
{
	struct object_instance* obj;
	struct list* next;
};

struct list* HEAD;
struct list* CREEPERS;


struct game_object PLAYER;
struct game_object CREEPER;
struct game_object WALL;

struct object_instance PLAYER_INSTANCE;
Fl_PNG_Image* fatality;
Fl_PNG_Image* panorama;
bool show_fatality = false;
int run = 0;
int creep_count = 0;
int score = 0;
int delta = 6;

int obj_left(struct list* l)
{
	return l->obj->x;
}

int obj_right(struct list* l)
{
	return l->obj->x + l->obj->parent->w;
}

void obj_draw(struct list* l, int offset)
{
	l->obj->image->draw(l->obj->x - offset, l->obj->y);
}

void obj_draw_right(struct list* l, int offset, int edge)
{
	l->obj->image->draw(l->obj->x - offset, l->obj->y, edge - l->obj->x, l->obj->parent->h);
}

void obj_draw_left(struct list* l, int offset, int edge)
{
	l->obj->image->draw(0, l->obj->y, obj_right(l) - offset, l->obj->parent->h, l->obj->parent->w - (obj_right(l) - offset));
}

class Fl_Draw_Box : public Fl_Double_Window{


 //left x coordinate

	public: 
		int w, h;
		int left_x;
		int left_edge() { return left_x; }
		int right_edge() { return left_x + w; }

	Fl_Draw_Box(int w, int h, const char* test) : Fl_Double_Window(w, h, test)
	{
		this->w = w;
		this->h = h;
		left_x = 0;
	}


	virtual void draw() {
		Fl_Double_Window::draw();
		panorama->draw(0, 0, w, h, 305 + left_x,0);
		//fl_rectf(0, 0, w, h, 0x60, 0x60, 0xA0);
		struct list* l = HEAD;
		while (l->next != NULL)
		{
			if (left_edge() > obj_right(l) || right_edge() < obj_left(l))
			{
				//do nothing				
			} else {
				if (l->obj == &PLAYER_INSTANCE)
				{
					if (obj_left(l) >= left_edge() && obj_right(l) > right_edge() - 300)
					{
						left_x += delta;
					}
					if (obj_left(l) < left_edge() + 150 && obj_right(l) <= right_edge())
					{
						left_x -= delta;
						//if (obj_left(l) > 0)
						//	left_x -= w / 2;
					}
				}

				if (obj_left(l) >= left_edge() && obj_right(l) <= right_edge())
				{
					obj_draw(l, left_edge());

				}

				if (obj_left(l) >= left_edge() && obj_right(l) > right_edge())
				{
					
					obj_draw_right(l, left_edge(), right_edge());
				}
				
				if (obj_left(l) < left_edge() && obj_right(l) <= right_edge())
				{
					obj_draw_left(l, left_edge(), right_edge());
				}
			}
			l = l->next;
		}
		if (show_fatality)
		{
			fatality->draw(w/2 - 384/2, h/2-128/2); //384 x 128
		}
		char buf[256];
		_snprintf(buf, 256, "Score : %d", score);
		fl_color(0xff, 0xff, 0xff);
		fl_draw(buf, 10, 20);
		int font = fl_font();
		int size = fl_size();
		if (score == creep_count)
		{
			_snprintf(buf, 256, "THANK YOU %%STUDENT_NAME%%"); 
			fl_color(0xff, 0x00, 0x00);
			fl_font(FL_TIMES, 40);
			fl_draw(buf, w/2 - 300, h/2 - 20);
			_snprintf(buf, 256, "BUT YOUR PROJECT IS NOT FOUND");
			fl_draw(buf, w / 2 - 350, h / 2 + 60);
			_snprintf(buf, 256, "ON GITHUB");
			fl_draw(buf, w / 2 - 100, h / 2 + 100);
		}
		fl_font(font, size);
	};

	virtual int handle(int ev)
	{
		for (int i = 0; i < 10; i++)
		{
			if (run > i * 50 && run < (i + 1) * 50)
			{
				delta = 6 * (i + 1);
			}
		}
		
		if (ev == FL_KEYDOWN) {
			uchar key = Fl::event_key();
			switch (key)
			{
			case 'w':
				if (PLAYER_INSTANCE.state & STATE_ON_GROUND)
				{
					
					PLAYER_INSTANCE.py = -20;
				}
				
				return 1;
			case 's':
				return 1;
			case 'a':
				PLAYER_INSTANCE.px = -delta;
				return 1;
			case 'd':
				PLAYER_INSTANCE.px = delta;
				return 1;
			default: return 0;
			}
		}
		else if (ev == FL_KEYUP)
		{
			uchar key = Fl::event_key();
			switch (key)
			{
			case 'w':
				return 1;
			case 's':
				return 1;
			case 'a':
				PLAYER_INSTANCE.px = 0;
				delta = 6;
				return 1;
			case 'd':
				PLAYER_INSTANCE.px = 0;
				delta = 6;
				return 1;
			default: return 0;
			}
		}

	  return Fl_Window::handle(ev);
	}
	
};


void callback(void* winp)
{
Fl_Draw_Box * win=(Fl_Draw_Box *)winp;

win->redraw();

Fl::repeat_timeout(0.04,callback, win);
}

void handle_gravity(struct object_instance* o, Fl_Draw_Box * win)
{
	if (o->state & STATE_DEAD)
	{
		o->px = 0;
	}
	int gravity = 1;

	o->x += o->px;
	if (o->x < 0)
	{
		o->x = 0;
	}

	if (o->y >= win->h - o->parent->h - 10)
	{
		if (o->py > 0)
		{
			o->py = 0;
		}
		o->y = win->h - o->parent->h - 10 + o->py;
		o->state |= STATE_ON_GROUND;
		if (o == &PLAYER_INSTANCE)
		{
			run++;
			//printf("%d\n", run);
		}
		
	}
	else {
		if (o == &PLAYER_INSTANCE)
		{
			run = 0;
		}
		o->py += gravity;
		o->y += o->py;
		o->state &= ~(STATE_ON_GROUND); //clear state bit
	}
}

bool collision(int w, int h, int x1, int x2, int y1, int y2)
{
	return	((x1 <= w) && (y1 <= h) && (x1 >= 0) && (y1 >= 0)) || ((x2 <= w) && (y1 <= h) && (x2 >= 0) && (y1 >= 0)) || \
			((x1 <= w) && (y2 <= h) && (x1 >= 0) && (y2 >= 0)) || ((x2 <= w) && (y2 <= h) && (x2 >= 0) && (y2 >= 0));
}

void handle_creepers(Fl_Draw_Box * win)
{
	//1. check if collide
	//2. if yes -> check y coordinate, equal -> lose, too small overlap -> lose, you need to jump over them
	
	struct list* creep = CREEPERS;

	int w = PLAYER_INSTANCE.parent->w, h = PLAYER_INSTANCE.parent->h;
	
	while (creep->next != NULL)
	{
		int x1 = -PLAYER_INSTANCE.x + creep->obj->x;
		int x2 = -PLAYER_INSTANCE.x + creep->obj->x + creep->obj->parent->w;
		int y1 = -PLAYER_INSTANCE.y + creep->obj->y;
		int y2 = -PLAYER_INSTANCE.y + creep->obj->y + creep->obj->parent->h;
		
		if ((creep->obj->state & STATE_NORMAL) && (PLAYER_INSTANCE.state & STATE_NORMAL) && collision(w, h, x1, x2, y1, y2))
		{
			
			if (y1 > 0)
			{
				printf("win\n");
				//you win
				creep->obj->state |= STATE_DEAD;
				creep->obj->state &= ~(STATE_NORMAL);
				creep->obj->image = CREEPER.image_2;
				score++;
				
			} else {
				printf("lose\n");
				PLAYER_INSTANCE.state &= ~(STATE_NORMAL);
				PLAYER_INSTANCE.state |= STATE_DEAD;
				PLAYER_INSTANCE.image = PLAYER.image_2;
				//you lose
			}
		}

		
		creep = creep->next;
	}
}

void callback_fatality(void* winp)
{
	show_fatality = true;
}

void callback_creepers(void* winp)
{

	struct list* creep = CREEPERS;
	while (creep->next != NULL)
	{
		if (creep->obj->state & STATE_DEAD)
		{

		}
		else {
			int px = rand();
			int py = rand();
			creep->obj->px = (px - RAND_MAX / 2) * 15.0 / (RAND_MAX / 2);
			if (creep->obj->state & STATE_ON_GROUND)
			{
				creep->obj->py = -py * 28.0 / RAND_MAX;
			}
			
		}
		
		creep = creep->next;
	}
	Fl::repeat_timeout(2, callback_creepers, winp);
}

void callback_physics(void* winp)
{
	Fl_Draw_Box * win = (Fl_Draw_Box *)winp;
	if (PLAYER_INSTANCE.state & STATE_DEAD)
	{
		PLAYER_INSTANCE.y -= 10;
		Fl::add_timeout(3, callback_fatality, winp);
	}
	else {
		handle_gravity(&PLAYER_INSTANCE, win);
	}
	
	
	//CREEPERS handling

	struct list* creep = CREEPERS;
	while (creep->next != NULL)
	{
		handle_gravity(creep->obj, win);
		creep = creep->next;
	}
	handle_creepers(win);
	
	Fl::repeat_timeout(0.04, callback_physics, win);
}

void add_object(struct list* l, struct game_object* o, int x, int y) //WARNING: uses HEAD global pointer
{
	struct object_instance* obj = new struct object_instance;
	obj->x = x;
	obj->y = y;
	obj->py = 0;
	obj->px = 0;
	obj->parent = o;
	struct list* n = new struct list;
	n->next = HEAD;
	n->obj = obj;
	HEAD = n;

	if (o == &CREEPER)
	{
		struct list* m = new struct list;
		m->next = CREEPERS;
		m->obj = obj;
		CREEPERS = m;
		obj->image = CREEPER.image_1;
		obj->state = STATE_NORMAL;
		creep_count++;
	}
}

int main(int argc, char * argv[]) 
{

	Fl_Draw_Box* window = new Fl_Draw_Box(820, 610, "mario");
	// Init player an list of objects
	PLAYER.h = 50;
	PLAYER.w = 50;
	PLAYER.image_1 = new Fl_PNG_Image("mario_1.png");
	PLAYER.image_2 = new Fl_PNG_Image("mario_2.png");
	PLAYER_INSTANCE.image = PLAYER.image_1;
	CREEPER.h = 50;
	CREEPER.w = 50;
	CREEPER.image_1 = new Fl_PNG_Image("creeper_1.png");
	CREEPER.image_2 = new Fl_PNG_Image("creeper_2.png");

	panorama = new Fl_PNG_Image("panorama.png");
	fatality = new Fl_PNG_Image("fatality.png");

	PLAYER_INSTANCE.parent = &PLAYER;
	PLAYER_INSTANCE.x = 50;
	PLAYER_INSTANCE.y = 50;
	PLAYER_INSTANCE.state = STATE_NORMAL;

	CREEPERS = new struct list;
	CREEPERS->next = NULL;
	HEAD = new struct list;
	HEAD->next = NULL;
	struct list * PL = new struct list;
	PL->obj = &PLAYER_INSTANCE;
	PL->next = HEAD;
	HEAD = PL;

	add_object(HEAD, &CREEPER, 250, 200);
	add_object(HEAD, &CREEPER, 400, 200);
	add_object(HEAD, &CREEPER, 300, 200);
	for (int i = 0; i < 100; i++)
	{
		add_object(HEAD, &CREEPER, (i+5) * 300, 200);
	}

	window->show(argc, argv);
	Fl::add_timeout(0.1, callback, window);
	Fl::add_timeout(0.09, callback_physics, window);
	Fl::add_timeout(0.09, callback_creepers, window);
	return Fl::run();
}




