#include <stdio.h>
#include <stdint.h>

//constants for accessing timer in memory
volatile uint32_t * const TIMER_START_VAL = (uint32_t *) 0xFFFEC600;
volatile uint32_t * const TIMER_SNAPSHOT = (uint32_t *) 0xFFFEC604;
volatile uint32_t * const TIMER_CONTROL = (uint32_t *) 0xFFFEC608;

//32 bit gcd avalon memory interface
volatile uint32_t * const param_a_32 = (uint32_t *) 0xFF200018;
volatile uint32_t * const param_b_32 = (uint32_t *) 0xFF200014;
volatile uint32_t * const output_32 = (uint32_t *) 0xFF200010;
volatile uint8_t * const done_32 = (uint32_t *) 0xFF20001C;

//64 bit gcd avalon memory interface (not done yet, these are incorrect)
volatile uint32_t * const param_a_64_hi = (uint32_t *) 0xFF200094;
volatile uint32_t * const param_a_64_lo = (uint32_t *) 0xFF200090;
volatile uint32_t * const param_b_64_hi = (uint32_t *) 0xFF20008C;
volatile uint32_t * const param_b_64_lo = (uint32_t *) 0xFF200088;
volatile uint64_t * const output_64 = (uint32_t *) 0xFF200080;
volatile uint8_t * const done_64 = (uint32_t *) 0xFF20001D;


void start_timer(){
    //stop the timer
    *TIMER_CONTROL = 0x00000002;
    //set the timer interval to max
    *TIMER_START_VAL = 0xFFFFFFFF;
    //start the timer
    *TIMER_CONTROL = 0x00000003;
}

uint32_t get_elapsed_time(){
    //stop the timer
    *TIMER_CONTROL = 0x00000002;
    return (0xFFFFFFFF - *TIMER_SNAPSHOT);
}

uint32_t get_GCF_32(uint32_t a, uint32_t b) {

    // Swap if B > A
    if (a < b) {
        uint32_t temp = a;
        a = b;
        b = temp;
    }

    // Euclid's subtraction algorithm
    while (a != b) {
        if (a > b) {
            a = a - b;
        } else {
            b = b - a;
        }
    }
    return a;
}

uint32_t get_hw_GCF_32(uint32_t a, uint32_t b) {
    *param_a_32 = a;
    *param_b_32 = b;

    while(*done_32 == 0){
    }

    //return the output from memory
    return *output_32;
}

uint64_t get_GCF_64(uint64_t a, uint64_t b) {

    // Swap if B > A
    if (a < b) {
        uint64_t temp = a;
        a = b;
        b = temp;
    }

    // Euclid's subtraction algorithm
    while (a != b) {
        if (a > b) {
            a = a - b;
        } else {
            b = b - a;
        }
    }
    return a;
}

uint64_t get_hw_GCF_64(uint64_t a, uint64_t b) {
    //grab the upper and lower words of each operand and assign to hw module
    *param_a_64_lo = *((uint32_t *)&a);
    *param_a_64_hi = *(((uint32_t *)&a) + 1);
    *param_b_64_lo = *((uint32_t *)&b);
    *param_b_64_hi = *(((uint32_t *)&b) + 1);
    
    //wait until done signal is sent from hw
    while(done_64 == 0){
    }

    //return the 64 bit value from hw output
    return *output_64;
}

int main() {
    int selection;
    uint32_t REG_A;
    uint32_t REG_B;
    uint64_t REG_A_64;
    uint64_t REG_B_64;
    uint32_t elapsed_time;

    while(1) {

        //ask user for which version of gcd
        printf("32 or 64 bit GCD?: ");
        scanf("%d", &selection);

        if(selection == 32) {

            //ask for user input 32 bit values
            printf("Enter an unsigned 32 bit number for reg A: ");
            scanf("%u", &REG_A);
            printf("Enter an unsigned 32 bit number for reg B: ");
            scanf("%u", &REG_B);

            //calculate result in software
            printf("\nBegin software gcf:\n");
            start_timer();
            uint32_t GCF = get_GCF_32(REG_A, REG_B);
            elapsed_time = get_elapsed_time();
            printf("GCD = %u\n", GCF);
            printf("Time: %.3fus\n\n", (float)elapsed_time/200.0);

            //calculate result in hardware
            printf("\nBegin hardware gcf:\n");
            start_timer();
            GCF = get_hw_GCF_32(REG_A, REG_B);
            elapsed_time = get_elapsed_time();
            printf("GCD = %u\n", GCF);
            printf("Time: %.3fus\n\n", (float)elapsed_time/200.0);
        }
        else if(selection == 64) {
            //ask for user input 64 bit values
            printf("Enter an unsigned 64 bit number for reg A: ");
            scanf("%llu", &REG_A_64);
            printf("Enter an unsigned 64 bit number for reg B: ");
            scanf("%llu", &REG_B_64);
            
            //calculate result in software
            printf("\nBegin software gcf:\n");
            start_timer();
            uint64_t GCF_64 = get_GCF_64(REG_A_64, REG_B_64);
            elapsed_time = get_elapsed_time();
            printf("GCD = %llu\n", GCF_64);
            printf("Time: %.3fus\n\n", (float)elapsed_time/200.0);

            //calculate result in hardware
            printf("\nBegin hardware gcf:\n");
            start_timer();
            GCF_64 = get_hw_GCF_64(REG_A_64, REG_B_64);
            elapsed_time = get_elapsed_time();
            printf("GCD = %llu\n", GCF_64);
            printf("Time: %.3fus\n\n", (float)elapsed_time/200.0);
        }
    }
    return 0;
}