#ifndef GLOBAL_H
#define GLOBAL_H

#include <utility>

struct WindowState {
	int width, height;
};

class Global {
public:
	/// returns width and height correspondingly
	static std::pair<int, int>	get_window_size()						{return std::make_pair(window.width, window.height);}
	static void					set_window_size(int width, int height)	{window = {width, height};}
	static float				get_aspect_ratio()						{return (float)window.width/(float)window.height;}
	
private:
	inline static WindowState window = {.width = 1920, .height = 1080};
	
};

#endif