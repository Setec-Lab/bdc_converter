/**
 * @file main.c
 * @author Juan J. Rojas
 * @date 10 Nov 2018
 * @brief main loop for the BDC prototype controller or converter.
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail (after leaving Kyutech):
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/bdc_prototype/src/master
 */

#include "hardware.h"

/**@brief This is the main function of the program.
*/


void main(void)
{
    initialize(); /// * Call the #initialize() function
    __delay_ms(10);
    interrupt_enable();
    voc = sVOC; 
    ivbusr = sVREF;
    vbusr = sVREF;
    vbatmin = sVBATMIN;
    vbatmax = sVBATMAX;
    iref = sCREF;
    log_on = 1;
    while(1)
    {           
        if (SECF) /// * The following tasks are executed every second:
        {     
            SECF = 0;
            //log_control_hex();
            //if ((vbatav < vbatmax) && (vbusr > ivbusr)) vbusr -= 2;
//            #if CONVERTER    
//            if (vbatav < vbatmin)
//            {
//                STOP_CONVERTER();
//                TRISC0 = 1;
//            }
//            #endif
            display_value_u((uint16_t)second);
            LINEBREAK;
            display_value_u(vbus);
            LINEBREAK;
            display_value_u(vbusr);
            LINEBREAK;
            display_value_u(dc);
            LINEBREAK;
        }       
	}
}

/**@brief This is the interruption service function. It will stop the process if an @b ESC or a @b "n" is pressed. 
*/
void __interrupt() ISR(void) 
{
    char recep = 0; /// Define and initialize @p recep variable to store the received character
    
    if(TMR1IF)
    {
        TMR1H = 0xE1; //TMR1 Fosc/4= 8Mhz (Tosc= 0.125us)
        TMR1L = 0x83; //TMR1 counts: 7805 x 0.125us = 975.625us
        TMR1IF = 0; //Clear timer1 interrupt flag
        vbus = read_ADC(VS_BUS); /// * Then, the ADC channels are read by calling the #read_ADC() function
        vbat = read_ADC(VS_BAT); /// * Then, the ADC channels are read by calling the #read_ADC() function
        ibat = (int16_t)(read_ADC(IS_BAT)); /// * Then, the ADC channels are read by calling the #read_ADC() function
        //HERE 2154 is a hack to get 0 current
        ibat = 2048 - ibat; ///If the #state is #CHARGE or #POSTCHARGE change the sign of the result  
        if (conv){
            control_loop(); /// -# The #control_loop() function is called*/
            //if ((vbat >= vbatmax) && (vbusr < voc)) vbusr +=1; ///NEEDS CORRECTION
        }
        calculate_avg(); /// * Then, averages for the 250 values available each second are calculated by calling the #calculate_avg() function
        timing(); /// * Timing control is executed by calling the #timing() function         
    }
    
    if(RCIF)/// If the UART reception flag is set then:
    {
        if(RC1STAbits.OERR) /// * Check for errors and clear them
        {
            RC1STAbits.CREN = 0;  
            RC1STAbits.CREN = 1; 
        }
        while(RCIF) recep = RC1REG; /// * Empty the reception buffer and assign its contents to the variable @p recep
        switch (recep)
        {
        case 0x63: /// * If @p recep received a @b "c", then:
            STOP_CONVERTER(); /// -# Stop the converter by calling the #STOP_CONVERTER() macro.
            RESET_TIME() 
            //TRISC0 = 1;                         /// * Set RC0 as input
            vbusr = ivbusr;
            break;
        case 0x73: /// * If @p recep received an @b "s", then:
            START_CONVERTER(); /// -# Start the converter by calling the #START_CONVERTER() macro.
            RESET_TIME() 
            //TRISC0 = 0; //MAYBE BPUT IT INSIDE START                        /// * Set RC0 as output
            vbusr = ivbusr;
            break;
        default:
            recep = 0;
        }
    }  
   
}