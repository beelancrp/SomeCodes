#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>		//Used for GPIO configuration
#include <pcf8591.h>		//Used for ADC configuration
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <string.h>

const int MIN_PACKAGE_SIZE = 11;
#define MAX_CONTENT_SIZE  67
#define RS_HEADER_SIZE_DEF    2
const int RS_HEADER_SIZE  =  2;
const int CRC_LENGHTH     =  2;
#define OUT_STACK_SIZE    100
#define CRC_POLINOM       4129
const char RS_HEADER[2]    = {0xAA, 0xAA};
const char buffer[100];
const char RPI_DEV_TYPE[2] = {0xFF, 0xFE};
const char RPI_OPR_CODE[2] = {0xE5, 0xF5};

typedef enum  {
	init, 
	monitoringBus, 
	shortCicuit, 
	alertSending
}	prgState;

typedef struct  {
	char content[100];
	int  size;
}	additionalContent;

typedef struct  {
	char content[MAX_CONTENT_SIZE];
	int  size;
}	additionalContent;

typedef struct  {
	char operCode_High;
	char operCode_Low;
}	operationCode;

typedef struct {
	char devType_High;
	char devType_Low;
}	devType;

typedef struct  {
	char dataLength;
	char orgSubnet;
	char orgDeviceId;
	unsigned int devType;
	unsigned int operCode;
	char targSubnet;
	char targId;
	additionalContent additionContent;
	unsigned int checkSum;
}	hdlDataPacage;

typedef struct{
	unsigned int devType;
	char devSubnet;
	char devId;
}   hdlDeviceInfo;

struct hdlDeviceInfo hdlDevsInfo[64];

char * scat (char *d, char *s);
unsigned int twoCharToUint(char highByte, char lowByte);
char getHighByte(unsigned int _number);
char getLowByte(unsigned int _number);
unsigned int calculateCRC(unsigned int dataLenghth, char *data[]);
additionalContent cutAdditionalContent(unsigned int dataLenghth, char *data[]);
hdlDataPacage hdlDataParser(unsigned int dataLenghth, char *data[]);
int open_closeUART(int value);
int sendHdlPacage(hdlDataPacage hdlPacage);
hdlDataPacage recieveHdlPacage(void);

int main(int argc, char *argv[]) {
	
	prgState programState = init;
	
	int buttonPin = 4;
	int relayPin  = 1;
	int initButton;
	float deltaResistance;
	float ohmmeterInitValue;
	float ohmmeterMinValue;
	float ohmmeterMaxValue;
	float ohmmeterCurrentValue;
	short int alertCenceled = 0;
	short int alarm			= 0;
	hdlDataPacage rpiDataPacage;
	hdlDataPacage recievedHdlDataPacage;
	int deviceNumber = 0;
	
	rpiDataPacage.devType      = twoCharToUint(RPI_DEV_TYPE[0], RPI_DEV_TYPE[1]);
	rpiDataPacage.orgSubnet    = '1';
	rpiDataPacage.orgDeviceId  = '3';
	rpiDataPacage.operCode	   = twoCharToUint(RPI_OPR_CODE[0], RPI_OPR_CODE[1]);
	rpiDataPacage.targSubnet   = '255';
	rpiDataPacage.targId	   = '255';
	
	
  while (1){
	switch (programState){
		
		case init:{
			
			wiringPiSetup();
			pcf8591Setup(20, 0x48);
			pinMode(buttonPin, INPUT);
			pinMode(ralayPin, OUTPUT);
			initButton = digitalRead(buttonPin);
			
			if (initButton == 1){
				ohmmeterInitValue = analogRead(20);
				ohmmeterMaxValue  = ohmmeterInitValue + (deltaResistance/2);
				ohmmeterMinValue  = ohmmeterInitValue - (deltaResistance/2);
				sendHdlPacage(rpiDataPacage);
				time_t start = time();
				for(;;){
					recievedHdlDataPacage = recieveHdlPacage();
					if (recievedHdlDataPacage.orgDeviceId != '0'){
						if (isIn(recievedHdlDataPacage.orgDeviceId, hdlDevsInfo, deviceNumber) == 1){
							continue;
						}else{
							hdlDevsInfo[deviceNumber].devId = recievedHdlDataPacage.orgDeviceId;
							hdlDevsInfo[deviceNumber].devSubnet = recievedHdlDataPacage.orgSubnet;
							hdlDevsInfo[deviceNumber].devType   = recievedHdlDataPacage.devType;
							deviceNumber++;
						}
					}
					if(time() > start + 10000){
					break;
					}
				}
			};
			
			
			break;
		}		
		case monitoringBus: {
			
			ohmmeterCurrentValue = analogRead(20);	
			if (ohmmeterCurrentValue < ohmmeterMinValue || ohmmeterCurrentValue > ohmmeterMaxValue){
				programState = shortCicuit;
				break;	
			};
			while (alarm == 0){
					recievedHdlDataPacage = recieveHdlPacage();
					if (recievedHdlDataPacage.orgDeviceId != '0'){
						if (isIn(recievedHdlDataPacage.orgDeviceId, hdlDevsInfo, deviceNumber) == 0){
							alarm = 1;
							programState = shortCicuit;
						}
					}
				}
			
		};
		case shortCicuit: {
			
			if (alertCenceled == 1){
				alertCenceled = 0;
				digitalWrite(relayPin, 0);
				programState = monitoringBus;
				break;
			};
			if (alertCenceled == 0){
				digitalWrite(relayPin, 1);
				programState = alertSending; 
				break;
			};
			
		};
		case alertSending: {
			
			break;
		};
	};
	return 0;
  }
}

char * scat (char *d, char *s){
	char *r=(char*)calloc(strlen(d)+strlen(s),sizeof(char));
	strcpy (r,d);
	strcpy (r+strlen(d),s);
	return r;
}

unsigned int twoCharToUint(char highByte, char lowByte){
	unsigned int localVariable;
	
	localVariable = (unsigned int) scat(highByte, lowByte);
	
	return localVariable;
}

additionalContent cutAdditionalContent(unsigned int dataLenghth, char *data[]){ // нужно уточнить у костика
	additionalContent localAddContent;
	int byteIndex;
	int index = 0;
	
	localAddContent.size = (int) dataLenghth - RS_HEADER_SIZE - MIN_PACKAGE_SIZE;
	
	if (localAddContent.size > 0 && localAddContent.size <= MAX_CONTENT_SIZE){
		for (byteIndex = 0 + RS_HEADER + MIN_PACKAGE_SIZE - CRC_LENGHTH;(dataLenghth - CRC_LENGHTH); byteIndex++){
			index++;
			localAddContent.content[index] = data[byteIndex];
		};
	}
	return localAddContent;
}

hdlDataPacage hdlDataParser(unsigned int dataLenghth, char *data[]){
	hdlDataPacage localHdlDataPacage;
	
	localHdlDataPacage.dataLength 		= data[0+RS_HEADER_SIZE_DEF];
	localHdlDataPacage.orgSubnet  		= data[1+RS_HEADER_SIZE_DEF];
	localHdlDataPacage.orgDeviceId		= data[2+RS_HEADER_SIZE_DEF];
	localHdlDataPacage.devType    		= twoCharToUint(
										  data[3+RS_HEADER_SIZE_DEF],
										  data[4+RS_HEADER_SIZE_DEF]);
	localHdlDataPacage.operCode   		= twoCharToUint(
										  data[5+RS_HEADER_SIZE_DEF],
										  data[6+RS_HEADER_SIZE_DEF]);
	localHdlDataPacage.targSubnet 		= data[7+RS_HEADER_SIZE_DEF];
	localHdlDataPacage.targId     		= data[8+RS_HEADER_SIZE_DEF];
	localHdlDataPacage.additionContent  = cutAdditionalContent(
										  dataLenghth,
										  data);
	localHdlDataPacage.checkSum         = twoCharToUint(
										  data[dataLenghth-1],
										  data[dataLenghth]);
	return localHdlDataPacage;
	
}

char getHighByte(unsigned int _number){
	char highByte;
	
	highByte = (char)(_number/256);
	
	return highByte;
}

char getLowByte(unsigned int _number){
	char lowByte;
	
	lowByte = (char)(_number%256);
	
	return lowByte;
}

unsigned int calculateCRC(unsigned int dataLenghth, char *data[]){
	
}

int open_closeUART(int value){
	int uart0_filestream = -1;
	if (value == 1){
//		uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
		if (uart0_filestream == -1)
		{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
		return 1;
		}else{ return 0;
		};
	}
	if (value == 0){
		close(uart0_filestream);
	}
}

int sendHdlPacage(hdlDataPacage hdlPacage){
	hdlDataPacage localHdlDataPacage = hdlPacage;
	char txBuffer[100];
	int i;
	unsigned int crc;
	
	txBuffer[0] = RS_HEADER[0];
	txBuffer[1] = RS_HEADER[1];
	txBuffer[2] = (char)(MIN_PACKAGE_SIZE + localHdlDataPacage.additionContent.size);
	txBuffer[3] = localHdlDataPacage.orgSubnet;
	txBuffer[4] = localHdlDataPacage.orgDeviceId;
	txBuffer[5] = getHighByte(localHdlDataPacage.devType);
	txBuffer[6] = getLowByte(localHdlDataPacage.devType);
	txBuffer[7] = getHighByte(localHdlDataPacage.operCode);
	txBuffer[8] = getLowByte(localHdlDataPacage.operCode);
	txBuffer[9] = localHdlDataPacage.targSubnet;
	txBuffer[10]= localHdlDataPacage.targId;
	if (localHdlDataPacage.additionContent.size > 0){
		for (i = 11; (localHdlDataPacage.additionContent.size + 10); i++){
			txBuffer[i] = localHdlDataPacage.additionContent.content[i-10];
		}
	}
	
	i = 10 + localHdlDataPacage.additionContent.size;
	crc = calculateCRC((unsigned int)i, txBuffer);
	
	i+=1;
	txBuffer[i] = getHighByte(crc);
	i+=1;
	txBuffer[i] = getLowByte(crc);
	
	open_closeUART(1);
//	write(uart0_filestream, &tx_buffer, sizeof(txBuffer);
	open_closeUART(0);
}

hdlDataPacage recieveHdlPacage(void){
	char rxBuffer[100];
	hdlDataPacage localHdlPacage;
	unsigned int crc;
	unsigned int checkCRC;
	
	open_closeUART(1) ;
//	read(uart0_filestream, (void*)rxBuffer, 255);
	open_closeUART(0);
	
	crc      = calculateCRC(strlen(rxBuffer), rxBuffer);
	checkCRC = twoCharToUint((strlen(rxBuffer) - 1), strlen(rxBuffer));
	
	if (crc == checkCRC){
		localHdlPacage = hdlDataParser(strlen(rxBuffer), rxBuffer);
		return localHdlPacage;
	}else{
		localHdlPacage.orgDeviceId = '0';
		return localHdlPacage;
	}	
}

int isIn(unsigned int currentId, hdlDeviceInfo *hdlDevsInfo[], unsigned int deviceNumber){
	
	int i = 0;
	
	for (i = 0; i < deviceNumber; i++){
		if (currentId == hdlDevsInfo[i].devId){
			return 1;
		}
	}
	return 0;
}















unchar Check_crc(unchar *ptr, unchar len)
{
 unint crc;
 unchar dat;
 crc=0;
 while(len--!=0)
 {
 dat=crc>>8; /* */
 crc<<=8; /* */
 crc^=CRC_TAB[dat^*ptr]; /* */
 ptr++;
 }
 dat=crc;
 if((*ptr==(crc>>8))&&(*(ptr+1)==dat))
 return(TRUE);
 else
 return(FALSE);
}
void Pack_crc(unchar *ptr, unchar len)
{
	 unint crc;
 	 unchar dat;
	 crc=0;
	
	 SubnetID:0x01
	 DevcieID:0x64

 while(len--!=0)
 {
 dat=crc>>8; /* */
 crc<<=8; /* */
 crc^=CRC_TAB[dat^*ptr]; /* */
 ptr++;
 }
 *ptr=crc>>8;
 ptr++;
 *ptr=crc;
}
unsigned int CRC_TAB[]={ /* CRC tab */
 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
 };














