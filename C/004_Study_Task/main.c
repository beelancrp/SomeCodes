#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	int numberOfDay;
	
	printf("Enter the number of the day: ");
	scanf("%d", &numberOfDay);
	
	switch (numberOfDay){
		case 1 :{
			printf("Monday");
			break;
		}
		case 2 :{
			printf("Tuesday");
			break;
		}
		case 3 :{
			printf("Wednesday");
			break;
		}
		case 4 :{
			printf("Thursday");
			break;
		} 
		case 5 :{
			printf("Friday");
			break;
		}
		case 6 :{
			printf("Saturday");
			break;
		}
		case 7 :{
			printf("Sunday");
			break;
		}
		default :{ printf("You enter a wrong number!");
		}
	}
	
	return 0;
}
