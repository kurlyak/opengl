//======================================================================================
//	Ed Kurlyak 2023 Volume Fog OpenGL
//======================================================================================

#include "OpenGLHeader.h"
#include "MeshManager.h"

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{
	glDeleteProgram(m_ProgramHandlePass0);
	glDeleteProgram(m_ProgramHandlePass1);
	
	glDeleteFramebuffers(1, &m_FrameBuffer);
	
	glDeleteTextures(1, &m_RenderedTexture);
}

void CMeshManager::Init_MeshManager(UINT Width, UINT Height)
{
	m_Camera.Init(Width, Height);

	Load_BMP();

	//����� ����� 2076 �������������
	//�.�. 2076 * 3, ������ �����������
	//����� ��� �������, ������ �������
	//����� ��� ���������� x,y,z
	
	vec3* VertBuff = new vec3[2076 * 3];
	vec2* TexCoordBuff = new vec2[2076 * 3];
	//unsigned int  *IndexBuff = new unsigned int[2076 * 3];

	FILE* f;
	fopen_s(&f, "room.txt", "rt");

	char Buffer[1024];
	fgets(Buffer, 1024, f);

	//����� ���������� ������������� 2076
	int Size;
	sscanf_s(Buffer, "%d", &Size);
	
	for (int i = 0; i < 2076 * 3; i++)
	{
		fgets(Buffer, 1024, f);
		sscanf_s(Buffer, "%f %f %f %f %f", &VertBuff[i].x,
			&VertBuff[i].y,
			&VertBuff[i].z,
			&TexCoordBuff[i].x,
			&TexCoordBuff[i].y);
	}

	fclose(f);

	/*
	for (int i = 0; i < 2076; i++)
	{
		IndexBuff[i * 3 + 0] = i * 3 + 0;
		IndexBuff[i * 3 + 1] = i * 3 + 1;
		IndexBuff[i * 3 + 2] = i * 3 + 2;
	}
	*/

	glewInit();

	GLuint VboVertex;
	GLuint VboTextCoord;
	//GLuint VboIndices;

	glGenBuffers(1, &VboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, VboVertex);
    glBufferData(GL_ARRAY_BUFFER, 2076 * 3 * sizeof(vec3), VertBuff, GL_STATIC_DRAW);
	
    glGenBuffers(1, &VboTextCoord);
    glBindBuffer(GL_ARRAY_BUFFER, VboTextCoord);
    glBufferData(GL_ARRAY_BUFFER, 2076 * 3 * sizeof(vec2), TexCoordBuff, GL_STATIC_DRAW);
	
	/*
	glGenBuffers(1, &VboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2076 * 3 * sizeof(unsigned int) , IndexBuff, GL_STATIC_DRAW);
	*/
	glGenVertexArrays( 1, &m_fsCube );
	glBindVertexArray(m_fsCube);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboIndices);

	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VboVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VboTextCoord);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindVertexArray(0);

	delete[] VertBuff;
	delete[] TexCoordBuff;
	//delete[] IndexBuff;

	GLuint VboQuadVertex;
	
	float QuadVert[] = { 
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f };
	
	glGenBuffers(1, &VboQuadVertex);
    glBindBuffer(GL_ARRAY_BUFFER, VboQuadVertex);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), QuadVert, GL_STATIC_DRAW);

	glGenVertexArrays( 1, &m_fsQuad );
    glBindVertexArray(m_fsQuad);

	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VboQuadVertex);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindVertexArray(0);

	/*
	VERT AND FRAG SHADER PASS1
	*/
	GLuint VertShaderPass0 = glCreateShader( GL_VERTEX_SHADER );
	if ( 0 == VertShaderPass0)
	{
		MessageBox(NULL, "Error creating vertex shader", "Info", MB_OK);
	}

	const GLchar* ShaderCodePass1V = Load_Shader_As_String ((char*)".\\Shader\\pass0.vert");

	const GLchar* CodeArray[] = {ShaderCodePass1V};
	glShaderSource (VertShaderPass0, 1, CodeArray, NULL);

	glCompileShader(VertShaderPass0);

	GLint Result;
	glGetShaderiv( VertShaderPass0, GL_COMPILE_STATUS, &Result);
	if(GL_FALSE == Result)
	{
		MessageBox(NULL, "Vert shader compilation failed", "Info", MB_OK);

		GLint LogLen;
		glGetShaderiv(VertShaderPass0, GL_INFO_LOG_LENGTH, &LogLen);
		if(LogLen > 0)
		{
			char * Log = (char *) malloc(LogLen);
			GLsizei Written;
			glGetShaderInfoLog(VertShaderPass0, LogLen, &Written, Log);
			char Buff[1024];
			sprintf_s(Buff, 1024, "Shader Log:\n%s", Log);
			MessageBox(NULL, Buff, "Info", MB_OK);
		}
	}

	GLuint FragShaderPass0 = glCreateShader( GL_FRAGMENT_SHADER );
	if ( 0 == FragShaderPass0 )
	{
		MessageBox(NULL, "Error creating frag shader", "Info", MB_OK);
	}

	const GLchar*	ShaderCodePass1F = Load_Shader_As_String ((char*)".\\Shader\\pass0.frag");
	const GLchar* 	CodeArray_f[] = {ShaderCodePass1F};
	glShaderSource (FragShaderPass0, 1, CodeArray_f, NULL);

	glCompileShader(FragShaderPass0);

	glGetShaderiv( FragShaderPass0, GL_COMPILE_STATUS, &Result);
	if(GL_FALSE == Result)
	{
		MessageBox(NULL, "Frag shader compilation failed", "Info", MB_OK);

		GLint LogLen;
		glGetShaderiv(FragShaderPass0, GL_INFO_LOG_LENGTH, &LogLen);
		if(LogLen > 0)
		{
			char * Log = (char *) malloc(LogLen);
			GLsizei Written;
			glGetShaderInfoLog(FragShaderPass0, LogLen, &Written, Log);
			char Buff[1024];
			sprintf_s(Buff, 1024, "Shader Log:\n%s", Log);
			MessageBox(NULL, Buff, "Info", MB_OK);
		}
	}

	delete[] ShaderCodePass1V;
	delete[] ShaderCodePass1F;

	/*
	VERT AND FRAG SHADER PASS2
	*/
	GLuint VertShaderPass1 = glCreateShader( GL_VERTEX_SHADER );
	if ( 0 == VertShaderPass1 )
	{
		MessageBox(NULL, "Error creating vertex shader fog", "Info", MB_OK);
	}

	const GLchar* ShaderCodePass2V = Load_Shader_As_String ((char*)".\\Shader\\pass1.vert");
	const GLchar* CodeArrayV2[] = {ShaderCodePass2V};
	glShaderSource (VertShaderPass1, 1, CodeArrayV2, NULL);

	glCompileShader(VertShaderPass1);

	glGetShaderiv( VertShaderPass1, GL_COMPILE_STATUS, &Result);
	if(GL_FALSE == Result)
	{
		MessageBox(NULL, "Vert shader fog compilation failed", "Info", MB_OK);

		GLint LogLen;
		glGetShaderiv(VertShaderPass1, GL_INFO_LOG_LENGTH, &LogLen);
		if(LogLen > 0)
		{
			char * Log = (char *) malloc(LogLen);
			GLsizei Written;
			glGetShaderInfoLog(VertShaderPass1, LogLen, &Written, Log);
			char Buff[1024];
			sprintf_s(Buff, 1024, "Shader fog Log:\n%s", Log);
			MessageBox(NULL, Buff, "Info", MB_OK);
		}
	}

	GLuint FragShaderPass1 = glCreateShader( GL_FRAGMENT_SHADER );
	if ( 0 == FragShaderPass1 )
	{
		MessageBox(NULL, "Error creating frag fog shader", "Info", MB_OK);
	}

	const GLchar*	ShaderCodePass2F = Load_Shader_As_String ((char*)".\\Shader\\pass1.frag");
	const GLchar* 	CodeArrayF2[] = {ShaderCodePass2F};
	glShaderSource (FragShaderPass1, 1, CodeArrayF2, NULL);

	glCompileShader(FragShaderPass1);

	glGetShaderiv( FragShaderPass1, GL_COMPILE_STATUS, &Result);
	if(GL_FALSE == Result)
	{
		MessageBox(NULL, "Frag shader fog compilation failed", "Info", MB_OK);

		GLint LogLen;
		glGetShaderiv(FragShaderPass1, GL_INFO_LOG_LENGTH, &LogLen);
		if(LogLen > 0)
		{
			char * Log = (char *) malloc(LogLen);
			GLsizei Written;
			glGetShaderInfoLog(FragShaderPass1, LogLen, &Written, Log);
			char Buff[1024];
			sprintf_s(Buff, 1024, "Shader fog Log:\n%s", Log);
			MessageBox(NULL, Buff, "Info", MB_OK);
		}
	}

	delete[] ShaderCodePass2V;
	delete[] ShaderCodePass2F;


	m_ProgramHandlePass0 = glCreateProgram();

	if( 0 == m_ProgramHandlePass0)
	{
		MessageBox(NULL, "Error creating programm object", "Info", MB_OK);
	}

	m_ProgramHandlePass1 = glCreateProgram();

	if( 0 == m_ProgramHandlePass1)
	{
		MessageBox(NULL, "Error creating programm object", "Info", MB_OK);
	}

	glAttachShader(m_ProgramHandlePass0, VertShaderPass0);
	glAttachShader(m_ProgramHandlePass0, FragShaderPass0);
	
	glLinkProgram(m_ProgramHandlePass0);

	glDetachShader(m_ProgramHandlePass0, VertShaderPass0);
	glDetachShader(m_ProgramHandlePass0, FragShaderPass0);

	glDeleteShader(VertShaderPass0);
	glDeleteShader(FragShaderPass0);
	
	glAttachShader(m_ProgramHandlePass1, VertShaderPass1);
	glAttachShader(m_ProgramHandlePass1, FragShaderPass1);

	glLinkProgram(m_ProgramHandlePass1);

	glDetachShader(m_ProgramHandlePass1, VertShaderPass1);
	glDetachShader(m_ProgramHandlePass1, FragShaderPass1);
	
	glDeleteShader(VertShaderPass1);
	glDeleteShader(FragShaderPass1);

	
	m_FrameBuffer = 0;
	glGenFramebuffers(1, &m_FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

	//�������� � ������� ����� ���������
	glGenTextures(1, &m_RenderedTexture);
	
	//"Bind" �������� � ������� ����� ���������
	glBindTexture(GL_TEXTURE_2D, m_RenderedTexture);

	//������� ������ ����������� ��� ������
	//��������� 0 ������ "������"
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 800, 600, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//depth
	//���� �������� ������� ��� ��� ����� �����
	//���������� � ����������� (��� ����� �������)
	glGenTextures(1, &m_DepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_RenderedTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthTexture, 0);
	
	// ������������� ������ draw buffers.
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" ��� ������ DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) 
	{
		// ��� ������, ����� ��������
	}

	//������������ FPS �� 30 ������ � ���.
	m_Timer.TimerStart(30);
}

void CMeshManager::Update_MeshManager()
{
	m_Timer.CalculateFPS();
	float ElapsedTime = m_Timer.GetElaspedTime();

	mat4 MatModel = mat4(1.0f);
	mat4 MatView = mat4(1.0f);
	mat4 MatProjection = mat4(1.0f);

	//MatModel *= glm::rotate(glm::radians(Angle), vec3(0.0f, 1.0f, 0.0f));
	//MatModel *= glm::rotate(glm::radians(Angle), vec3(1.0f, 0.0f, 0.0f));
	//MatModel *= glm::translate(vec3(0.0f,-0.45f,0.0f));

	//MatView = glm::lookAt(vec3(25.0f, 5.0f, 6000.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
	
	MatView = m_Camera.Frame_Move(ElapsedTime);
	
	MatProjection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 50000.0f);

	m_MatMVP = MatProjection * MatView * MatModel;

}

void CMeshManager::Draw_MeshManager()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	//PASS 1 ������ ��� � ��������
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	
	glEnable (GL_DEPTH_TEST);

	glViewport(0,0,800,600);

	glClearColor(0.0f, 0.125f, 0.3f, 1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUseProgram(m_ProgramHandlePass0);
	glBindVertexArray(m_fsCube);
	
	GLuint MatMVP = glGetUniformLocation(m_ProgramHandlePass0, "MVP");
	if (MatMVP >= 0)
	{
		glUniformMatrix4fv(MatMVP, 1, GL_FALSE, (const GLfloat*)& m_MatMVP);
	}

	GLuint CamPos = glGetUniformLocation(m_ProgramHandlePass0, "gCamPos");
	if (CamPos >= 0)
	{
		glUniform3fv(CamPos, 1, (const GLfloat*)& m_Camera.vCamPos);
	}
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, m_TexID );

	GLuint Tex = glGetUniformLocation(m_ProgramHandlePass0, "ourTexture");
	if( Tex >= 0 )
    {
        glUniform1i(Tex, 0);
    }

	//������ �����
	//glDrawElements(GL_TRIANGLES, 2076 * 3, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 2076 * 3);
	
	glBindVertexArray(0);
	glUseProgram(0);
	
	//PASS2 ������ �������� � screen aligned quad

	glEnable (GL_DEPTH_TEST);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0,800,600);

	glClearColor(0.0f, 0.125f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_ProgramHandlePass1);
	glBindVertexArray(m_fsQuad);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, m_RenderedTexture );

	Tex = glGetUniformLocation(m_ProgramHandlePass1, "ourTexture");
	if( Tex >= 0 )
    {
        glUniform1i(Tex, 0);
    }

	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glBindVertexArray(0);
	glUseProgram(0);
}

GLchar* CMeshManager::Load_Shader_As_String(char* Fn)
{

	FILE* Fp;
	char* Content = NULL;

	unsigned int Count = 0;

	if (Fn != NULL)
	{

		errno_t Err;
		Err = fopen_s(&Fp, Fn, "rt");

		if (Err == NULL)
		{

			fseek(Fp, 0, SEEK_END);
			Count = ftell(Fp);
			rewind(Fp);

			if (Count > 0)
			{
				Content = (char*)malloc(sizeof(char) * (Count + 1));
				Count = fread(Content, sizeof(char), Count, Fp);
				Content[Count] = '\0';
			}
			fclose(Fp);
		}
	}
	return Content;
}

bool CMeshManager::Load_BMP()
{
	//��������� BMP ���� ��� ������ � �������� ������
	FILE* fp = NULL;
	fopen_s(&fp, "Room.bmp", "rb");
	if (fp == NULL) printf("Error Open File");

	//������ ��������� ����� ��������
	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);

	//������ ��������� ����� ��������
	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(BITMAPINFOHEADER), 1, fp);

	//���������� �� ������ BMP ����� �� ������ ������
	fseek(fp, bfh.bfOffBits, SEEK_SET);

	//��������� �� ������ ���� ���������� �� BMP �����
	unsigned char* pRes;
	pRes = new unsigned char[bih.biHeight * bih.biWidth * 3];
	//������ �� ����� rgb ������ �����������
	fread(pRes, bih.biHeight * bih.biWidth * 3, 1, fp);

	//��������� �������� ��������� ����
	fclose(fp);

	unsigned char* pRes_temp;
	pRes_temp = new unsigned char[bih.biHeight * bih.biWidth * 3];

	int index = 0;
	for (int h = 0; h < bih.biHeight; h++)
	{
		for (int w = 0; w < bih.biWidth; w++)
		{
			index = (h * bih.biWidth + w) * 3;
			//������ RGB �������
			pRes_temp[index + 0] = pRes[index + 2];
			pRes_temp[index + 1] = pRes[index + 1];
			pRes_temp[index + 2] = pRes[index + 0];
		}
	}

	glGenTextures(1, &m_TexID);
	glBindTexture(GL_TEXTURE_2D, m_TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bih.biWidth, bih.biHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, pRes_temp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	delete[] pRes;
	delete[] pRes_temp;

	return TRUE;
}

