#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	float a,b,c,x1,x2,D;
	
	printf("Enter the arguments A,B,C: ");
	scanf("%f %f %f", &a, &b, &c);
	
	if(a < 0){
		printf("You are LOH! A-must be >0!");
	}
	else{
		D = (b*b)-(4*a*c);
		if(D > 0){
			
			x1 = (-b + (sqrt(D)))/(2*a);
			x2 = (-b - (sqrt(D)))/(2*a);
			
			printf("X1=%f, X2=%f", x1,x2);
		}
		else{
			if(D == 0){
				
				x1,x2 = (-b)/(2*a);
				printf("X1=X2=$f", x1,x2);
			}
			else{
				printf("There are no answers!");
			}
		}
	}
	
	return 0;
}
