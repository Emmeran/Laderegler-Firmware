#define SERIAL_MAGIC_NUMBER 0xF7

#define LADEREGLER_BSTATUS_LENGTH 24

enum SerialCommand
{
	CMDS_PING, // 0
	CMDS_PONG,
	CMDS_REPLY,
	CMDS_STATUS, // 3
	CMDS_LOG,
	CMDS_IOCTL,
	CMDS_TIME, // 6
	CMDS_SILENCE,
	CMDS_BSTATUS,
};

union SerialHeader
{
	struct
	{
		u8 magicNumber;
		u8 commandByte;
		u8 packetLength;
	} s;
	
	u8 data[3];
};
































