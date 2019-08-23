/**
 * @file main.c
 * @author Juan J. Rojas
 * @date 10 Nov 2018
 * @brief main loop for the BDC prototype converter.
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
    log_on = 1;
    while(1)
    {   
        if (recep_flag)
        {
            recep_flag = 0;
            switch (recep[0])
            {
                case 0x01: ///
                    if (char_count == 3) char_count++;
                    else char_count = 0;
                    break;
                case 0x03:
                    if (recep[1] == 0x01 || char_count == 2) char_count++;
                    else char_count = 0;
                    break;
                case 0x04:
                case 0x05: 
                case 0x06:
                case 0x07:
                case 0x08:
                    if (char_count == 1)
                    {
                        action = recep[0];
                        char_count++;
                    }else char_count = 0;
                    break;
                default: 
                    char_count = 0;
                }
                if (char_count == 4)
                {
                    switch (action)
                    {
                        case 0x04:
                            START_CONVERTER(); /// -# Start the converter by calling the #START_CONVERTER() macro.
                            RESET_TIME();
                            break;
                        case 0x05:
                            STOP_CONVERTER(); /// -# Stop the converter by calling the #STOP_CONVERTER() macro.
                            RESET_TIME();
                            break;
                        case 0x06:
                            dc++;
                            if (dc > DC_MAX) dc = DC_MAX;
                            break;
                        case 0x07:
                            dc--;
                            if (dc < DC_MIN) dc = DC_MIN;
                            break;
                        case 0x08:
                            break;
                    }
                    action = 0;
                    char_count = 0;
                }
        } 
        
        if (SECF)
        {
            SECF = 0;
            log_control_hex();
            //if ((vbatav < vbatmax) && (vbusr > ivbusr)) vbusr -= 2;
            if (vbatav < vbatmin)STOP_CONVERTER();
        }
	}
}

/**@brief This is the interruption service function. It will stop the process if an @b ESC or a @b "n" is pressed. 
*/
void __interrupt() ISR(void) 
{
    //char recep = 0; /// Define and initialize @p recep variable to store the received character
    
    if(TMR1IF)
    {
        TMR1H = 0xE1; //TMR1 Fosc/4= 8Mhz (Tosc= 0.125us)
        TMR1L = 0x83; //TMR1 counts: 7805 x 0.125us = 975.625us
        TMR1IF = 0; //Clear timer1 interrupt flag
        vbus = read_ADC(VS_BUS); /// * Then, the ADC channels are read by calling the #read_ADC() function
        vbat = read_ADC(VS_BAT); /// * Then, the ADC channels are read by calling the #read_ADC() function
        ibat = (int16_t)(read_ADC(IS_BAT)); /// * Then, the ADC channels are read by calling the #read_ADC() function
        //HERE 2154 is a hack to get 0 current
        ibat = 2048 - ibat; 
        if (conv){
//            control_loop(); /// -# The #control_loop() function is called*/
//            //if ((vbat >= vbatmax) && (vbusr < voc)) vbusr +=1; ///NEEDS CORRECTION
            set_DC(&dc);
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
        recep[1] = recep[0];
        recep[0] = RC1REG; /// * Empty the reception buffer and assign its contents to the variable @p recep   
        recep_flag = 1;
    }  
}