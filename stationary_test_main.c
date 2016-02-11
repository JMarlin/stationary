#include "peops/externals.h"

int main(int argc, char* argv[]) {
	
	unsigned long disp;
	
	GPUinit();
	GPUopen(&disp, "Stationary", "");
	GPUdisplayText("You are now running Stationary");
	updateDisplay();
	while(1);
	return 1;
}