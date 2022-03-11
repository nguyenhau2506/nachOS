#include "syscall.h"
#include "copyright.h"

int main(){
	int i;
	for(i = 33; i < 127; i++){
		PrintChar(i);
		PrintString("\n");
	}
}