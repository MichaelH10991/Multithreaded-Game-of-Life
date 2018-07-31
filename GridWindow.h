#pragma once

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Grid.h"


// GridWindow is a class the represents an on-screen window and contains the 2D array (Grid) to display in the window
class GridWindow {

	// Private Attributes

private:

	HWND								hwnd;
	Gdiplus::GdiplusStartupInput		gdiplusStartupInput;
	ULONG_PTR							gdiplusToken;
	Grid								gridArray1, gridArray2;
	INT									arrayIndex; // 0->gridArray1; 1->gridArray2
	BOOLEAN								okayToUpdate;


	// Public methods

public:

	// Constructor
	GridWindow(int width, int height, Grid& grid, HINSTANCE hInstance, INT iCmdShow);
	
	// Main event loop
	void mainLoop(void(*updateFn)(Grid&, Grid&));


	// Accessor methods
	Grid& getGridArray() {

		return (arrayIndex == 0) ? gridArray1 : gridArray2;
	}

	BOOLEAN getOkayToUpdate() {

		return okayToUpdate;
	}

	void setOkayToUpdate(BOOLEAN value) {

		okayToUpdate = value;
	}
};
