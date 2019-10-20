# CNET-linux-lab2

#include <cnet.h>
#include<string.h>
#define ETH_MAXDATA 1500 // 1500 bytes
#define ETH_MINPACKET 64 // 64 bytes
#define LEN_ETHERHEADER (2*sizeof(CnetNICaddr) + 2)

char buffer[100];
char NICAddresses[3][20]={"00:90:27:62:58:84","00:90:27:41:B0:BE","00:A0:C9:AF:9E:81"};

typedef struct
{
	CnetNICaddr dest; // dia chi MAC cua may dich
	CnetNICaddr src; // dia chi MAC cua may truyen
	char len[2]; // chieu dai du lieu truyen
	char type[2];
	char data[ETH_MAXDATA]; // du lieu truyen
} ETHERPACKET;

void write_to_ethernet(CnetNICaddr dest, size_t link, char *buf, size_t
len)
{
	ETHERPACKET packet;
	memcpy(packet.dest, dest, sizeof(CnetNICaddr));
	memcpy(packet.src, linkinfo[link].nicaddr, sizeof(CnetNICaddr));
	short int twobytes;
	twobytes = len;
	memcpy(packet.type, &twobytes, 2);
	memcpy(packet.data, buf, len);
	len += LEN_ETHERHEADER;
		if(len < ETH_MINPACKET) /* padding*/
			len = ETH_MINPACKET;
/*Ghi du lieu vao duong truyen vat ly*/
CHECK(CNET_write_physical_reliable(link, (char *)&packet, &len));
}

void on_keyboard( CnetEvent event, CnetTimerID timer, CnetData data )
{
size_t bufferLength = 100;
CHECK( CNET_read_keyboard( buffer, &bufferLength ) );
int i = -1;
if(!strcmp(buffer, "computer_A"))
	i = 0;
else if(!strcmp(buffer, "computer_B"))
	i = 1;
else if(!strcmp(buffer, "computer_C"))
	i = 2;
if(i != -1) 
{
char greeting[100];
size_t greetingLength = 100;
sprintf(greeting, "Hello, %s", NICAddresses[i]);
CnetNICaddr dest;
CNET_parse_nicaddr(dest, NICAddresses[i]);
write_to_ethernet(dest, 1, greeting, greetingLength);
printf("Sending\n");
}
//printf( "You just typed '%s'.\n", greeting );
}


void on_phys_ready( CnetEvent event, CnetTimerID timer, CnetData data )
{
ETHERPACKET frame;
int link;
size_t frameLength;
// Doc du lieu tu tang vat ly
frameLength = sizeof( ETHERPACKET );
CHECK( CNET_read_physical( &link, (char *)(&frame), &frameLength ));
printf("%s \n",frame.data);
char temp1[20], temp2[20];
CNET_format_nicaddr(temp1,frame.dest);
CNET_format_nicaddr(temp2,linkinfo[link].nicaddr);

if(!strcmp(temp1,temp2))
{
        printf("Correct destination address!");
} 
else
{
        printf("Wrong destination address!");
}
}


void reboot_node (CnetEvent event, CnetTimerID timer, CnetData data)
{
CHECK( CNET_set_handler( EV_KEYBOARDREADY, on_keyboard, 0 ) );
CHECK( CNET_set_handler( EV_PHYSICALREADY, on_phys_ready, 0 ) );
}
