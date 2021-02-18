#pragma once

#include <map>
#include <SDL.h>
#include <string>


#include "base.hpp"

class LSystemRender : public RenderObjectBase
{
private:
	std::string end_string_;
	const float angle_;
	const int real_w_;
	const int real_h_;
	const float speed_;
	SDL_Texture* tex_target_;
	const float length_;
	long old_tick_ = 0;
	SDL_Point pt_start_, pt_end_;
	bool first_render_ = true;
	size_t current_index_ = 0;
	float current_angle_ = 90;

	static std::string create_l_system(const int iterations, 
	                                   const std::string& axiom, 
	                                   const std::map<char, std::string>& rules)
	{
		std::string start_string(axiom);

		if (iterations == 0)
			return start_string;
		
		std::string end_string;

		for(int i =0; i < iterations; i++)
		{
			end_string = "";

			for (auto ch : start_string)
			{
				auto r = rules.find(ch);

				if (r == rules.end())
					end_string += ch;
				else
					end_string += r->second;				
			}

			start_string = end_string;
		}

		return end_string;
	}

public:
	LSystemRender(
		SDL_Renderer* renderer,
		const int screen_w,
		const int screen_h,
		const int real_w,
		const int real_h,
		const int iterations, 
		const std::string& axiom, 
		const std::map<char, std::string>& rules,
		const float angle,
		const float speed = 10,
		const float length = 8)
	: angle_(angle), real_w_(real_w), real_h_(real_h), speed_(speed), length_(length)
{
		end_string_ = create_l_system(iterations, axiom, rules);

		tex_target_ = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET,
			screen_w,
			screen_h);

		pt_start_ = pt_end_ = { screen_w / 2, screen_h / 2 };
	}

	void on_loop(long current_tick) override
	{
		if(old_tick_+10 > current_tick)
			return;

		old_tick_ = current_tick;

		if(current_index_ >= end_string_.size()-1)
			return;

		while (current_index_ < end_string_.size())
		{
			const auto cmd = end_string_[current_index_];

			current_index_++;
			
			switch (cmd)
			{
			case 'F':
				pt_start_ = pt_end_;
				pt_end_ = rotate(pt_start_, length_, current_angle_);				
				return;
			case '+':
				current_angle_ -= angle_;
				break;
			case '-':
				current_angle_ += angle_;
				break;
			}
		}
	}
	
	void on_render(SDL_Renderer* renderer) override
	{
		SDL_SetRenderTarget(renderer, tex_target_);

		if (first_render_)
			SDL_RenderClear(renderer);

		first_render_ = false;

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		
		SDL_RenderDrawLine(renderer, pt_start_.x, pt_start_.y, pt_end_.x, pt_end_.y);
		
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
		first_render_ = true;
		current_index_ = 0;
	}
	
	void stop() override
	{
		
	}

	~LSystemRender() override
	{
		SDL_DestroyTexture(tex_target_);
	}
};
