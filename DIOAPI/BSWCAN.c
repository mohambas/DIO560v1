#include "BSWCAN.h"
#include "Definitions.h"


// ------------------------ BSW functions --------------------------//
int canInit(const char* can_iface_name)
{
	int s; /* can raw socket */ 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct timeval tv;
	
	//close(IOCAN_Socket);
	//close(CPUCAN_Socket);
	
	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		printf("Open Socket Error!\n");
		return -1;
	}
	
	// Resolve the iface index
	(void)memset(&ifr, 0, sizeof(ifr));
	(void)strncpy(ifr.ifr_name, can_iface_name, IFNAMSIZ);

	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		printf("Resolving iface index error!\n");
		return -1;
	}
	
	// Assign the iface
	(void)memset(&addr, 0, sizeof(addr));
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

		/* disable default receive filter on this RAW socket */
		/* This is obsolete as we do not read from the socket at all, but for */
		/* this reason we can remove the receive list in the Kernel to save a */
		/* little (really a very little!) CPU usage.                          */
		//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	tv.tv_sec = 0;
	tv.tv_usec = 50000; // 50ms
	//setsockopt(s, SOL_CAN_RAW, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		printf("binding Socket Error!\n");
		return -1;
	}
	
	printf("(can_Init) Interface ""%s"" initiated at index:  %d\n", ifr.ifr_name, addr.can_ifindex);

	return addr.can_ifindex;   // Refers to the initiated socket
}

///////////////////////////////////////////////////////
int can0_init()
{
	int s; /* can raw socket */ 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct timeval tv;
	
	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		printf("Open IO Socket Error!\n");
		return -1;
	}
	
	// Resolve the iface index
	(void)memset(&ifr, 0, sizeof(ifr));
	(void)strncpy(ifr.ifr_name, "can0", IFNAMSIZ);

	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		printf("Resolving iface index error!\n");
		return -1;
	}
	
	// Assign the iface
	(void)memset(&addr, 0, sizeof(addr));
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

		/* disable default receive filter on this RAW socket */
		/* This is obsolete as we do not read from the socket at all, but for */
		/* this reason we can remove the receive list in the Kernel to save a */
		/* little (really a very little!) CPU usage.                          */
		//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	tv.tv_sec = 0;
	//===== time out =========//
	tv.tv_usec = 50000; // 50ms
	//setsockopt(s, SOL_CAN_RAW, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		printf("binding Socket Error!\n");
		return -1;
	}
	
	printf(" - Interface ""%s"" initiated with index: %d \r\n", ifr.ifr_name, addr.can_ifindex);

	return s;   // Refers to the initiated socke
}

int can1_init()
{
	int s; /* can raw socket */ 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct timeval tv;
	
	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		printf("Open CPU Socket Error!\n");
		return -1;
	}
	
	// Resolve the iface index
	(void)memset(&ifr, 0, sizeof(ifr));
	(void)strncpy(ifr.ifr_name, "can1", IFNAMSIZ);

	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		printf("Resolving iface index error!\n");
		return -1;
	}
	
	// Assign the iface
	(void)memset(&addr, 0, sizeof(addr));
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

		/* disable default receive filter on this RAW socket */
		/* This is obsolete as we do not read from the socket at all, but for */
		/* this reason we can remove the receive list in the Kernel to save a */
		/* little (really a very little!) CPU usage.                          */
		//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	tv.tv_sec = 0;
	tv.tv_usec = 500000; // 500ms
	//setsockopt(s, SOL_CAN_RAW, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		printf("binding Socket Error!\n");
		return -1;
	}
	
	printf(" - Interface ""%s"" initiated with index: %d\n", ifr.ifr_name, addr.can_ifindex);

	return s;   // Refers to the initiated socke
}

/*struct timeval timeStamp(int CANSocket)
{
	struct timeval tv;
	ioctl(CANSocket, SIOCGSTAMP, &tv);
	return tv
}*/

/*int sendto_can0(int CANSocket, struct can_frame cf)
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	int nbytes;

	strcpy(ifr.ifr_name, "can0");
	ioctl(CANSocket, SIOCGIFINDEX, &ifr);
    addr.can_ifindex = ifr.ifr_ifindex;
    addr.can_family  = AF_CAN;

	nbytes = sendto(CANSocket, &cf, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
}

int sendto_can1(int CANSocket, struct can_frame cf)
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	int nbytes;
	strcpy(ifr.ifr_name, "can1");
	ioctl(CANSocket, SIOCGIFINDEX, &ifr);
    addr.can_ifindex = ifr.ifr_ifindex;
    addr.can_family  = AF_CAN;

	nbytes = sendto(CANSocket, &cf, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
}*/

int sendFrame(int CANSocket, int canID, unsigned char dLen, unsigned char canData[])  // send can packet to the can0 device
{
	struct can_frame cf;
	//struct sockaddr_can addr;

	cf.can_id = canID;
	cf.can_dlc = dLen;

	for (int i = 0; i < dLen; i++)
	{
		cf.data[i] = canData[i];
	}

	// new form
	/*addr.can_ifindex = CANSocket;
	addr.can_family  = AF_CAN;

	nbytes = sendto(CANSocket, &cf, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
	if (nbytes != sizeof(cf)) {
		printf(" CAN Write Error.\n");
		return ERROR;
	}*/
	///

	if ((write(CANSocket, &cf, sizeof(cf))) != sizeof(cf)) {
		printf(" CAN Write Error.\n");
		return ERROR;
	}
	
	return SUCCESS;
}

int receiveFrame(int CANSocket, struct can_frame *cf)  // receive can packet from can_Socket device
{
	int nbytes;

	nbytes = read(CANSocket, cf, sizeof(struct can_frame));
	//nbytes = recvmsg(CANSocket, cf, 0);

	// new form
	/*struct sockaddr_can addr;
	socklen_t len = sizeof(addr);
	nbytes = recvfrom(CANSocket, cf, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, &len);*/
	//

	if (nbytes < 0) {
		errCnt++;
		//perror("read error");
		printf("%d - CAN read error.\n", errCnt);
		return ERROR;
	}

	//paranoid check
	if (nbytes < sizeof(struct can_frame)) {
		//fprintf(stderr, "read: incomplete CAN frame \n");
		printf("read: incomplete CAN frame. \n");
		return ERROR;
	}

	//int i = 0;
	//printf("can0:  %03X   [%d]", cf->can_id, cf->can_dlc);
	//for (i = 0; i < cf->can_dlc; i++)
	//	printf(" %02X", cf->data[i]);
	//printf("\n");

	return SUCCESS;
}

void printRcvFrame(struct can_frame rcvFrame)  // for debug usage
{
	char c;
	
	printf("%03X [%d] ", rcvFrame.can_id, rcvFrame.can_dlc);
	for (c = 0; c < rcvFrame.can_dlc; c++)
		printf(" %02X", rcvFrame.data[c]);
	printf("\n");
}
