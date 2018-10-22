/* Main loop for Charge and Discharge System. */
/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  main.c 
 * Author: Juan J. Rojas
 * Version control: Git
 */

#include "hardware.h"
#include "state_machine.h"

char const              next_cell_str_main[] = "---------->NEXT_CELL<----------";
uint16_t                counting = 0;
uint8_t                 PWM = 0;
uint8_t                 US_COUNT = 10;  //count 10 because f=10Khz
uint8_t                 MS_FLAG = 0;
uint8_t                 SEC = 0;
uint8_t                 MIN = 0;

void main(void){
    Init_Registers();
	Initialize_Hardware();
    Init_UART();
    state = STANDBY;       
    count = COUNTER; 
    iprom = 0;
    vprom = 0;
    tprom = 0;
    wait_count = 0;
    dc_res_count = 0;
    esc = 0;   
    v = 0;
    kp = 0.01;
    ki = 0.01;
    __delay_ms(10);

    TRISBbits.TRISB0 = 0;               //Set RB0 as output. led
    ANSELBbits.ANSB0 = 0;               //Digital
    UART_interrupt_enable();
    PWM = 180;
    TRISC0 = 1;  
    while(1){        
        if(TMR0IF){
            TMR0IF = 0;   
            TMR0 = 0x9E;         
            US_COUNT --;
            if (!US_COUNT){
                US_COUNT = 10;
                MS_FLAG = 1;
            }
        }    
        if(MS_FLAG){    
            MS_FLAG = 0;            
            counting++;            
            //pid(v, 7000);
            if (counting >= 1000){
                if (LATB0){
                RB0 = 0;
                }else RB0 = 1; 
                counting = 0;
                if(SEC < 59) SEC++;
                else{SEC = 0; MIN++;}
                UART_send_string("LED ON/OFF");
                LINEBREAK;
                //PWM from 0.1 to 0.9
//                if (PWM >= 230){
//                    PWM = 25;
//                }else PWM ++;
                PSMC1DCL = PWM;     
                PSMC1CONbits.PSMC1LD = 1; //Load Buffer
//                display_value(MIN);
//                UART_send_string(":");
//                display_value(SEC);
//                UART_send_string("\n\r");
                UART_send_string("D: ");
                display_value(((unsigned int)PWM*0.3906));    
                UART_send_string("\n\r");
                UART_send_string("V: ");
                display_value((unsigned int)v);    
                UART_send_string("\n\r");
            }
            read_ADC();
            calculate_avg();      
			//State_Machine();
            //LOG_ON();
            //log_control();
		}        
	}
}

void interrupt serial_interrupt(void) 
{
    
    if(RCIF)
    {
        if(RC1STAbits.OERR) // check for Error 
        {
            RC1STAbits.CREN = 0; //If error -> Reset 
            RC1STAbits.CREN = 1; //If error -> Reset 
        }
        
        while(RCIF) esc = RC1REG; //receive the value and put it to esc

//        if (esc == 0x1B)
//        {
//            state = STANDBY;
//            esc = 0;
//            wait_count = 0;
//            dc_res_count = 0;
//        }else if  (esc == 110)
//        {
//            STOP_CONVERTER();
//            LINEBREAK;
//            UART_send_string(next_cell_str_main);
//            LINEBREAK;
//            __delay_ms(50);
//            cell_count++;
//            state = IDLE;  
        if (esc == 0x1B)
        {
            TRISC0 = 1;                         //Deactivate PWM
        }else if (esc == 0x72)//+
        {
            TRISC0 = 0;
        }else if (esc == 0x61)//a
        {
            PWM++;
        }else if  (esc == 0x7A)//z
        {
            PWM--;
        }else
        {
            esc = 0;
        }
        UART_send_string("\n\r REC: \n\r");
        UART_send_char(esc);
        UART_send_string("\n\r");
    }    
}