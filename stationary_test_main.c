extern long GPUtest(void);

int main(int argc, char* argv[]) {
	
	unsigned long disp;
	
	GPUinit();
	GPUopen(&disp, "Stationary", "");
	while(1);
	return 1;
}