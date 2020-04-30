#include <cstdlib>
#include <iostream>
#include <string>
#include <istream>
#include <string.h>
#include <fstream>

using namespace std;

int main(){
	ifstream path;
	path.open("path");
	char *mydir = new char[200];
	path >> mydir;
	char *command = new char[500];
	strcat(command, "dot -Tpng -oPIC ");
	strcat(mydir, "/Source/graphFile.dot");
	strcat(command, mydir);
	system(command);
	//system("dot -Tpng -oPIC /home/marina/Документы/piaa_2/Source/graphFile.dot");
	system("xdg-open PIC");
	return 0;
}
