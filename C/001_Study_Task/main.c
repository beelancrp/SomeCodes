#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	
	float x,y;
	int i=0;
	printf("\t  x\t|\t  y\n");
	for (x=-5; x<=5; x+=0.5){
		i++;
		y = 5 - ((x*x)/2);
		
		printf("%d:\t %4.1f\t|\t%4.1f ;\n",i,x,y);
	}
	return 0;
}
