/**
 * @file hardware.h
 * @author Juan J. Rojas
 * @date 10 Nov 2018
 * @brief Definitions for the BDC prototype controller
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail (after leaving Kyutech):
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/bdc_prototype/src/master
 */

#ifndef HARDWARE_H
    #define HARDWARE_H
    // PIC16F1786 Configuration Bit Settings

    // 'C' source line config statements
    #pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
    #pragma config WDTE = OFF       // Watchdog Timer Disabled (WDT disabled)
    #pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
    #pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)//If this is enabled, the Timer0 module will not work properly.
    #pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
    #pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
    #pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset enabled)
    #pragma config CLKOUTEN = ON    // Clock Out Negative Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
    #pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
    #pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

    // CONFIG2
    #pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
    #pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (Vcap functionality is disabled on RA6.)
    #pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
    #pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
    #pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
    #pragma config LPBOR = OFF      // Low Power Brown-Out Reset Enable Bit (Low power brown-out is disabled)
    #pragma config LVP = OFF        // Low-Voltage Programming Enable (Low-voltage programming disabled)//IF THIS IN ON MCLR is always enabled

    #include <xc.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <stdint.h> // To include uint8_t and uint16_t
    #include <string.h>
    #include <stdbool.h> // Include bool type

unsigned int 						ad_res;
unsigned int						v;  //ADDED
unsigned int 						i;  //ADDED
unsigned int 						t;  //ADDED
unsigned int 						count;             //ADDED
unsigned short long   				iprom;
unsigned short long 				vprom;
unsigned short long					tprom;
unsigned int 						vref;
unsigned int 						iref;
char 								cmode;
unsigned char 						cc_cv;
unsigned int 						second;
unsigned char 						esc;
int 								pp;
int 								pi;
float 								kp;				//Proportional constant, seems too big data type
float 								ki;				//Integral constant, seems too big data type
uint16_t                            dc;             //Duty, check data size
unsigned char 						spb;			//Baud rate set
unsigned int            			log_on; 

uint16_t                counting = 0;
uint16_t                 PWM = 0;

#define		ERR_MAX					4095
#define		ERR_MIN					-4095
#define		SET_VOLTAGE(x)			{ vref = x; }
#define		SET_CURRENT(x)			{ iref = x; }

#define 	_XTAL_FREQ 				32000000
#define		BAUD_RATE               9600

#define		V_CHAN                  0b00001 //AN1 (RA1) 
#define		I_CHAN                  0b00000 //AN0 (RA0)
//#define		T_CHAN                  0b010011 //RC3

#define		CELL1_ON				PORTAbits.RA7 = 1
#define		CELL2_ON				PORTAbits.RA6 = 1
#define		CELL3_ON				PORTCbits.RC0 = 1
#define		CELL4_ON				PORTCbits.RC1 = 1
#define		CELL1_OFF				PORTAbits.RA7 = 0
#define		CELL2_OFF				PORTAbits.RA6 = 0
#define		CELL3_OFF				PORTCbits.RC0 = 0
#define		CELL4_OFF				PORTCbits.RC1 = 0

#define		AD_SET_CHAN(x)          { ADCON0bits.CHS = x; __delay_us(1); }
#define		AD_CONVERT()            { GO_nDONE = 1; while(GO_nDONE);}
#define     AD_RESULT()             { ad_res = 0; ad_res = (ADRESL & 0xFF)|((ADRESH << 8) & 0xF00);} 
//CONTROL LOOP RELATED DEFINITION
#define     CURRENT_MODE            4                               //Number of times the voltage should be equal to the CV voltage in order to change to CV mode.

#define     LINEBREAK               UART_send_char(10)

//DC-DC CONVERTER RELATED DEFINITION
#define		STOP_CONVERTER()		{ dc = 0; set_DC(); /*TRISAbits.TRISA4 = 1; PORTAbits.RA1 = 1;*//*TURN OFF RELAY*/ Cell_OFF(); LOG_OFF(); v = 0; i = 0; t = 0; vprom = 0; iprom = 0; tprom = 0;}
#define  	START_CONVERTER()		{ dc = DC_MIN; /*TRISAbits.TRISA4 = 0; PORTAbits.RA1 = 0;*/ /*TURN ON RELAY*/ Cell_ON(); }

#define 	LOG_ON()				{ log_on = 1; }
#define 	LOG_OFF()				{ log_on = 0; }

//PARAMETER OF CHARGE AND DISCHARGE
#define     PARAM_CHAR()        	{ kp=0.1; ki=0.05; SET_CURRENT(i_char); /*PORTAbits.RA0 = 0;*/ cmode=1; pi = 0; pp = 0; EOCD_count = 4;}
#define     PARAM_DISC()        	{ kp=0.1; ki=0.05; SET_CURRENT(i_disc); /*PORTAbits.RA0 = 1;*/ cmode=1; pi = 0; pp = 0;  EOCD_count = 4;} //MAYBE THAT THING CHARGE CAN DISAPEAR
#define     PARAM_DCRES()       	{ kp=0.1; ki=0.05; SET_CURRENT(capacity / 5); /*PORTAbits.RA0 = 1;*/ cmode=1; pi = 0; pp = 0; dc_res_count = 14;}

#define 	DC_MIN         358		// DC = 0.7 MINIMUM
#define 	DC_MAX         461		// DC = 0.9 MAX

 
#define     COUNTER        1000


void Initialize_Hardware(void);
void Init_Registers(void);
void Shutdown_Peripherals(void);
void pid(unsigned int feedback, unsigned int setpoint);
void set_DC(void);
void read_ADC(void);
void log_control(void);
void display_value(unsigned int value);
void cc_cv_mode(void);
void char_disc(void);
void control_loop(void);
void calculate_avg(void);
void Init_UART(void);
void UART_interrupt_enable(void);
void UART_send_char(char bt);
char UART_get_char(void); 
void UART_send_string(char* st_pt);
void Cell_ON(void);
void Cell_OFF(void);

#endif /* HARDWARE_H*/