#include <iostream>
#define ARCBALL_CAMERA_IMPLEMENTATION
#include "arcball.h"
#include "Camera.h"
#include <fmt/core.h>
#include <algorithm>

//
Camera::Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float fieldOfView, float aspectRatio, float near, float far) {
	_pos_ = position_ = pos;
	_up_ = up_ = up;
	_target_ = target_ = target;
	_zoom_ = 1.5;

	//perspective
	fieldOfView_ = fieldOfView / 180.0f * 3.14159f;
	aspectRatio_ = aspectRatio;
	//ortho
	x_ = 0;
	y_ = 0;
	width_ = 800;
	height_ = 600;

	zoom_= _zoom_;
	near_ = near;
	far_ = far;

	is_perspective_ = true;

	//input state
	left_down_= false;
	right_down_ = false;
	middle_down_ = false;
	mouse_delta_ = 0;

	distance_to_center_ = glm::distance(target,pos);

	update();
}

Camera::~Camera() {
	
}

void Camera::updateMouseState(int x,int y,int button, int state)
{
	switch (button)
	{
	case 0:// GLFW_MOUSE_BUTTON_LEFT:
		left_down_ = state;
		//fmt::print("left :{}\n",state);
		break;
	case 2://GLFW_MOUSE_BUTTON_MIDDLE:
		middle_down_ = state;
		//fmt::print("middle :{}\n", state);
		break;
	case 1://GLFW_MOUSE_BUTTON_RIGHT:
		right_down_ = state;
		//fmt::print("right :{}\n", state);
		break;
	}
	cur_pos_ = { x,y };
	if (state >= 0)
	{
		last_mousepos_ = cur_pos_;
	}
	//fmt::print("[{},{}]->[{},{}]\n",last_mousepos_.x,last_mousepos_.y,cur_pos_.x,cur_pos_.y);
}
void Camera::updateMouseDelta(float delta)
{
	// std::signbit(delta) ? 0.1f : -0.1f;
#ifdef DESK_GL
	if (delta > 0)
		moveForward(delta);
	else if (delta < 0)
		moveBackward(delta);
#else
	if (delta < 0)
		moveForward(-abs(delta));
	else if (delta > 0)
		moveBackward(-abs(delta));
#endif
}

void Camera::update() {

	//if (glm::distance(cur_pos_, last_mousepos_) < 0.01)
	//	return;
	//
	arcball_camera_update(
		&position_[0], &target_[0], &up_[0],
		&viewMatrix_[0][0],
		1.0f / 60.0f,
		0.1f, // zoom per tick
		0.3f + _zoom_ * 0.5, // pan speed
		3.0f, // rotation multiplier
		width_, height_, // screen (window) size
		last_mousepos_.x, cur_pos_.x,
		last_mousepos_.y, cur_pos_.y,
		right_down_,
		left_down_,
		mouse_delta_,
		0);
	//update mouse state
	last_mousepos_ = cur_pos_;
	mouse_delta_ = 0.0f;


	// Update camera coordinate system vectors
	front_ = target_ - position_;
	front_ = glm::normalize(front_);
	right_ = glm::normalize(glm::cross(front_, up_));
	//up_ = glm::normalize(glm::cross(right_, front_));

	// Update view matrix
	//viewMatrix_ = glm::lookAt(position_, position_ + front_, up_);

	// Update projection matrix
	//perspective
	if(is_perspective_)
		projectionMatrix_ = glm::perspective(fieldOfView_, aspectRatio_, near_, far_);
	//ortho
	else
		projectionMatrix_ = glm::ortho( - zoom_ *aspectRatio_,  zoom_ * aspectRatio_,  - zoom_,  zoom_, near_, far_);

}

void Camera::resetView()
{
	position_ =_pos_ ;
	up_ = _up_;
	target_ = _target_;
	zoom_ = _zoom_;

	this->update();
}
void Camera::viewport(int x, int y, int width, int height)
{
	//perspective

	float last_aspect = aspectRatio_;
	aspectRatio_ = width / (float)height;
	if (abs(last_aspect - aspectRatio_) > 0.01)
		this->update();
	
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;

	if(!is_perspective_)
	{
		//ortho
		if (x != x_ || y != y_ || width != width_ || height != height_)
		{
			
			this->update();
		}
	}
}

void Camera::changeProjection(bool b_perspective)
{
	this->is_perspective_ = b_perspective;
}
const glm::mat4& Camera::getViewMatrix() {
	return viewMatrix_;
	//auto v2 = glm::mat4(glm::mat3(viewMatrix_));
	//return v2;
}


const glm::mat4& Camera::getProjectionMatrix() {
	return projectionMatrix_;	
}

const glm::vec3& Camera::getPosition() {
	return position_;	
}

const glm::vec3& Camera::getDirection() {
	return front_;
}

void Camera::setPosition(glm::vec3 pos) {
	position_ = pos;
}

void Camera::setDirection(glm::vec3 dir) {
	front_ = dir;
}

void Camera::moveForward(float delta) {
	//position_ += front_ * delta;
	mouse_delta_ = delta * 0.01f;

	zoom_ -= 0.05f;
	if (zoom_ < 0.1f) zoom_ = 0.1f;
}

void Camera::moveBackward(float delta) {
	//position_ -= front_ * delta;
	mouse_delta_ = -delta * 0.01f;
	zoom_ += 0.05f;
	if (zoom_ > 10.1f) zoom_ = 10.1f;
}

void Camera::moveRight(float delta) {
	position_ += right_ * delta;
}

void Camera::moveLeft(float delta) {
	position_ -= right_ * delta;
}

void Camera::moveUp(float delta) {
	position_ += worldUpVector_ * delta;
}

void Camera::moveDown(float delta) {
	position_ -= worldUpVector_ * delta;
}

void Camera::addYaw(float yaw) {

}

void Camera::addPitch(float pitch) {

}

//!mouseX (-1,1)
//!mouseY (1,-1)
std::pair<glm::vec3,glm::vec3> Camera::getRay(float mouseX,float mouseY)
{
	if(is_perspective_)
	{
		//from web
		glm::vec2 ray_nds = glm::vec2(mouseX, mouseY);
		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
		glm::mat4 invProjMat = glm::inverse(projectionMatrix_);
		glm::vec4 eyeCoords = invProjMat * ray_clip;
		eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
		glm::mat4 invViewMat = glm::inverse(viewMatrix_);
		glm::vec4 rayWorld = invViewMat * eyeCoords;
		glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

		return std::pair<glm::vec3,glm::vec3>(position_,rayDirection);
	}
	else
	{
		//! from: https://stackoverflow.com/questions/52307753/raycasting-mouse-picking-while-using-an-perspective-vs-orthographic-projection
		auto cam_pos = position_;
		auto cam_dir = glm::normalize(target_ - position_);
		auto cam_up = up_;

		auto gl_ortho_half_width =  zoom_ *aspectRatio_;
		auto gl_ortho_half_height =  zoom_;
		//get x,y coordinates relative to frustum width and height
		float x = gl_ortho_half_width * mouseX;
		float y = gl_ortho_half_height * mouseY;

		glm::vec3 cam_right = glm::normalize(glm::cross(cam_dir,cam_up));
		cam_up = glm::normalize(glm::cross(cam_right,cam_dir));

	
		auto ray_origin = cam_pos + x * cam_right + y * cam_up;
		auto ray_dir = cam_dir;

		return std::pair<glm::vec3,glm::vec3>(ray_origin,ray_dir);
	}
};


bool Camera::intersectBox(float mouseX,float mouseY,std::pair<glm::vec3,glm::vec3> aabbox)
{
	glm::vec3 ray_origin;
	glm::vec3 ray_dir;

	auto ray = getRay(mouseX,mouseY);
	ray_origin = ray.first;
	ray_dir = ray.second;

	//https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
	auto intersectAABB = [](glm::vec3 r_org,glm::vec3 r_dir,glm::vec3 lb,glm::vec3 rt,float& t)->bool{
		// r.dir is unit direction vector of ray
		glm::vec3 dirfrac;
		dirfrac.x = 1.0f / r_dir.x;
		dirfrac.y = 1.0f / r_dir.y;
		dirfrac.z = 1.0f / r_dir.z;
		// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
		// r.org is origin of ray
		float t1 = (lb.x - r_org.x)*dirfrac.x;
		float t2 = (rt.x - r_org.x)*dirfrac.x;
		float t3 = (lb.y - r_org.y)*dirfrac.y;
		float t4 = (rt.y - r_org.y)*dirfrac.y;
		float t5 = (lb.z - r_org.z)*dirfrac.z;
		float t6 = (rt.z - r_org.z)*dirfrac.z;

		float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
		if (tmax < 0)
		{
			t = tmax;
			return false;
		}

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
		{
			t = tmax;
			return false;
		}

		t = tmin;
		return true;
	};

	float t;
	return intersectAABB(ray_origin,ray_dir,aabbox.first,aabbox.second,t);
}

void Camera::toX()
{
	position_ = glm::vec3(distance_to_center_,0,0);
	up_ = glm::vec3(0,1,0);
	target_ = glm::vec3(0,0,0);
}

void Camera::toY()
{
	position_ = glm::vec3(0,distance_to_center_,0);
	up_ = glm::vec3(1,0,0);
	target_ = glm::vec3(0,0,0);
}

void Camera::toZ()
{
	position_ = glm::vec3(0,0,distance_to_center_);
	up_ = glm::vec3(0,1,0);
	target_ = glm::vec3(0,0,0);
}

void Camera::toAxis()
{
	position_ = glm::vec3(3,3,3);
	up_ = glm::vec3(0, 1, 0);
	target_ = glm::vec3(0, 0, 0);
}