# myos

when we start a computer x86 cpu, the instruction pointer holds the memory address of instruction being executed by the processor.
EIP holds the value 0xFFFFFF0. this is called reset vector.
now the chipset's memory maps makes sure that 0xffffff0 is mapped to a certain part of the bios, not to the ram. Meanwhile, the bios copies itself to the ram for faster access . this is called shadowing. this address contain just the jump instruction to the address in memory where bios has copied itself.

then bios first searches for a bootable device int he configured boot device order. it checks for a certain magic number to determine if the device is bootable or not. 

once the bios has found a bootable device, it copies the contents of the device's first sector into ram starting from physical address 0x7coo and then jumps to the address and executes the code just loaded. this code is called bootloader. 

the bootloader then loads the kernel at the physical address 0x100000. the address 0x100000 is used as the start address for all big kernels on x86 machines.

by setting cr0 register to 1, mode is changed to 32 bit protected mode from 16 bit real mode. it is done by grub bootloader. thus the kernel loads in 32bit protected mode.


sudo apt-get install VirtualBox grub-legacy xorriso
to create grub.cfg mannually we use 

grub-mkrescue --output=$@ iso--- iso is target directory// here we are trying to make the cd image

communicating with hardware lecture4

we need to really precise on byte order transferring

multiboot unsigned number--- 
sending data to keyboard is not really hard but receiving data is pretty hard.

memory segment :
binary data loaded to ram. here i am using user space for writing os.
suppose processor is executing data in user space and if a button is pressed then what happed to internally:
when we press button of keyboard then keyboard controller at port communicate with the processor. controller is responsible to generate the interrupt.
we have to set up an intruppt descriptor table which contain info about keyboard intruppt and also contain the memory segment where the execution should jump to. interrupt is always executing in kernel mode. and processor jump to kernel mode from used mode. 
keyboard-cpu-idt-kernel_space
before designing interrrupt descriptor table we need to understand user space and kernel space memory segment


create gloabal descriptor table: is a table of segment entry(starting point of the segment and links and flags(access rights,executable right....)

8 byte entry in table; 16bit for limits, 16bit for pointer, another byte for pointer, access rights,  byte no 6 subdivided into 2 parts low bit form limit, upper byte for flag and last byte for pointer <-----

from right to left: byte 1, 2 and lower part of byte 6 is for link, byte 4,5,6 and 8 are for pointers, byte 5 is for access rights


about global descriptor table: we have to distinguish the following case.
first case: if the limit is small than 65536 then we just set the 6th byte. otherwise 2 

2 and half byte for limits 
The 8086 is a 16-bit processor. This means that its registers are 16 bits wide and can therefore take up values ​​up to 216 - 1 = 65535. Now, however, pointers must also fit in these registers, and in this way only 64 KiB can be addressed, which was not exactly a generous address space during the introduction of the processor.

For this reason, a simple type of segmentation was introduced in realm mode. For each memory access, a base address is added which is determined by the content of the corresponding segment register (cs for code, ds for data, ss for the stack). Thus, addresses are effectively 20 bits large; so 1 MiB address space is accessible, whereby the segment register has to be changed when accessing more than 64KiB.

12 additional virtual bit only allowed when all bit is 1.

 when we press a key, a signal will go to programmable interrupt controller, pic just ignore that. to avaoid this, 
 to send data: 
 cpu has a multiplexer and demultiplexer which is connected to different hardware. they are used to send and receive data from pci. 
 
 outb: port number and data what we want to send. (assembler)
different port has different bandwidth

object oriented design:
port object and two methods:read and write(uint8_t)
if you send an 8 bit integer to 16 bit port then no problem;
send 16 bit to 8bit port then compiler will tell us it is illigle and you are doing some wrong.

how to calculate the location of cursor on screen:

 now we do not want to write the text on the screen but we want to write the data at cursor location
 
 screen 18 char wide and 25 char high 
 
 
 interrupt descriptor table:
 3 section: interrrupt number(timer will give), address where to jump in ram(void pointer to the handler), flag, segment(going to tell processor to certain segment) user space to kernel space, access right(0 to 3) 
 
 problem with handler: interrupt number will be passed as parameter to handler function
 
 different handler and different code to handle interrupt
 
 how these ram(kernel or user space) address get to push to the entry table. 
 
 static code need static pointer to access.
 static Interrupt Manager mean we will have only one active interrupt Manager.
 
 but operating system handle interrupt in differnt way. Here the static function jump to the interruptManager
 
Differnet object for different types of interrupt

mouse communication:
if we move the mouse or press the key of mouse then we get three different bytes from mouse. problem is that this offset does not start at 0. it must start at 1 or 2. if your mouse doesnot behaves correctly then we need to change or reset the offset.


 
 
 


