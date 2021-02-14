#pragma once

#include <iostream>
#include <list>

#include <SDL.h>

#include "base.hpp"

class Branch
{
private:
	double curr_angle;
	long old_time_ = 0;
	float speed_;
	float max_len_, curr_len = 0;
	Uint32 color_;
	SDL_Point pt_start_, pt_end_;
	int last_index_=1;
	bool is_deleted_=false;
	
	
public:
	
	Branch(const SDL_Point pt_start,
	       const float max_len,
	       const double angle,
	       const Uint32 color,
	       const float speed = 10)
	{
		speed_ = speed;
		max_len_ = max_len;
		curr_angle = angle;
		color_ = color;

		pt_end_ = pt_start_ = pt_start;
	}
	
	bool get_is_deleted() const {return is_deleted_;}
	
	void on_loop(std::list<Branch*>& new_branches, long current_tick)
	{
		if (speed_ <= 0 || (old_time_ + 100) > current_tick)
			return;
	
		if(curr_len >= max_len_)
		{
			const auto new_len = max_len_ / 1.5f;
			const auto new_speed = speed_*0.8;

			speed_ = new_speed;
			Uint32 a=static_cast<Uint8>(color_>>24);
			Uint32 r=static_cast<Uint8>(color_>>16);
			Uint32 g=static_cast<Uint8>(color_>>8);
			Uint32 b=static_cast<Uint8>(color_);
			
			r = static_cast<Uint32>(r / 1.3);
			g = static_cast<Uint32>(g / 1.05);
			b = static_cast<Uint32>(b / 1.3);
			
			auto color = (a<<24)|(r<<16)|(g<<8)|b;
			
			if(new_len > 2)
			{
				new_branches.push_back(new Branch(pt_end_,
					new_len,
					curr_angle + 45,
					color,
					new_speed));
				
				last_index_++;
				curr_angle -= 45;
				curr_len = 0;
				max_len_ = new_len;

				pt_start_ = pt_end_;
			}
			else
			{
				speed_ = 0;
				is_deleted_ = true;
			}
			return;
		
		}
		
		old_time_ = current_tick;
		
		curr_len = std::min(curr_len + speed_, max_len_);
					
		pt_end_ = rotate(pt_start_, curr_len, curr_angle);
	}
	
	void on_render(SDL_Renderer* pRenderer)
	{
		SDL_SetRenderDrawColor(pRenderer,
	 		static_cast<Uint8>(color_>>16), 
			static_cast<Uint8>(color_>>8), 
			static_cast<Uint8>(color_), 
			0xFF);
		
		SDL_RenderDrawLine(pRenderer, pt_start_.x, pt_start_.y, pt_end_.x, pt_end_.y);
	}
	
	~Branch()
	{
	}
};

class PythagorasTree :public RenderObjectBase
{
private:
	std::list<Branch*> branches_;
	SDL_Texture* tex_target_ = nullptr;
	int real_w_, real_h_;
	SDL_Point pt_start_;
	float speed_, max_len_;
	bool runnibg_ = false, first_render_ = true;

	void clear_branches()
	{
		for (const auto* b : branches_)
			delete b;

		branches_.clear();
	}
public:

	PythagorasTree(SDL_Renderer* renderer,
		const int screen_w,
		const int screen_h,
		const int real_w,
		const int real_h,
		const float speed = 10)
		: real_w_(real_w), real_h_(real_h), pt_start_({screen_w/2, screen_h}), speed_(speed), max_len_(screen_h/3)
	{
		tex_target_ = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET,
			screen_w,
			screen_h);
	}

	void on_loop(long current_tick) override
	{
		if (!runnibg_)
			return;
				
		std::list<Branch*> new_branches;
		std::list<Branch*> remove_branches;

		for (auto* b : branches_) {
			b->on_loop(new_branches, current_tick);
			
			if (b->get_is_deleted())
				remove_branches.push_back(b);
		}

		for (auto* b : new_branches)
			branches_.push_back(b);

		for (auto* b : remove_branches)
		{
			branches_.remove(b);
			delete b;
		}
	}
	
	void on_render(SDL_Renderer* const renderer) override
	{
		SDL_SetRenderTarget(renderer, tex_target_);

		if(first_render_)
			SDL_RenderClear(renderer);

		first_render_ = false;

		for (auto b : branches_)
			b->on_render(renderer);
		
		SDL_SetRenderTarget(renderer, nullptr);
		SDL_Rect rect;
		rect.x = offset_x_;
		rect.y = offset_y_;
		rect.w = real_w_;
		rect.h = real_h_;
		SDL_RenderCopy(renderer, tex_target_, nullptr, &rect);
	}
	
	void reset() override
	{
		clear_branches();
		runnibg_ = true;
		first_render_ = true;

		branches_.push_back(new Branch(pt_start_, max_len_, 90, 0xFFAAFFAA, speed_));
	}
	
	void stop() override
	{
		runnibg_ = false;
	}

	~PythagorasTree() override
	{
		clear_branches();
		SDL_DestroyTexture(tex_target_);
	}
	
};
