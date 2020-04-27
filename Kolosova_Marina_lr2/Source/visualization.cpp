#include <cstdlib>
#include <iostream>
#include <string>
#include <istream>
#include <string.h>
#include <fstream>

using namespace std;

int main(){
	system("dot -Tpng -oPIC /home/marina/Документы/piaa_2/Source/graphFile.dot");
	system("xdg-open PIC");
	return 0;
}
