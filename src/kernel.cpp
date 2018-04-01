//This is the main file and that start the loading of operating system

#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h> 
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;


void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    
    static uint8_t x = 0, y = 0;
    
    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i]){
            case '\n':
                y++;
                x = 0;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }
        
        
        if(x >= 80)
        {
            y++;
            x = 0;
        }
        
        if(y>=25)
        {
            for(y=0; y<25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
        
    }
}

void printfHex(uint8_t key)
{
     char* foo = "00";
     char* hex = "0123456789ABCDEF";
     foo[0] = hex[(key >> 4) & 0xF];
     foo[1] = hex[key & 0xF];
     //printf("\n");
     printf(foo);
     //printf("\n");
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnkeyDown(char c)
    {
        char* foo = " ";
        foo[0] = c;
        //printf("\n");
        printf(foo);
        //printf("\n");
    }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
public:
    MouseToConsole()
    {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);        
    }
    
    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

        x += xoffset;
        if(x >= 80) x = 79;
        if(x < 0) x = 0;
        y += yoffset;
        if(y >= 25) y = 24;
        if(y < 0) y = 0;

        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }
    
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

 extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("Hello World!---Aditya\n");
    
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt); //initiation of interrupt
    
    printf("Initialize Hardware, Stage 1\n");
    
    DriverManager drvManager;
    #ifdef GRAPHICSMODE
            KeyboardDriver keyboard(&interrupts, &desktop);
        #else
            PrintfKeyboardEventHandler kbhandler;
            KeyboardDriver keyboard(&interrupts, &kbhandler);
    #endif
    //KeyboardDriver keyboard(&interrupts);
    drvManager.AddDriver(&keyboard);
    
     #ifdef GRAPHICSMODE
            MouseDriver mouse(&interrupts, &desktop);
        #else
            MouseToConsole mousehandler;
            MouseDriver mouse(&interrupts, &mousehandler);
    #endif    
    drvManager.AddDriver(&mouse);
    
    printf("Initialize Hardware, Stage 2 \n");
        
    drvManager.ActivateAll();
    printf("Initialize Hardware, Stage 3 ");
    
    interrupts.Activate();
    
    while(1);
}
