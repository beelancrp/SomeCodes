#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void deleteSpaces (char *orgnlString, char *newString);
int isLowerCase (char ch);
int findSubString (char *string, char *subString);

int main(int argc, char *argv[]) {	
	int charNumber=0;
	int codedTextLength;
	char key[]="aaaaabbbbbabbbaabbababbaaababaab";
	char alphabet[]="abcdefghijklmnopqrstuvwxyz";
	char codedText[]="Hot sUn BEATIng dOWN bURNINg mY FEet JuSt WalKIng arOUnD HOt suN mAkiNG me SWeat";
	char message[64]="";
			
	deleteSpaces(codedText,codedText);
	codedTextLength=strlen(codedText);
	
	for (charNumber=0; charNumber < codedTextLength; charNumber++){
		if (isLowerCase(codedText[charNumber])){
			codedText[charNumber]='a';
		}
		else{
			codedText[charNumber]='b';
		}
	}
	
	for (charNumber=0; charNumber < ((codedTextLength/5)*5);){
		int i;
		char localString[6];
		
		for (i=0; i < 5; i++){
			localString[i]=codedText[charNumber];
			charNumber++;
		}
		localString[i]=0;
		message[(charNumber-1)/5]=alphabet[findSubString(key,localString)];
		
		//printf("%d - %s [%d] \n", findSubString(key,localString) ,localString, strlen(localString));
	}
	message[charNumber/5]=0;
	
	printf("%s", message);
	return 0;
}

void deleteSpaces (char *orgnlString, char *newString){
	
	int i,j; 
	int stringLength = strlen(orgnlString);
	
	j = 0;
	for (i=0; i < stringLength; i++){
		if (orgnlString[i]!=' '){
			newString[j]=orgnlString[i];
			j++;
		}
		else{
			continue;
		}	
	}
	newString[j]=0;
}

int isLowerCase (char ch){
	
	if ((ch>=97 && ch<=122) || (ch>=65 && ch<=90)){
		if (ch>=97 && ch<=122)
			return 1;
		else	
			return 0;
	}
	else
		return -1;
}

int findSubString (char *string, char *subString){
	
	int i,j,count,charNumber;
	int subStringLength = strlen(subString);
	int stringLength = strlen (string);
	charNumber=-1;
	
	count = 0;
	for (i=0; i < stringLength; i++){
		for (j=count; j < subStringLength; j++){
//			printf("%d > %c [%d] - %c [%d]\n", i, string[i], i ,subString[j], j);
			if (string[i]==subString[j]){
				count++;
//				printf("%d \n", count);
				break;
			}
			else{
				count=0;
				break;
			}
		}

		//printf("%d\n", count);
		if(count==subStringLength){
			charNumber = i-subStringLength+1;
			break;
		}
	}
	
	return charNumber;
}
