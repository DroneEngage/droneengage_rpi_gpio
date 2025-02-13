#include <stdio.h>
#include <wiringPi.h>

#define	LED	(2)  // GPIO Pin2 

int main (int argc, char *argv [])
{
  printf ("Raspberry Pi Blink\n") ;
  printf ("=========================\n") ;

    
  wiringPiSetupGpio();
  pinMode(LED, OUTPUT);
  pullUpDnControl(pin_number, PUD_DOWN);
  for (;;)
  {
    digitalWrite (LED, HIGH) ;	// On
    printf("ON\n");
    delay (1500) ;		// mS
    digitalWrite (LED, LOW) ;	// Off
    printf("off\n");	
    delay (1500) ;
  }

  return 0 ;
}
