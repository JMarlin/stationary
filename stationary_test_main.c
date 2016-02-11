#include "externals.h"

int main(int argc, char* argv[]) {
	
	unsigned long disp;
	
	iShowFPS = 1;
	GPUinit();
	GPUopen(&disp, "Stationary", "");
	GPUdisplayText("You are now running Stationary");
	updateDisplay();
	while(1);
	return 1;
}