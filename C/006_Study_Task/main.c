#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef int bool;
#define true 1
#define false 0 
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
bool chekNumber (int number);

int main(int argc, char *argv[]) {
	unsigned long int c,a=0;
	unsigned long int number=0;
	unsigned long int b=0;
	unsigned long int e=0;
	unsigned long int counter=0;
	printf("Enter the numbers: ");
	scanf("%d",&c);
	
	while(counter<c){
		number++;
		e=0;
		for(a=1;a<number;a++){
			b=number%a;
			if(b==0){
				e+=a;
				printf("%lu devider to %lu:  \n",a , number);
			}
		}
		
		if(e==number){
			counter++;
			printf("%d: %d\n",  counter, e);			
		}
	}
/*	while ( a < c )	{
		number++;
		if(chekNumber(number)){
			printf("%d Number \n", number);
			a++;
		}
		
	}
*/	
	return 0;
}
bool chekNumber (int number){
	int b,i;
	int calc=0;
	
	for(i=1; i <=number; i++){
		b = number%i;
		if (b==0){
			calc++;
		}			
	}
	if (calc==2){
		return true;
	}
	else{
		return false;
	}
}
