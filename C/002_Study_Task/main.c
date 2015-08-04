#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	int a,b;
	printf("Enter the numbers!\n");
	scanf("%d %d", &a,&b);
	
	if(a > b){
		
		printf("Number A bigger then B\n%d",a);
	}	
	else {
		
		if (a == b){
			
			printf("A equales B!\n%d,%d",a,b);
		}
			
		else{
			
			printf("Number B bigger then A\n%d",b);
		}
	}
	
	return 0;
}
