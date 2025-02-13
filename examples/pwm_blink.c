#include <stdio.h>
#include <wiringPi.h>

#define LED 2  // GPIO Pin 2 (BCM numbering)
#define PWM_PIN 18 // Example PWM pin (BCM numbering) - Choose a suitable PWM-capable pin
#define PWM_RANGE 1024 // Adjust as needed
#define DUTY_CYCLE_PERCENT 50 // Duty cycle percentage (0-100)

int main(int argc, char *argv) {
    printf("Raspberry Pi PWM Blink\n");
    printf("=====================\n");

    if (wiringPiSetupGpio() == -1) {
        fprintf(stderr, "WiringPi setup failed!\n");
        return 1;
    }

    // Set PWM pin mode:
    pinMode(PWM_PIN, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS); // Optional: Set PWM mode (MS mode is common)
    pwmSetRange(PWM_RANGE); // Set PWM range (resolution)

    for (;;) {
        // Calculate duty cycle value:
        int dutyCycleValue = (DUTY_CYCLE_PERCENT * PWM_RANGE) / 100;

        // Set PWM duty cycle:
        pwmWrite(PWM_PIN, dutyCycleValue); 
        printf("PWM ON (Duty Cycle: %d%%)\n", DUTY_CYCLE_PERCENT);
        delay(1500);

        // Turn PWM off (0 duty cycle):
        pwmWrite(PWM_PIN, 0);  // Or just set to 0
        printf("PWM OFF\n");
        delay(1500);
    }

    return 0;
}
