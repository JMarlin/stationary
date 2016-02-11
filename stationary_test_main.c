#include "peops/externals.h"

int main(int argc, char* argv[]) {
	
	unsigned long disp;
	
	GPUinit();
	GPUopen(&disp, "Stationary", "");
	GPUdisplayText("You are now running Stationary");
	while(1) updateDisplay();
	return 1;
}