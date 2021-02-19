#include <iostream>
#include <vector>

#include <SDL.h>

#include "base.hpp"
#include "pythagoras_tree.hpp"
#include "l_system_render.hpp"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
#if __WIN32__

const int REAL_WIDTH = SCREEN_WIDTH;
const int REAL_HEIGHT = SCREEN_HEIGHT;

#else

const int REAL_WIDTH = 1250;
const int REAL_HEIGHT = 1600;

#endif

SDL_Window* p_window = nullptr;
SDL_Renderer* p_renderer = nullptr;
double scale_x = 1, scale_y=1;
bool is_mouse_down = false;
SDL_Point start_point, current_point;
Uint32 offset_x=0, offset_y=0;
std::vector<RenderObjectBase*> fractals_;
RenderObjectBase* current_fractal_;
int current_index = 0;

void on_loop(const long current_tick)
{
	if(is_mouse_down)
	{
		offset_x=(current_point.x-start_point.x);
		offset_y=(current_point.y-start_point.y);
	}
	else
	{
		offset_x=offset_y=0;
	}

	current_fractal_->set_offset(offset_x, 0);
	current_fractal_->on_loop(current_tick);
}

void on_render()
{
	SDL_SetRenderDrawColor(p_renderer, 0x00, 0x00, 0x00, 0xff);
	
	SDL_RenderClear(p_renderer);
	
	current_fractal_->on_render(p_renderer);
	
	SDL_RenderPresent(p_renderer);
}

void onEvent(const SDL_Event& event, bool* quit){
	switch(event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
		{
			if(is_mouse_down 
				|| event.button.button != SDL_BUTTON_LEFT)
				return;
			
			is_mouse_down=true;
			start_point= 
				current_point={0, 0};
			
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			if (!is_mouse_down
				|| event.button.button != SDL_BUTTON_LEFT)
				return;

			is_mouse_down = false;
			const auto distance = current_point.x - start_point.x;
			if (std::abs(distance) < REAL_WIDTH / 2)
				return;

			current_index += distance < 0 ? 1 : -1;
			if (current_index == fractals_.size())
				current_index = 0;
			if (current_index < 0)
				current_index = fractals_.size() - 1;

			current_fractal_->stop();
			current_fractal_ = fractals_[current_index];
			current_fractal_->reset();
			break;
		}
		
		case SDL_MOUSEMOTION:
		{
			if(!is_mouse_down)
				return;
			current_point={event.motion.x, event.motion.y};
			
			if(start_point.x==0 && start_point.y==0)
				start_point=current_point;			
			break;
		}

		case SDL_KEYUP:
		{
			switch (event.key.keysym.scancode)
			{
				case SDL_SCANCODE_ESCAPE:
					*quit = true;
					break;
			}
			break;
		}
		
		case SDL_QUIT:
			*quit = true;
			break;
	}
}

bool on_init()
{
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS))
	{
		std::cerr << " Init SDL error: " << SDL_GetError() << ".\n";
		return false;
	}
	p_window = SDL_CreateWindow("Factorials",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	
	if(p_window == nullptr)
		return false;
	
	p_renderer = SDL_CreateRenderer(
		p_window, 
		-1, 
		SDL_RENDERER_ACCELERATED 
		| SDL_RENDERER_TARGETTEXTURE);

	fractals_.push_back(new PythagorasTree(p_renderer,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		REAL_WIDTH,
		REAL_HEIGHT, 10));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F--F--F",
			{ {'F', "F+F--F+F"} },
			60,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }));
	
		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			3,
			"F+F+F+F",
			{ {'F', "F-F+F+FFF-F-F+F"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }));
	
		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F+F+F+F",
			{ {'F', "FF+F++F+F"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }));	

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F--F",
			{ {'F', "F-F+F+F-F"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }));
	
		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F-F-F-F",
			{ {'F', "F-F+F+F-F"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }));
	
		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			12,
			"F",
			{ {'F', "+F--F+"} },
			45,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }));
	
		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			10,
			"YF",
			{
				{'X',"YF+XF+Y"}, {'Y',"XF-YF-X"}
			},
			60,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }));
	
		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			7,
			"FXF--FF--FF",
			{
				 {'F',"FF"}, {'X',"--FXF++FXF++FXF--"}
			},
			60,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT-10 },
			10,
			4, 
			0));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F+F+F+F",
			{ {'F',"FF+F+F+F+FF"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			100,
			4,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F+F+F+F",
			{ {'F',"FF+F-F+F+FF"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			100,
			10,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			3,
			"F+F+F+F",
			{ {'F',"FF+F+F+F+F+F-F"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F+F+F+F",
			{ {'F',"F+F-F+F+F"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			5,
			"F++F++F++F++F",
			{ {'F',"F++F++F+++++F-F++F"} },
			36,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			6,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			3,
			"F+F+F+F",
			{ {'F',"-F+F-F-F+F+FF-F+F+FF+F-F-FF+FF-FF+F+F-FF-F-F+FF-F-F+F+F-F+"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			6,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"FX",
			{
				{ 'X', "X+YF++YF-FX--FXFX-YF+" },
				{ 'Y', "-FX+YFYF++YF+FX--FX-Y" }
			},
			60,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			5,
			"F+XF+F+XF",
			{ {'X',"XF-F+F-XF+F+XF-F+F-X"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			6,
			90));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			6,
			"-X--X",
			{ {'X',"XFX--XFX"} },
			45,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			6,
			0));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			3,
			"YF",
			{
				{'X', "XFX-YF-YF+FX+FX-YF-YFFX+YF+FXFXYF-FX+YF+FXFX+YF-FXYF-YF-FX+FX+YFYF-"},
				{'Y', "+FXFX-YF-YF+FX+FXYF+FX-YFYF-FX-YF+FXYFYF-FX-YFFX+FX+YF-YF-FX+FX+YFY"}
			},
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			6,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			5,
			"LFL-F-LFL",
			{
				{'L', "+RF-LFL-FR+"},
				{'R', "-LF+RFR+FL-"}
			},
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			6,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			8,
			"L",
			{
				{'L', "+RF-LFL-FR+"},
				{'R', "-LF+RFR+FL-"}
			},
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			6,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"X",
			{
				{'X', "XFYFX+F+YFXFY-F-XFYFX"},
				{'Y', "YFXFY-F-XFYFX+F+YFXFY"}
			},
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			6,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F",
			{{'F', "F+F-F-F-F+F+F+F-F"}},
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			0));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F+F+F+F",
			{ {'F', "F+FF++F+F"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			0));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			4,
			"F+F+F",
			{ {'F', "F-F+F"} },
			120,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			45));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			12,
			"FX",
			{ {'X', "X+YF+"}, {'Y', "-FX-Y" } },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			0));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			8,
			"F",
			{ {'F', "F-F+F"} },
			120,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			0));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			12,
			"FX+FX",
			{ {'X',"X+YF+"}, {'Y', "-FX-Y"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			0));

		fractals_.push_back(new LSystemRender(p_renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			REAL_WIDTH,
			REAL_HEIGHT,
			12,
			"FX+FX+FX",
			{ {'X',"X+YF+"}, {'Y', "-FX-Y"} },
			90,
			{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
			20,
			10,
			0));

	current_fractal_ = fractals_[current_index];
	current_fractal_->reset();
	
	return true;	
}

int main(int argc, char* argv[])
{
	if(!on_init())
		return 1;

	auto quit = false;
   
    long frameCount = 0, oldTicks = SDL_GetTicks();
   	while(!quit)
	{
		SDL_Event event;
		
		while(SDL_PollEvent(&event))
		{
			onEvent(event, &quit);
			SDL_PumpEvents();
		}
		auto tick = SDL_GetTicks();

   		on_loop(tick);

   		on_render();
		
		frameCount++;
		
		if(oldTicks + 1000 < tick)
		{
			oldTicks = tick;
			std::cout << "FPS: " << frameCount << std::endl;
			frameCount = 0;
		}
	}

	for (auto* f : fractals_)
		delete f;
	
	SDL_DestroyRenderer(p_renderer);
    SDL_DestroyWindow(p_window);
	
	SDL_Quit();
    return 0;
}
