#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

class Camera {
public:

	Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float fieldOfView, float aspectRatio, float near, float far);

	~Camera();

	void updateMouseState(int x,int y,int button, int state);
	void updateMouseDelta(float delta);

	void update();
	void viewport(int x, int y, int w, int h);

	const glm::mat4& getViewMatrix();
	const glm::mat4& getProjectionMatrix();
	const glm::vec3& getPosition();
	const glm::vec3& getDirection();
	const glm::vec3& getUp(){ return up_;}

	void resetView();
	void setPosition(glm::vec3 pos);
	void setDirection(glm::vec3 dir);
	void moveForward(float delta);
	void moveBackward(float delta);
	void moveRight(float delta);
	void moveLeft(float delta);
	void moveUp(float delta);
	void moveDown(float delta);
	void addYaw(float yaw);
	void addPitch(float pitch);
	void changeProjection(bool b_perspective);

	void setTarget(const glm::vec3& tar) { target_ = tar; };
	glm::vec3 getTarget() { return target_; };
	float getZoom() { return zoom_; };


	//x
	void toX();
	//y
	void toY();
	//z
	void toZ();
	//axis
	void toAxis();

	//! get ray from camera
	std::pair<glm::vec3,glm::vec3> getRay(float mouseX,float mouseY);

	//!check box intersect
	bool intersectBox(float mouseX,float mouseY,std::pair<glm::vec3,glm::vec3> aabbox);

	glm::vec2 getScreenSize() { 
		return glm::vec2(width_, height_); 
	};

	void setView(const glm::mat4& newview){ viewMatrix_ = newview;}

protected:
	glm::mat4 viewMatrix_ = glm::mat4(1.0f);
	glm::mat4 projectionMatrix_ = glm::mat4(1.0f);

	// Camera position and rotation
	glm::vec3 position_;
	glm::vec3 target_;

	// Camera coordinate system vectors
	glm::vec3 front_;
	glm::vec3 right_;
	glm::vec3 up_;

	// Other camera properties
	glm::vec3 worldUpVector_;
	float fieldOfView_;
	float aspectRatio_;
	float near_;
	float far_;
	//ortho
	float x_,y_,width_,height_;
	float zoom_;

	bool is_perspective_;
	
	//input state
	glm::vec2 last_mousepos_,cur_pos_;
	bool left_down_;
	bool right_down_;
	bool middle_down_;
	float mouse_delta_;

	float distance_to_center_;


	//reset view
	glm::vec3 _pos_;
	glm::vec3 _target_;
	glm::vec3 _up_;
	float _zoom_;
};

#endif // CAMERA_H