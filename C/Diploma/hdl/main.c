#include <stdio.h>
#include <stdlib.h>
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

typedef struct  {
	char content[MAX_CONTENT_SIZE];
	int  size;
}	additionalContent;

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
	return 0;
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
	}	
}



















/*
char hdlDataPacageCollector(additionalContent addContent, operationCode operCode){
	char header1 = '0xAA';		
	char header2 = '0xAA';
	char dataPacage[100];
	hdlDataPacage localHdlDataPacage;
	
	localHdlDataPacage.additionContent = addContent;
	localHdlDataPacage.orgSubnet	   = '1';
	localHdlDataPacage.orgDeviceId	   = '2';
	localHdlDataPacage.targSubnet	   = '255';
	localHdlDataPacage.targId		   = '255';
	localHdlDataPacage.orgDeviceType1  = '0xFF';
	localHdlDataPacage.orgDeviceType2  = '0xFE';
	localHdlDataPacage.operCode        = operCode;
	
	dataPacage[0] = header1;
	dataPacage[1] = header2;
	dataPacage[2] = localHdlDataPacage.dataLength;
	dataPacage[3] = localHdlDataPacage.orgSubnet;
	dataPacage[4] = localHdlDataPacage.orgDeviceId;
	dataPacage[5] = localHdlDataPacage.orgDeviceType1;
	dataPacage[6] = localHdlDataPacage.orgDeviceType2;
	dataPacage[7] = localHdlDataPacage.operCode.operCode1;
	dataPacage[8] = local
	HdlDataPacage.operCode.operCode2;
	dataPacage[9] = localHdlDataPacage.targSubnet;
	dataPacage[10]= localHdlDataPacage.targId;
	
	
	
}*/
