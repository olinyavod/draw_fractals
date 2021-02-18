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
			if(!is_mouse_down 
				|| event.button.button != SDL_BUTTON_LEFT)
				return;
			
			is_mouse_down = false;
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

	/*current_fractal_ = new PythagorasTree(p_renderer,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		REAL_WIDTH,
		REAL_HEIGHT, 10);*/

	current_fractal_ = new LSystemRender(p_renderer,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		REAL_WIDTH,
		REAL_HEIGHT,
		10,
		"F",
		{{'F', "F-F+F"}},
		120);
	
	fractals_.push_back(current_fractal_);
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
			//std::cout << "FPS: " << frameCount << std::endl;
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
