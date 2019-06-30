/**
 * @file main.c
 * @author Juan J. Rojas
 * @date 10 Nov 2018
 * @brief main loop for the BDC prototype controller.
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail (after leaving Kyutech):
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/bdc_prototype/src/master
 */

#include "hardware.h"

uint8_t                 SEC = 0;
uint8_t                 MIN = 0;

void main(void){
    Init_Registers();
	Initialize_Hardware();
    Init_UART();    
    count = COUNTER; 
    iprom = 0;
    vprom = 0;
    tprom = 0;
    vref = 4800;
    esc = 0;   
    v = 0;
    kp = 0.07;
    ki = 0.03;
    __delay_ms(10);

    TRISBbits.TRISB0 = 0;               //Set RB0 as output. led
    ANSELBbits.ANSB0 = 0;               //Digital
    Interrupt_enable();
    TRISC0 = 1;
    TRISC2 = 1; 
    while(1){                      
        if (counting > 1000){
            if (LATB0){
            RB0 = 0;
            }else RB0 = 1; 
            counting = 0;
            if(SEC < 59) SEC++;
            else{SEC = 0; MIN++;}
            UART_send_string("\n\r");
            UART_send_string("LED ON/OFF");
            UART_send_string("\n\r");
            UART_send_string("D: ");
            display_value(((unsigned int)dc*0.1953));    
            UART_send_string("\n\r");
            UART_send_string("V: ");
            display_value((unsigned int)vprom);    
            UART_send_string("\n\r");
            UART_send_string("B: ");
            display_value((unsigned int)bprom);    
            UART_send_string("\n\r");
            if ((bprom < 4150) && (vref > 4800)) vref -= 2;
            if (bprom < 2500)
            {
                TRISC0 = 1;                         //Deactivate PWM
                TRISC2 = 1;
            }
        }	  
	}
}

void interrupt ISR(void) 
{
    if(TMR1IF)
    {
        TMR1H = 0xE0;//TMR1 Fosc/4= 8Mhz (Tosc= 0.125us)
        TMR1L = 0xC0;//TMR1 counts: 8000 x 0.125us = 1ms
        PIR1bits.TMR1IF= 0; //Clear timer1 interrupt flag
        calculate_avg();  
        counting++; 
        read_ADC();
        pid(v, vref);
        if ((b >= 4150) && (vref < 5400)) vref +=1;
    }
    if(RCIF)
    {
        if(RC1STAbits.OERR) // check for Error 
        {
            RC1STAbits.CREN = 0; //If error -> Reset 
            RC1STAbits.CREN = 1; //If error -> Reset 
        }
        
        while(RCIF) esc = RC1REG; //receive the value and put it to esc
        if (esc == 0x1B)
        {
            TRISC0 = 1;                         //Deactivate PWM
            TRISC2 = 1;
        }else if (esc == 0x72)//restart "r"
        {
            TRISC0 = 0;
            TRISC2 = 0;      
            vref = 4800;
        }else if (esc == 0x61)//a
        {
            TRISC0 = 0;
            TRISC2 = 1;
        }else if  (esc == 0x7A)//z
        {
            //nothing
        }else
        {
            esc = 0;
        }
        UART_send_string("\n\r REC: \n\r");
        UART_send_char(esc);
        UART_send_string("\n\r");
    }   
}