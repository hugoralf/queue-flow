#include<stdio.h>
#include<wiringPi.h>
#include<signal.h>
#include<time.h>
#include<unistd.h>
#include<string.h>

#define PIN_INPUT 7

//declaration of variables
int counter = 0;
char buffer_date[15];

//input pin
void GPIO_setup(){
	wiringPiSetup();
	wiringPiSetupGpio();
	pinMode(PIN_INPUT, INPUT);
}

//interrupt event on PIN_INPUT on edge rising
void GPIO_handler(){
	counter +=1;
	printf("ate agora: %d\n", counter);
}

//interrupt handling signal
void signal_handler(){
	static char buffer_data [16];
	static int i = 0, last_counter = 0, last_data[10];
	int aux = 0;
	if (i < 10){
		last_data[i] = (counter - last_counter);
		//printf("vetor 10_%d: %d %d\n", i,  last_data[i], counter);
		i++; 
	}else{
		for(i = 0; i < 10; i++){
			aux += last_data[i];
			printf("aux %d  ", aux);
		}
		printf("\nfluxo: %d\n", aux);
		for(i = 1; i < 10; i++)
			last_data[i - 1] = last_data[i];
		last_data[i - 1] = (counter - last_counter);	
	}
	snprintf(buffer_data, sizeof(buffer_data), "%d\n", (counter - last_counter));
	last_counter = counter;
	FILE *file_in = fopen(buffer_date, "a+");
	fprintf(file_in, buffer_data);
	fclose(file_in);
	alarm(3);
}

int main(){

time_t time_now;
strftime(buffer_date, sizeof(buffer_date), "%F", localtime(&time_now));
strcat(buffer_date, ".out");
printf("%s\n", buffer_date);
GPIO_setup();
signal(SIGALRM, signal_handler);
alarm(3);
	for(;;)
		wiringPiISR(PIN_INPUT, INT_EDGE_RISING, &GPIO_handler);
}
