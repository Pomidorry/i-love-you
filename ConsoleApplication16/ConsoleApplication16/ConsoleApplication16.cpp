#define _USE_MATH_DEFINES
#include <windows.h>
#include <gl/gl.h>
#include<cmath>
#pragma comment(lib, "opengl32.lib")

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
POINTFLOAT* mas = NULL;
int cnt;
int scaleY = 1;
void DrawOs(float alpha) {
	static float d = 0.05;
	glPushMatrix();
	glRotatef(alpha, 0, 0, 1);
	glBegin(GL_LINES);
	glVertex2f(-1, 0);
	glVertex2f(1, 0);
	glVertex2f(1, 0);
	glVertex2f(1 - d, 0 + d);
	glVertex2f(1, 0);
	glVertex2f(1 - d, 0 - d);
	glEnd();
	glPopMatrix();
}
void DrawGrid(float alpha) {
	glPushMatrix();
	glRotatef(alpha, 0, 0, 1);
	glBegin(GL_LINES);
	for (int i = -10; i < 10; i++) {
		glVertex2f(-1, 0+i*0.2);
		glVertex2f(1, 0+i*0.2);
	}
	glEnd();
	glPopMatrix();
}

void init(float start, float finish, int count) {
	cnt = count;
	mas = (POINTFLOAT*)realloc(mas, sizeof(*mas) * cnt);
	float dx = (finish - start) / (cnt - 1);
	for (int i = 0; i < cnt; i++) {

		mas[i].x = 16 * pow(sin(i), 3);
		mas[i].y = 13 * cos(i) - 5 * cos(2 * i) - 2 * cos(3 * i) - cos(4 * i);
	}
}
void show() {
	float sx = 2.0 / (mas[cnt - 1].x - mas[0].x);
	float dx = (mas[cnt - 1].x + mas[0].x) * 0.5;
	glPushMatrix();
	glScalef(0.05, 0.05, 0.05);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < cnt; i++) {
		for (int j = 0; j < cnt; j++) {
			glColor3f(0 + i * 0.0005, 0+j*0.0005, 0);
			glVertex2d(mas[i].x, mas[i].y);
		}
	}
	glEnd();
	glPopMatrix();
}
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;
	float theta = 0.0f;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"GLSample";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	/* create main window */
	hwnd = CreateWindowEx(0, L"GLSample", L"Zhopa",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000,
		1000,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);

	/* enable OpenGL for the window */
	EnableOpenGL(hwnd, &hDC, &hRC);
	init(0, 5, 1000);
	/* program main loop */
	while (!bQuit)
	{
		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			/* OpenGL animation code goes here */

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glLoadIdentity();
			glLineWidth(5);
			glColor3f(0, 1, 0);
			DrawOs(0);
			
			glColor3f(0, 1, 0);
			DrawOs(90);
			glLineWidth(1);
			DrawGrid(0);
			DrawGrid(90);
			show();
			SwapBuffers(hDC);

			theta += 1.0f;
			Sleep(1);
		}
	}

	/* shutdown OpenGL */
	DisableOpenGL(hwnd, hDC, hRC);

	/* destroy the window explicitly */
	DestroyWindow(hwnd);

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;



	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}
	break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}

