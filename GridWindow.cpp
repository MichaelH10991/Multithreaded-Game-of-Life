
#include "GridWindow.h"

using namespace Gdiplus;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void drawGrid(HWND hwnd, HDC hdc);

// Create and display the window
GridWindow::GridWindow(int width, int height, Grid& grid, HINSTANCE hInstance, INT iCmdShow) {

	gridArray1 = grid;

	// Setup 2nd (double-buffer) grid
	gridArray2 = Grid(gridArray1.getNumElements());

	arrayIndex = 0; // Default to grid array 'gridArray1'


	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	WNDCLASS wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("CIS6023");

	RegisterClass(&wndClass);

	hwnd = CreateWindow(
		TEXT("CIS6023"),		  // window class name
		TEXT("Game of Life"),     // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		width,					  // initial x size
		height,		              // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		(void*)this);             // creation parameters

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	okayToUpdate = TRUE;
}


// Main event loop
void GridWindow::mainLoop(void(*updateFn)(Grid&, Grid&)) {

	MSG msg;

	while (1) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {

			if (okayToUpdate) {

				if (arrayIndex == 0) {

					updateFn(gridArray1, gridArray2);
					arrayIndex = 1;
				}
				else {

					updateFn(gridArray2, gridArray1);
					arrayIndex = 0;
				}

				okayToUpdate = FALSE;

				RECT clientRect;
				GetClientRect(hwnd, &clientRect);
				InvalidateRect(hwnd, &clientRect, FALSE);
			}
		}
	}

	GdiplusShutdown(gdiplusToken);
}


// Main window event handler
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{

	case WM_CREATE:

		if (1) {

			LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
			GridWindow *window = (GridWindow*)createStruct->lpCreateParams;
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(window));
		}
		return 0;

	case WM_PAINT:

		hdc = BeginPaint(hwnd, &ps);
		drawGrid(hwnd, hdc);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}


// Draw the main grid
VOID drawGrid(HWND hwnd, HDC hdc) {

	GridWindow *window = reinterpret_cast<GridWindow*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

	Graphics graphics(hdc);

	Pen pen(Color(255, 0, 0, 0));
	SolidBrush blackBrush(Color(255, 0, 0, 0));
	SolidBrush whiteBrush(Color(255, 255, 255, 255));

	Grid G = window->getGridArray();
	int n = G.getNumElements();

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	const int hMargin = 5;
	const int vMargin = 5;

	int hExtent = (clientRect.right - clientRect.left) - (hMargin << 1);
	int vExtent = (clientRect.bottom - clientRect.top) - (vMargin << 1);

	int hSize = hExtent / n;
	int vSize = vExtent / n;
	
	// Grid stores vectors in column-major format
	for (int x = 0, baseX = hMargin; x < n; ++x, baseX += hSize) {

		for (int y = 0, baseY = vMargin; y < n; ++y, baseY += vSize) {

			if (G[x][y] != 0) {

				graphics.FillRectangle(&blackBrush, baseX, baseY, hSize, vSize);
			}
			else {

				graphics.FillRectangle(&whiteBrush, baseX, baseY, hSize, vSize);
			}

			graphics.DrawRectangle(&pen, baseX, baseY, hSize, vSize);
		}
	}

	window->setOkayToUpdate(TRUE);
}
