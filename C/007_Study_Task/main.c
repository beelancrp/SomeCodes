#include <stdio.h>
#include <stdlib.h>
//int calcMaxNumber (int a, int b, int c);
/* run this program using the console pauser or add your own getch, system("pause") or input loop */


int main(int argc, char *argv[]) {
	
	int a,b,c;
	int max=0;
	
	printf("Enter the numbers:");
	scanf("%d%d%d", &a,&b,&c);
	
	max = calcMaxNumber(a,b,c);
	

	if (max==a){
		if (b+c > max){
			printf("Triangle!");
		}
		else{
			printf("Not Triangle!");
		}
	}
	else {
		if(max==b){
			if (a+c > max){
				printf("Triangle!");
			}
			else{
				printf("Not Triangle!");
			}
		}
		else{
			if (max==c){
				if (a+b > max){
					printf("Triangle!");
				}
				else{
					printf("Not Triangle!");
				}
			}
		}
	}
	

	return 0;
}

int calcMaxNumber (int a, int b, int c){
	
	int maxNumber = 0;
		
	if (a > b){
		if (b > c){
			maxNumber = a;
		}
		else{
			if (c > a){
				maxNumber = c;
			}
		}
	}
	else{
		if(b > c){
			maxNumber = b;
		}
		else{
			maxNumber = c;
		}
	}
	
	return maxNumber;
}

