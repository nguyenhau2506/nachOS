#include "syscall.h"
#include "copyright.h"
#define MAX_NUM 2147483647
const int len = 100;


int main(){
	int arr[len], n, i, j, temp, haveSwap, type;
	do {
		PrintString("Input the size of array: ");
		n = ReadNum();
	} while(n == MAX_NUM || n > 100 || n < 0);

	PrintString("Input all elements of array\n");
	for(i = 0; i < n; i++){
		do{
			arr[i] = ReadNum();
		} while(arr[i] == MAX_NUM);
	}

	while(1 == 1){
		do{
			PrintString("1.Increasing order\n2.Decreasing order\n0.Exit\nYour choice: ");
			type = ReadNum();
		} while(type == MAX_NUM);

		if(type == 0){
			return 0;
		} else if(type == 1){//sap xep tang dan
			for(i = 0; i < n-1; i++) {
				haveSwap = 0;
				for(j = 0; j < n-i-1; j++){
					if(arr[j] > arr[j+1]){
						temp = arr[j];
						arr[j] = arr[j+1];
						arr[j+1] = temp;
						haveSwap = 1;
					}
				}
				if(haveSwap == 0)
					break;
			}
		} else if(type == 2){//sap xep giam dan
			for(i = 0; i < n-1; i++) {
				haveSwap = 0;
				for(j = 0; j < n-i-1; j++){
					if(arr[j] < arr[j+1]){
						temp = arr[j];
						arr[j] = arr[j+1];
						arr[j+1] = temp;
						haveSwap = 1;
					}
				}
				if(haveSwap == 0)
					break;
			}
		} else{
			continue;
		}
		PrintString("The array after sorted: \n");
		for(i = 0; i < n; i++){
			PrintNum(arr[i]);
			PrintString(" ");
		}
		PrintString("\n");
	}
	return 0;
}