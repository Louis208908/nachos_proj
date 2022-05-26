#include "syscall.h"
#include "iostream"

int main(){
	int i, j;
	for(i = 1;i < 6; ++i){
		std::cout << "now in sjf_test_2\n";
		PrintInt(2);
		for(j = 1;j < 50; ++j);
	}
	Exit(2);
}
