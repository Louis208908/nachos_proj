#include "syscall.h"

#include "stdio.h"

int main(){
	int i, j;
	for(i = 1;i < 6; ++i){
		printf("inside sjf_test2\n");
		PrintInt(2);
		for(j = 1;j < 50; ++j);
	}
	Exit(2);
}
