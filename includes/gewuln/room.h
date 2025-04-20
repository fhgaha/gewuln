#ifndef ROOM_H
#define ROOM_H

#include <unordered_map>
#include <gewuln/camera/camera.h>
#include <gewuln/camera/camera_look_at.h>
#include <gewuln/camera/camera_fly.h>
#include <memory>
#include <utility>

class Room
{
public:
	std::unordered_map<std::string, std::unique_ptr<Camera>>	cameras;	//create using new i assume?
	Camera														*initial_cam;
	Camera														*active_cam;
	
	//TODO
	//room exits
	//initial_character_position
	
	~Room() {
		// for (auto& [name, cam] : cameras) {
		// 	delete cam;
		// }
		
		delete initial_cam;
		delete active_cam;
	}
	
	
	void Init(){
		// cameras["fly_cam"] = std::make_unique<CameraFly>(
		//     glm::vec3(-3.228f, 3.582f, 4.333f),
		// 	glm::vec3(0.0f, 1.0f, 0.0f),
		// 	-39.0f,
		// 	41.0f
		// );
	}
		
	
};

#endif