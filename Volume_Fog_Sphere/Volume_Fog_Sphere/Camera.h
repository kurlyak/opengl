//======================================================================================
//	Ed Kurlyak 2023 Volume Fog OpenGL
//======================================================================================

#ifndef _CAMERA_
#define _CAMERA_

#include "OpenGLHeader.h"

#include "glm/glm.hpp"

using glm::mat4;
//using glm::mat3;
using glm::vec3;
//using glm::vec2;

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

class CFirstPersonCamera
{
public:

	CFirstPersonCamera(){};
	~CFirstPersonCamera(){};
	void Init_Camera(int Width, int Height);
	mat4 Frame_Move(float fTime);
	mat4 Matrix_Rotation_Axis(vec3 v, float angle);
	vec3 Vec3_Transform(vec3 v, mat4 mat) ;
	vec3 Vec3_Scale(vec3 v, float val);
	vec3 Vec3_Normalize(vec3 v);
	vec3 Vec3_Cross(vec3 v1, vec3 v2);
	vec3 Vec3_Add(vec3 v1, vec3 v2);
	float Vec3_Dot(vec3 v1, vec3 v2);

	vec3 vCamPos;

private:
	vec3 vRight, vUp, vLook;
	int ScreenWidth, ScreenHeight;
};

#endif