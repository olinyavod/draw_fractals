#pragma once

#include <SDL.h>

inline SDL_Point rotate(SDL_Point offset, double r, double angle)
{
	auto x = static_cast<int>(r * cos(angle * M_PI / 180.0));
	int y = static_cast<int>(-r * sin(angle * M_PI / 180.0));
		
	x += offset.x;
	y += offset.y;
		
	return { x, y };
}

class RenderObjectBase
{
protected:
	int offset_x_ = 0, offset_y_ = 0;
public:
	void set_offset(int offset_x, int offset_y)
	{
		offset_x_ = offset_x;
		offset_y_ = offset_y;
	}
	
	virtual ~RenderObjectBase() = default;
	virtual void on_loop(long current_tick) = 0;
	virtual  void on_render(SDL_Renderer* renderer) = 0;
	virtual void reset() = 0;
	virtual  void stop() = 0;
};