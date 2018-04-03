#include <hardwarecommunication/pci.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;



PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor()
{

}
PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor()
{

}



//constructor initialization
PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
: dataPort(0XCFC),
  commandPort(0XCF8)
{

}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController()
{

}

uint32_t PeripheralComponentInterconnectController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset)
{
	//from these number, we will construct identifier that we will send to the pci controller
	uint32_t id =
		0x1 << 31 //setting first bit to 1 explicitly
		| ((bus & 0xFF) << 16) //bus number is shifted by 16 bits
		| ((device & 0x1F) << 11) //device number is shifted by 11 bit
		| ((function & 0x07) << 8) //function number is shifted by 8 bits
		| (registeroffset & 0xFC); //we are cutting last two bits of number; 0xFC, so that the number will be 4 byte aligned.
	commandPort.Write(id); // writing id to command port
	uint32_t result = dataPort.Read();
	return result >> (8 * (registeroffset % 4));
}

void PeripheralComponentInterconnectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value)
{
	uint32_t id =
		0x1 << 31 //setting first bit to 1 explicitly
		| ((bus & 0xFF) << 16) //bus number is shifted by 16 bits
		| ((device & 0x1F) << 11) //device number is shifted by 11 bit
		| ((function & 0x07) << 8) //function number is shifted by 8 bits
		| (registeroffset & 0xFC); //we are cutting last two bits of number; 0xFC, so that the number will be 4 byte aligned.
	commandPort.Write(id); // writing id to command port
	dataPort.Write(value);
}
bool PeripheralComponentInterconnectController::DeviceHasFunctions(common::uint16_t bus, common::uint16_t device)
{
	return Read(bus, device, 0, 0x0E) & (1<<7); //we need 7 bit offset. we 0x0E address and will read only 7th bit to know that whether there is a function or not?
}

void printf(char* str);
void printfHex(uint8_t);

void  PeripheralComponentInterconnectController::SelectDrivers(DriverManager* driverManager)
{
	for(int bus = 0; bus < 8; bus++)
	{
		for(int device = 0; device < 32; device++)
		{
			int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;
			for(int function = 0; function < numFunctions; function++)
			{
				PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);
		
				if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
					break;
				printf("PCI BUS ");
				printfHex(bus & 0xFF);

				printf(", DEVICE ");
				printfHex(device & 0xFF);

				printf(", FUNCTION ");
				printfHex(function & 0xFF);
				
				printf(" = VENDOR ");
				printfHex((dev.vendor_id & 0xFF00) >> 8);
				printfHex(dev.vendor_id & 0xFF);
				printf(", DEVICE ");
				printfHex((dev.device_id & 0xFF00) >> 8);
				printfHex(dev.device_id & 0xFF);
				printf("\n");
			}
		}
	}
}
PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
	PeripheralComponentInterconnectDeviceDescriptor result;
	
	result.bus = bus;
	result.device = device;
	result.function = function;

	result.vendor_id = Read(bus, device, function, 0x00);
	result.device_id = Read(bus, device, function, 0x02);

	result.class_id = Read(bus, device, function, 0x0b);
	result.subclass_id = Read(bus, device, function, 0x0a);
	result.interface_id = Read(bus, device, function, 0x09);

	result.revision = Read(bus, device, function, 0x08);
	result.interrupt = Read(bus, device, function, 0x3c);

	return result;	
}

//am79c923 amd chip