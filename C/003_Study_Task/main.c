#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	int a,b,c;
	
	printf("Enter the numbers!\n");
	scanf("%d %d %d",&a,&b,&c);

		if (a > b){
			if(b > c){
				printf("Number A the biggest!\n%d",a);
			}
			else{
				if (a > c){
					printf("Number A the biggest!\n%d",a);
				}
				else{
					printf("Number C the biggest!\n%d",c);
				}
			}
		}
		else{
			if(b > c){
				printf("Number B the biggest!\n%d",b);
			}
			else{
				printf("Number C the biggest!\n%d",c);
			}
		}
	
	return 0;
}
