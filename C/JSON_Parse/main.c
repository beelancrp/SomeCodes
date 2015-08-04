#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int bool;
	#define true  1;
	#define false 0;

int calcObjectsNumber (const char *str );



int main(int argc, char *argv[]) {
	int  stringLength = 0;
	char Json_obj[1000];
	int	 i = 0;
	bool start = false;
	bool stop  = false;
	printf("String: %s\n", argv[1]);	
//	strcpy(Json_obj,argv[1]);	
	strcpy(Json_obj, "{\"key1\":\"value1\",\"key2\":\"value2\"}");
	printf("Oblect numbers: %d",calcObjectsNumber(Json_obj));
	
	stringLength = strlen(Json_obj);
	 
	 struct jpar	{
	 	char jparse_key[64];
	 	char jparse_value[64];
	 };
	 struct jpar jsonObjects[calcObjectsNumber(Json_obj)];

	bool endOfKey=false;
	char key[64]="";
	char value[64]="";
	int objNumber=0;
	
	for (i=0; i<stringLength; i++) {

		 	
	 	switch(Json_obj[i]) {
	 		
			case '{':{
				start = true;
				printf("Start: %d\n",i);
				//continue;
			} 
			
	 		case 34:{
	 			printf("Case:2\n");
	 			if (!endOfKey){
	 				int k=0;
	 				
				 	while(Json_obj[i]!=34){
					 	i++;
					 	key[k]=Json_obj[i];
					 	k++;
					 }
					 
					 endOfKey=true;
					 key[k]=0;
					 strcpy(jsonObjects[objNumber].jparse_key, key);
					 strcpy(key,"");
					 printf("%s\n", jsonObjects[objNumber].jparse_key);
					 continue;
				 }
				 else{
				 	
				 } 			 			
	 		}
	 		
			case ':':{
				endOfKey=true;
				continue;
			}
			
			case ',' :{
				continue;
			}
			
			case '}':{
				
				stop = true;
				start = false;
				continue;
			}
			default  :{
				printf("Default: %d\n",i);
			}	
	 	}
			 		
	} 
	return 0;
}
	
int calcObjectsNumber (const char *str ){
	int count = 1;
	int i = 0;
		for (i=0; i<strlen(str); i++) {
			if (str[i] == ','){
				count++;
			}
		}
			
	return count;
}
//{"sotana":"demon","god":"angel"}
