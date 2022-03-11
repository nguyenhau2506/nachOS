// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include <stdio.h>
#include "syscall.h"
#include <timer.h>
#define MaxFileLength 255
#define MaxBuffer 255
#define MAX_NUM 2147483647

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void IncreasePC()
{
    // have three pcregister PrevPCReg,PCReg,NextPCReg
    // PrevPCReg=PCReg
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    // PCReg=NextPCReg
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    // NextPCReg=NextPCReg+4
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}

char *User2System(int virtAddr, int limit)
{
    int i; // chi so index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // can cho chuoi terminal
    if (kernelBuf == NULL)
        return kernelBuf;

    memset(kernelBuf, 0, limit + 1);

    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

int System2User(int virtAddr, int len, char *buffer) // lay gia tri bo vao bien
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}
void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    
    switch (which)
    {
    case NoException:
        return;
    case SyscallException:
        switch (type)
        {
            
            case SC_Halt:
            {
                DEBUG('a', "\n Shutdown, initiated by user program.");
                printf("\n\n Shutdown, initiated by user program.");
                interrupt->Halt();
                break;
            }
            case SC_Create:
            {
                int virtAddr;
                char *filename;
                DEBUG('a', "\n SC_Create call ...");
                DEBUG('a', "\n Reading virtual address of filename");
                // Lấy tham số tên tập tin từ thanh ghi r4
                virtAddr = machine->ReadRegister(4);
                DEBUG('a', "\n Reading filename.");
                // MaxFileLength là = 32
                filename = User2System(virtAddr, MaxFileLength + 1);
                if (filename == NULL)
                {
                    printf("\n Not enough memory in system");
                    DEBUG('a', "\n Not enough memory in system");
                    machine->WriteRegister(2, -1); // trả về lỗi cho chương
                    // trình người dùng
                    delete filename;
                    return;
                }
                DEBUG('a', "\n Finish reading filename.");
                // DEBUG('a',"\n File name : '"<<filename<<"'");
                //  Create file with size = 0
                //  Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
                //  việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
                //  hành Linux, chúng ta không quản ly trực tiếp các block trên
                //  đĩa cứng cấp phát cho file, việc quản ly các block của file
                //  trên ổ đĩa là một đồ án khác
                if (!fileSystem->Create(filename, 0))
                {
                    printf("\n Error create file '%s'", filename);
                    machine->WriteRegister(2, -1);
                    delete filename;
                    return;
                }
                machine->WriteRegister(2, 0); // trả về cho chương trình
                // người dùng thành công
                delete filename;
                break;
            }
            case SC_ReadNum:
            {   
                
                char *buffer;
                buffer = new char[MaxBuffer + 1];
                // doc so tu  man hinh console
                int numbytes = gSynchConsole->Read(buffer, MaxBuffer); // doc so nguyen vao buffer có toi da 255 ki tu

                if(numbytes >= 10)
                {
                    printf("\n\n Over size");
                    DEBUG('a', "\n Over size");
                    machine->WriteRegister(2, MAX_NUM);
                    IncreasePC();
                    delete buffer;
                    return;
                }
                int firstIndex, lastIndex;
                bool isNegative;
                if (buffer[0] == '-')
                {
                    isNegative = true;
                    firstIndex = 1;
                }
                else
                {
                    isNegative = false;
                    firstIndex = 0;
                }

                for (int i = firstIndex; i < numbytes ; i++)
                {
                    if ((buffer[i] == '.'))
                    {
                        printf("\n\n The integer number is not valid");
                        DEBUG('a', "\n The integer number is not valid");
                        machine->WriteRegister(2, MAX_NUM);
                        IncreasePC();
                        delete buffer;
                        return;
                    }
                    else if(buffer[i] < '0' || buffer[i] > '9')
                    {
                        printf("\n\n The integer number is not valid");
                        DEBUG('a', "\n The integer number is not valid");
                        machine->WriteRegister(2, MAX_NUM);
                        IncreasePC();
                        delete buffer;
                        return;
                    }
                    lastIndex = i;
                }

                int number = 0;
                if (isNegative)
                {
                    for (int i = firstIndex; i <= lastIndex; i++)
                    {
                        number = number * 10 + (int)(buffer[i] - 48);
                    }
                    number = number * -1;
                }
                else
                {
                    for (int i = firstIndex; i <= lastIndex; i++)
                    {
                        number = number * 10 + (int)(buffer[i] - 48);
                    }
                }

                machine->WriteRegister(2, number);
                IncreasePC();
                delete buffer;
                return;
            }
            case SC_PrintNum:
            {
                int number = machine->ReadRegister(4);
                if(number == 0)//th number la so 0
                {
                    gSynchConsole->Write("0", 1); // in ra man hinh so 0
                    IncreasePC();
                    return;  
                }
                        
                bool isNegative = false; 
                int sizeOfNum = 0; 
                int firstNumIndex = 0; 

                if(number < 0)
                {
                    isNegative = true;
                    number = number * -1; //chuyen am thanh duong
                    firstNumIndex = 1; 
                } 	
                    
                int t_number = number; // bien tam cho number
                while(t_number)
                {
                    sizeOfNum++;
                    t_number /= 10;
                }
        
                // Tao buffer chuoi de in ra man hinh
                char* buffer;
                int MAX_BUFFER = 255;
                buffer = new char[MAX_BUFFER + 1];
                for(int i = firstNumIndex + sizeOfNum - 1; i >= firstNumIndex; i--)
                {
                    buffer[i] = (char)((number % 10) + 48);
                    number /= 10;
                }
                if(isNegative)//neu so am thi de buffer[0] se la dau tru
                {
                    buffer[0] = '-';
                    buffer[sizeOfNum + 1] = 0;
                    gSynchConsole->Write(buffer, sizeOfNum + 1);
                    delete buffer;
                    IncreasePC();
                    return;
                }
                buffer[sizeOfNum] = 0;	
                gSynchConsole->Write(buffer, sizeOfNum);
                delete buffer;
                IncreasePC();
                return;        	
            }
            case SC_ReadChar://doc 1 ki tu do nguoi dung nhap vao
            {
                char* buffer = new char[MaxBuffer];
                int numBytes = gSynchConsole->Read(buffer, MaxBuffer);

                if(numBytes > 1) //Neu nhap nhieu hon 1 ky tu thi khong hop le
                {
                    printf("Only 1 character!");
                    DEBUG('a', "\nERROR: Only 1 character!");
                    machine->WriteRegister(2, 0);
                }
                else if(numBytes == 0) 
                {
                    printf("Empty!");
                    DEBUG('a', "\nERROR: empty!");
                    machine->WriteRegister(2, 0);
                }
                else
                {
                    char c = buffer[0];
                    machine->WriteRegister(2, c);
                }

                delete buffer;
                IncreasePC(); // tang Programming Counter
                return;
            }
            case SC_PrintChar:
            {
                char c = (char)machine->ReadRegister(4); //doc tu thanh ghi r4
                gSynchConsole->Write(&c, 1); //1 byte
                IncreasePC();
                return;
            }
            case SC_RandomNum:
            {
                int n=Random()%10000;
                machine->WriteRegister(2, n);
                IncreasePC();
                return;
            }
            case SC_ReadString:
            {
                int virtAddr, length;
                char* buffer;
                virtAddr = machine->ReadRegister(4); // doc tu thanh ghi r4
                length = machine->ReadRegister(5); // doc tu thanh ghi r5
                buffer = User2System(virtAddr, length); // chuyen vung nho tu User Space sang System Space
                gSynchConsole->Read(buffer, length); // doc chuoi thong qua ham Read cua SynchConsole
                System2User(virtAddr, length, buffer); // chuyen vung nho tu System Space sang User Space
                delete buffer; 
                IncreasePC(); // tang Programming Counter 
                return;
            }
            case SC_PrintString:
            {
                int virtAddr;
                char* buffer;
                virtAddr = machine->ReadRegister(4); // doc tu thanh ghi r4
                buffer = User2System(virtAddr, 255); // chuyen vung nho tu User Space sang System Space
                int length = 0;
                while (buffer[length] != 0) // dem do dai cua chuoi 
                    length++; 
                gSynchConsole->Write(buffer, length + 1); // in chuoi thong qua ham Write cua SynchConsole
                delete buffer; 
                IncreasePC(); // tang Programming Counter 
                return;
            }
            break;
        }
    case PageFaultException:
    {
        DEBUG('a', "\n No valid translation found.");
        interrupt->Halt();
        break;
    }
    case ReadOnlyException:
    {
        DEBUG('a', "\nWrite attempted to page marked.");
        interrupt->Halt();
        break;
    }
    case BusErrorException:
    {
        DEBUG('a', "\nTranslation resulted in an invalid physical address");
        interrupt->Halt();
        break;
    }
    case AddressErrorException:
    {
        DEBUG('a', "\nUnaligned reference or one that was beyond the end of the address space");
        interrupt->Halt();
        break;
    }
    case OverflowException:
    {
        DEBUG('a', "Integer overflow in add or sub");
        interrupt->Halt();
        break;
    }
    case IllegalInstrException:
    {
        DEBUG('a', "\nUnimplemented or reserved instr.");
        interrupt->Halt();
        break;
    }
    case NumExceptionTypes:
    {
        DEBUG('a', "\ninvalid exceprion.");
        interrupt->Halt();
        break;
    }
    default:
        DEBUG('a', "no exception.");
        interrupt->Halt();
}
}