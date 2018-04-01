#include <gdt.h>

using namespace myos;
using namespace myos::common;


GlobalDescriptorTable::GlobalDescriptorTable() //it is a constructor. it will construct null segment, unused segment, code segment, data segment
: nullSegmentSelector(0, 0, 0),
unusedSegmentSelector(0, 0, 0),
codeSegmentSelector(0, 64*1024*1024, 0x9A),
dataSegmentSelector(0, 64*1024*1024, 0x92)
{
    uint32_t i[2]; //
    i[0] = sizeof(GlobalDescriptorTable) << 16; //need to shift left because high order byte
    i[1] = (uint32_t)this; //address of table itself

    asm volatile("lgdt (%0)"::"p" (((uint8_t *) i)+2)); // to tell the process to use this table now
    
}

GlobalDescriptorTable::~GlobalDescriptorTable()
{//does nothing for now. actually it should unload                          the gdt.
}

uint16_t GlobalDescriptorTable::DataSegmentSelector()//offset 
{
    return (uint8_t*)&dataSegmentSelector - (uint8_t*)this; // address of data section and uint8_t * this is the address of the table 
    
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
    return (uint8_t*)&codeSegmentSelector - (uint8_t*)this; // uint8_t * this is the size of the table 
    
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type)
{
    uint8_t* target = (uint8_t*)this;
    if(limit <= 65536)
    {
        // 16 bit address space
        target[6] = 0x40;
    }
    else
    {
        //32 bit address space
        //Now we have to sqeeze the 32 bit limit into 2.5 registers 20bit
        //This is done by discarding the 12 least significant bits, but this 
        //is only legal, if they are all ==1, so they are implicitly still there
        //So if the last bits aren't all 1, we have to set them to 1, but this
        //would increase the limit (cannot do that, because we might go beyond
        //the physical limit or get overlap with other segments) so we have 
        //to compensate this by decreasing a higher bit (and might have up to 
        //4095 wasted bytes behind the used memory)
        if((limit & 0xFFF) != 0xFFF)
            limit = (limit >> 12)-1;
        else
            limit = limit >> 12;
    
        // if we are not in 16 bit mode then we set the target will be set to 0xC0
        
        target[6] = 0xC0;
    }
    //now distribute 
    target[0] = limit & 0xFF;//is now least  significant bit
    target[1] = (limit >> 8) & 0xFF;
    target[6] |= (limit >> 16) & 0xF; //now only allowed to set last 4 bit
    
    //now we have to encode the pointer
    target[2] = base  & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;
    
    // now only we have to set access rights
    target[5] = type;
}
// we need to just take the corrosponding byte . 

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base()
{
   uint8_t* target = (uint8_t*) this;
   uint32_t result = target[7];
   result = (result << 8) + target[4];
   result = (result << 8) + target[3];
   result = (result << 8) + target[2];
   return result;
    
}
uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit()
{
    uint8_t* target = (uint8_t*) this;
    uint32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];
    
    if((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xFFF; // this is the else case.
    
    return result;
}
