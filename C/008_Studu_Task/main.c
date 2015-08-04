#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
	
	int n,i;
	int Ffib;
	
	printf("Enter the number:");
	scanf("%d",&n);
	
	if(n<=2){
		if(n==0){
			printf("0");
		}
		if(n==1){
			printf("1");
		}
	}
	else{
		for (i=2; i<n; i++){
			printf("%d\n",(i-1)+(i-2));
		}
	}
	
	return 0;
}
