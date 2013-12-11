// cf les indications sur la carte de développement
#define S1 !(BitRdPortI(PFDR, 4)) //switch, port F bit 4
#define S2 !(BitRdPortI(PBDR, 7)) //switch, port B bit 7
#define DS1 6 //port F bit 6
#define DS2 7 //port F bit 7

#memmap xmem
#class auto

#define TCPCONFIG  0
#define BOOTP_VERBOSE		// Print lots of detail
#define USE_ETHERNET		1
#define USE_DHCP

// DHCP Timeout in seconds
#define DHCP_TIMEOUT 8

#define IFCONFIG_ETH0 \
IFS_IPADDR, aton("192.168.1.127"), \
IFS_NETMASK, aton("255.255.255.0"), \
IFS_ROUTER_SET, aton("192.168.1.1")

#define MAX_ANSWER_LENGTH 256
#define MAX_BODY_LENGTH 128

#use "dcrtcp.lib"
#use "http.lib"


//specifique au protocole HTTP qui specifie quel type de donnee on envoi
SSPEC_MIMETABLE_START
    SSPEC_MIME(".html", "text/html"),
    SSPEC_MIME(".png", "image/png"),
    SSPEC_MIME(".css", "text/css"),
    SSPEC_MIME(".js","text/javascript")
SSPEC_MIMETABLE_END

int notify_cgi(HttpState* s);
void lighton(int led);
void lightoff(int led);
static CoData cos2;
static CoData *pcos2;
int notify;







//Si je tape /index.html alors on appel page1.html
SSPEC_RESOURCETABLE_START
SSPEC_RESOURCE_FUNCTION("/notify.cgi", notify_cgi),
SSPEC_RESOURCETABLE_END

void lighton(int led) {
BitWrPortI(PFDR, &PFDRShadow, 0, led);
}
void lightoff(int led) {
BitWrPortI(PFDR, &PFDRShadow, 1, led);
}

char* format_address(long addr, char *saddr) {
   sprintf(saddr, "%d.%d.%d.%d",
	   (int)((addr >> 24) & 0xFF),
	   (int)((addr >> 16) & 0xFF),
	   (int)((addr >> 8) & 0xFF),
	   (int)(addr & 0xFF));
   return saddr;
}

void acquire_netcfg_by_dhcp(void) {
   longword myipaddress;
   longword mynetmask;
   longword mybroadcastaddr;
   longword mygateway;
   char saddr[16];

   ifconfig(IF_ETH0,
		IFS_DOWN,
		IFS_ICMP_CONFIG, 1,
		IFS_DHCP_TIMEOUT, DHCP_TIMEOUT,
		IFS_DHCP, 1,
		IFS_UP,
		IFS_END);

	sock_init();

   while(ifpending(IF_ETH0) != IF_UP) {
	   while (ifpending(IF_ETH0) == IF_COMING_UP)
	      tcp_tick(NULL);
   }
   printf("Network configuration successful.\n");

	ifconfig(IF_ETH0,
   	IFG_IPADDR, &myipaddress,
      IFG_NETMASK, &mynetmask,
      IFG_ROUTER_DEFAULT, &mygateway,
      IFS_END);
   mybroadcastaddr = (myipaddress & mynetmask)
      	+ (~mynetmask);
   printf("IP Address = %s\n", format_address(myipaddress, saddr));
	printf("Netmask = %s\n", format_address(mynetmask, saddr));
	printf("Gateway = %s\n", format_address(mygateway, saddr));
	printf("Broadcast Address = %s\n", format_address(mybroadcastaddr, saddr));
}


//Prend l'url et la met dans une variable, consulte_cgi retourne cette url

int notify_cgi(HttpState* state) {
			notify=0;
			printf("Reception d'un mail");
         cgi_sendstring(state, "OK");

         return 0;
}

void main(void) {
	notify=1;

   brdInit();
   pcos2=&cos2;

	acquire_netcfg_by_dhcp();
   http_init();
   tcp_reserveport(80);

   while(1) {
   	costate{
         waitfor(notify==0);
      	lighton(DS2);
      	waitfor(DelayMs(60));
      	lightoff(DS2);
         waitfor(DelayMs(60));
     					 }
   	costate {
         waitfor(S2);
         waitfor(!S2);
         lightoff(DS2);
         notify=1;
         }
         costate {http_handler(); }
		   }
	}
















