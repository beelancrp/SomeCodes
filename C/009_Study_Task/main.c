#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	
	char arr1[10];
	int i,calc,sizeofar;
	
	printf("Enter:");
	scanf("%s", arr1);
	
	calc=0;
	
	sizeofar = strlen(arr1);
	
	for (i=0; i<=sizeofar ;i++){
		if (calc<0){
			break;
		}
		if (arr1[i]=='('){
			calc++;	
		}
		if (arr1[i]==')'){
			calc--;
		}
		printf("%d\n", calc);
	}
	
	
	if (calc==0){
		printf("yes");
	}
	else{
		printf("no");
	}
	
	return 0;
}
