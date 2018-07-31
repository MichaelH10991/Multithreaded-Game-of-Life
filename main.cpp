#include "GridWindow.h"
#include <iostream>
#include <tbb\tbb.h>
#include <random>
#include <chrono>
#include <cstdlib>
#include "tbb/partitioner.h"

using namespace std;
using namespace tbb;

#pragma warning(disable : 4996)
#pragma warning( disable: 588) 
//global variables
int c = 0;
tick_count t0 = tick_count::now();



//parallel update being used, sequential commented out, will also need to change the event loop
void initialise(Grid& gridArray);

//void update(Grid& inputArray, Grid& outputArray);
void parallelUpdate(Grid& inputArray, Grid& outputArray);

// Main start point for the programme.  This sets up a 2D array (Grid) and creates the window to display the grid.
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	// *** Create a 2D array (Grid) and call the initialise function to set it's elements
	// Example array size (n x n)
	const int n = 20;

	Grid myArray = Grid(n);
	initialise(myArray);


	// Create and show a window that draws the 2D array.  This example creates a window (400 x 400) pixels in size.
	GridWindow mainWindow = GridWindow(400, 400, myArray, hInstance, iCmdShow);

	// Run the main event loop.  This calls the update function defined below to modify the 2D array elements
	//parallel update
	mainWindow.mainLoop(parallelUpdate);

	//sequantial update
	//mainWindow.mainLoop(update);

	return 0;
}

void initialise(Grid& gridArray) {

	const int n = gridArray.getNumElements();

	parallel_for(

		blocked_range2d<int, int>(0, n, 0, n),

		[&](blocked_range2d<int, int>& range) {

		int yStart = range.rows().begin();
		int yEnd = range.rows().end();

		for (int y = yStart; y < yEnd; y++) {

			int xStart = range.cols().begin();
			int xEnd = range.cols().end();

			for (int x = xStart; x < xEnd; x++) {

				//Creates a glider on the grid
				gridArray[1][1] = 1;
				gridArray[2][2] = 1;
				gridArray[3][1] = 1;
				gridArray[3][2] = 1;
				gridArray[2][3] = 1;

			}
		}



	});
}

// Update function - this is called automatically from the window's event loop.  This takes an input array (inputArray) and returns the result of any processing in 'outputArray'.  Here you can set the output array values.

void update(Grid& inputArray, Grid& outputArray) {

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);

	const int n = inputArray.getNumElements();



	// For now, this example just copies the current (input) array to the output array using a normal serial loop

	for (int y = 1; y < n - 1; ++y) {

		for (int x = 1; x < n - 1; ++x) {

			int neighbour = 0;

			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					//adding up states
					neighbour += inputArray[x + i][y + j];
				}
			}
			

			neighbour -= inputArray[x][y];

			//rules
			if ((inputArray[x][y] == 1) && (neighbour < 2)) {
				outputArray[x][y] = 0;
			}
			else if ((inputArray[x][y] == 1) && (neighbour > 3)) {
				outputArray[x][y] = 0;
			}
			else if ((inputArray[x][y] == 0) && (neighbour == 3)) {
				outputArray[x][y] = 1;
			}
			else if ((inputArray[x][y] == 1) && (neighbour == 2) || (neighbour == 3)) {
				outputArray[x][y] = inputArray[x][y];
			}
			
			else outputArray[x][y] = inputArray[x][y];

		}

		

	}

	c++;
	if (c == 1) {
		tick_count t1 = tick_count::now();
		printf("Work took %g seconds\n", (t1 - t0).seconds());
		c = 0;
		t0 = t1;
	}

}

void parallelUpdate(Grid& inputArray, Grid& outputArray) {



	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);

	//number of elements in input array
	const int n = inputArray.getNumElements();
	int G = 1000;
//tell tbb the range of array to be processed, in this case n is the number of elements in the 2d array as defined above
	parallel_for(blocked_range2d<int, int>(1, n-1, 1, n-1), 
		//lambda function that takes range as param
		[&](const blocked_range2d<int, int>& r)
			
	{
			simple_partitioner();
			//declare start and end of y loop
		int yStart = r.rows().begin();
		int yEnd = r.rows().end();

		for (int y = yStart; y != yEnd; y++) {

			int xStart = r.cols().begin();
			int xEnd = r.cols().end();

			//declare start and end of x loop
			for (int x = xStart; x != xEnd; x++) {

				int neighbour = 0;
				// this finds and adds up all the 1's in the 3x3 grid surrounding the cell being processed (position x, y).
				for (int i = -1; i <= 1; i++) {
					for (int j = -1; j <= 1; j++) {
						//adding up states
						neighbour += inputArray[x + i][y + j];
					}
				}

				//subtracts the cell's state thats being processed, if 0, no worries, if one, subtract 1.
				neighbour -= inputArray[x][y];

				//rules
				if ((inputArray[x][y] == 1) && (neighbour < 2)) { //underpopulation
					outputArray[x][y] = 0;
				}
				else if ((inputArray[x][y] == 1) && (neighbour > 3)) { //overpopulation
					outputArray[x][y] = 0;
				}
				else if ((inputArray[x][y] == 0) && (neighbour == 3)) {//birth
					outputArray[x][y] = 1;
				}
				else if ((inputArray[x][y] == 1) && (neighbour == 2) || (neighbour == 3)) { //stasis
					outputArray[x][y] = inputArray[x][y];
				}

				else outputArray[x][y] = inputArray[x][y];

			}//for

		}//rules 2nd for

	});//parallel
	c++;
	//printf("Generation = %i\n", c);
	if (c == 1) {
		tick_count t1 = tick_count::now();
		printf("Work took %g seconds\n", (t1 - t0).seconds());
		c = 0;
		t0 = t1;
	}
}
