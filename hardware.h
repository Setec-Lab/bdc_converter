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

#define 	_XTAL_FREQ 				32000000
#define		BAUD_RATE               9600
#define		ERR_MAX					500
#define		ERR_MIN					-500
#define 	DC_MIN                  103	    // DC = 0.2 MINIMUM
#define 	DC_MAX                  461		// DC = 0.9 MAX
#define     KP                      15  ///< Proportional constant divider 
#define     KI                      35  ///< Integral constant divider 
#define     VREF                    4800                  
#define     sVREF                   (uint16_t) ( ( ( VREF * 4096.0 ) / 5000 ) + 0.5 )
#define     CREF                    2000                  
#define     sCREF                   (uint16_t) ( ( ( CREF * 4096.0 ) / (5000 * 2.5 * 5 ) ) + 0.5 )
#define     VOC                     5400
#define     sVOC                    (uint16_t) ( ( ( VOC* 4096.0 ) / 5000 ) + 0.5 )

#define     COUNTER                 1024

#define		IS_BAT                  0b00000 //AN0 (RA0)
#define		VS_BAT                  0b00001 //AN1 (RA1)  
#define		VS_BUS                  0b00010 //AN2 (RA2)  

bool                                SECF = 0;
uint16_t                            count = COUNTER + 1; ///< Counter that should be cleared every second. Initialized as #COUNTER 
int24_t                             intacum;   ///< Integral acumulator of PI compensator
uint16_t                            dc = 0;  ///< Duty cycle     
bool                                log_on = 0; ///< Variable to indicate if the log is activated 
int16_t                             second = 0; ///< Seconds counter, resetted after 59 seconds.
uint16_t                            minute = 0; ///< Minutes counter, only manually reset
bool                                conv = 0; ///< Turn controller ON(1) or OFF(0). Initialized as 0
uint16_t                            vbus = 0;
uint16_t                            vbat = 0;
uint16_t                            ibat = 0;
uint24_t                            vbusa = 0;
uint24_t                            vbata = 0;
uint24_t                            ibata = 0;
uint16_t                            vbusp = 0;
uint16_t                            vbatp = 0;
uint16_t                            ibatp = 0;
uint16_t                            capap = 0;
uint16_t                            cvref = 4800;
uint16_t                            ocref = 2000;
uint16_t                            vref = 0;    
uint16_t                            ivref = 0; 
uint16_t                            iref = 0;
uint16_t                            voc = 0;
        
void initialize(void);
void pid(uint16_t feedback, uint16_t setpoint);
void set_DC(void);
uint16_t read_ADC(uint16_t channel);
void log_control(void);
void display_value_u(uint16_t value);
void control_loop(void);
void calculate_avg(void);
void interrupt_enable(void);
void UART_send_char(char bt);
char UART_get_char(void); 
void UART_send_string(char* st_pt);
void timing(void);


 

#define     LINEBREAK               {UART_send_char(10); UART_send_char(13);}

#define		SET_VOLTAGE(x)			{ vref = x; }
#define		SET_CURRENT(x)			{ iref = x; }

#define     RESET_TIME()            { minute = 0; second = -1; } ///< Reset timers.


#define		AD_SET_CHAN(x)          { ADCON0bits.CHS = x; __delay_us(1); }
#define		AD_CONVERT()            { GO_nDONE = 1; while(GO_nDONE);}
#define     AD_RESULT()             { ad_res = 0; ad_res = (ADRESL & 0xFF)|((ADRESH << 8) & 0xF00);} 


//DC-DC CONVERTER RELATED DEFINITION
#define		STOP_CONVERTER()		{ dc = 0; set_DC(); RA4 = 0; log_on = 0; UART_send_string((char *) "\n\r STOP: \n\r"); }
#define  	START_CONVERTER()		{ dc = DC_MIN; set_DC(); RA4 = 1; log_on = 1; UART_send_string((char *) "\n\r START: \n\r"); }




// unsigned int 						ad_res;
// unsigned int						v;  //ADDED
// unsigned int 						i;  //ADDED
// unsigned int 						t;  //ADDED
// unsigned int                        b;  //battery
// unsigned int 						count;             //ADDED
// uint24_t              				iprom;
// uint24_t                			vprom;
// uint24_t            				tprom;
// uint24_t                            bprom;
// unsigned int 						vref;
// unsigned int 						iref;
// char 								cmode;
// unsigned char 						cc_cv;
// unsigned int 						second;
// unsigned char 						esc;
// int 								pp;
// int 								pi;
// float 								kp;				//Proportional constant, seems too big data type
// float 								ki;				//Integral constant, seems too big data type
// uint16_t                            dc;             //Duty, check data size
// unsigned char 						spb;			//Baud rate set
// unsigned int            			log_on; 

uint16_t                counting = 0;
uint16_t                 PWM = 0;

char const              comma_str[] = ",";
char const              in_arr_str[] = "->";
char const              end_arr_str[] = "<-";
char const              in_wait_str[] = "------------W-";
char const              end_wait_str[] = "-W------------";
char const              hip_str[] = "-";
char const              in_sta_str[] = "S-";
char const              end_sta_str[] = "-S";
char const              C_str[] = "C";
char const              V_str[] = "V";
char const              I_str[] = "I";
char const              T_str[] = "T";





#endif /* HARDWARE_H*/