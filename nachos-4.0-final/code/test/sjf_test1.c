#include "syscall.h"

int main(){
	int i, j;
	for(i = 1;i < 6; ++i){
		PrintInt(8787);
		for(j = 1;j < 50; ++j);
	}
	Exit(1);
}
