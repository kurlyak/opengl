//======================================================================================
//	Ed Kurlyak 2023 Volume Fog OpenGL
//======================================================================================

#include "OpenGLHeader.h"
#include "Camera.h"

float CFirstPersonCamera::Vec3_Dot(vec3 v1, vec3 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
vec3 CFirstPersonCamera::Vec3_Add(vec3 v1, vec3 v2)
{
	vec3 t;

	t.x = v1.x + v2.x;
	t.y = v1.y + v2.y;
	t.z = v1.z + v2.z;

	return t;
}

vec3 CFirstPersonCamera::Vec3_Cross(vec3 v1, vec3 v2)
{
	vec3 t;

	t.x = v1.y * v2.z - v1.z * v2.y;
	t.y = v1.z * v2.x - v1.x * v2.z;
	t.z = v1.x * v2.y - v1.y * v2.x;

	return t;
}

vec3 CFirstPersonCamera::Vec3_Normalize(vec3 v)
{
	vec3 t;

	float vlen = sqrtf( (v.x * v.x) + (v.y * v.y) + (v.z * v.z));

	t.x = v.x/vlen;
	t.y = v.y/vlen;
	t.z = v.z/vlen;

	return t;
}

vec3 CFirstPersonCamera::Vec3_Scale(vec3 v, float val)
{
	vec3 t;

	t.x = v.x * val;
	t.y = v.y * val;
	t.z = v.z * val;

	return t;
}

vec3 CFirstPersonCamera::Vec3_Transform(vec3 v, mat4 mat) 
{

	vec3 t;

	float w = 1.0;

	t.x = v.x * mat[0][0] + v.y * mat[1][0] + v.z * mat[2][0] + 1*mat[3][0];
	t.y = v.x * mat[0][1] + v.y * mat[1][1] + v.z * mat[2][1] + 1*mat[3][1];
	t.z = v.x * mat[0][2] + v.y * mat[1][2] + v.z * mat[2][2] + 1*mat[3][2];
	//w = v.x * mat[0][3] + v.y * mat[1][3] + v.z * mat[2][3] + 1*mat[3][3];

	/*
	t.x = t.x / w;
	t.y = t.y / w;
	t.z = t.z / w;
	*/

	return t;

}

mat4 CFirstPersonCamera::Matrix_Rotation_Axis(vec3 v, float angle)
{
		float x = v.x;
		float y = v.y;
		float z = v.z;
	
		float s = sinf(angle);
		float c = cosf(angle);
		float omc = (float)1.0f - c;

		float xomc = x * omc;
		float yomc = y * omc;
		float zomc = z * omc;

		float xxomc = x * xomc;
		float xyomc = x * yomc;
		float xzomc = x * zomc;
		
		float yyomc = y * yomc;
		float yzomc = y * zomc;
		float zzomc = z * zomc;

		float xs = x * s;
		float ys = y * s;
		float zs = z * s;

		return mat4(xxomc + c,  xyomc + zs, xzomc - ys, 0.0f,
			xyomc - zs, yyomc + c,  yzomc + xs, 0.0f,
			xzomc + ys, yzomc - xs, zzomc + c, 0.0f,
			0.0f ,0.0f, 0.0f, 1.0f);
}

void CFirstPersonCamera::Init(int Width, int Height)
{
	vRight        = vec3( 1.0, 0.0, 0.0);
    vUp           = vec3( 0.0, 1.0, 0.0);
    vLook         = vec3( 0.0, 0.0, 1.0);
    vCamPos          = vec3(25.0f, 5.0f, 6000.0f);

	ScreenWidth = Width;
	ScreenHeight = Height;

	ShowCursor(FALSE);
	
}

mat4 CFirstPersonCamera::Frame_Move(float fTime)
{
	
	POINT mousePos;
	GetCursorPos(&mousePos);

	SetCursorPos(ScreenWidth/2, ScreenHeight/2);
    
	int nDeltaX=ScreenWidth/2-mousePos.x;
	int nDeltaY=ScreenHeight/2-mousePos.y;
	
	//что бы камера перемещалась вверх- вниз
	//(вертикально) сделайте значение больше нуля
	//float m_fRotationScalerY = 0.025;
	float m_fRotationScalerY = 0.0;
	//для перемещения камеры горизонтально
	float m_fRotationScalerX = 0.025f;

	if(nDeltaY<0) m_fRotationScalerY = -m_fRotationScalerY;
	else if(nDeltaY>0) m_fRotationScalerY = m_fRotationScalerY;
	else if(nDeltaY==0) m_fRotationScalerY = 0;

	mat4 matRotRight = Matrix_Rotation_Axis(vRight , m_fRotationScalerY);
	vRight = Vec3_Transform(vRight, matRotRight);
	vUp = Vec3_Transform(vUp, matRotRight);
	vLook = Vec3_Transform(vLook, matRotRight);

	if(nDeltaX<0) m_fRotationScalerX = m_fRotationScalerX;
	else if(nDeltaX>0) m_fRotationScalerX = -m_fRotationScalerX;
	else if(nDeltaX==0) m_fRotationScalerX = 0;

	vec3 vUpTemp = vec3( 0.0f, 1.0f, 0.0f );
	mat4 matRotUp = Matrix_Rotation_Axis(vUpTemp , m_fRotationScalerX);
	
	vRight = Vec3_Transform(vRight, matRotUp);
	vUp = Vec3_Transform(vUp, matRotUp);
	vLook = Vec3_Transform(vLook, matRotUp);
	
	//реакция на нажатия клавиш
	float ratioMove = 5000;
	vec3 temp = vec3(0,0,0);;
	vec3 vAccel = vec3(0,0,0);
	
	if(GetAsyncKeyState('W')& 0xFF00) 
	{
		temp = vec3(vLook.x,0,vLook.z);
		temp = Vec3_Scale(temp, -ratioMove);
		vAccel = Vec3_Scale(temp, fTime);
		vAccel.y = 0.0;
	}

	if(GetAsyncKeyState('S')& 0xFF00) 
	{
		temp = vec3(vLook.x,0,vLook.z);
		temp = Vec3_Scale(temp, ratioMove);
		vAccel = Vec3_Scale(temp, fTime);
	}

	if(GetAsyncKeyState('D')& 0xFF00) 
	{
		temp = vec3(vRight.x,0,vRight.z);
		temp = Vec3_Scale(temp, -ratioMove);
		vAccel = Vec3_Scale(temp, fTime);
	}

	if(GetAsyncKeyState('A')& 0xFF00) 
	{
		temp = vec3(vRight.x,0,vRight.z);
		temp = Vec3_Scale(temp, ratioMove);
		vAccel = Vec3_Scale(temp, fTime);
	}
	
	vAccel.y = 0.0;
	
	vCamPos = Vec3_Add(vCamPos,vAccel);
	
	vLook = Vec3_Normalize(vLook);
	vUp = Vec3_Cross(vLook,vRight);
	vUp = Vec3_Normalize(vUp);
	vRight = Vec3_Cross(vUp,vLook);
	vRight = Vec3_Normalize(vRight);
	
	float x = -Vec3_Dot(vCamPos, vRight);
	float y = -Vec3_Dot(vCamPos, vUp);
	float z = -Vec3_Dot(vCamPos, vLook);
	
	mat4 MatView = mat4(vRight.x, vUp.x, vLook.x, 0.0f,
		vRight.y, vUp.y, vLook.y, 0.0f,
		vRight.z, vUp.z, vLook.z, 0.0f,
		x,y,z, 1.0f);
		
	return MatView;
}

