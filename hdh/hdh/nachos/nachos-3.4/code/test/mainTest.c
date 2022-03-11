#include "syscall.h"
#include "copyright.h"
#define maxlen 32

int main()
{
    // int n;
    // n=RandomNum();
    
    // PrintNum(n);
    //Halt();
    char str[50];
    int length = 50;
    PrintString("Nhap chuoi: ");
    ReadString(str, length);
    PrintString(str);
    PrintString("\n");
    // return 0;
}