/* State machine source file for Charge and Discharge System. */
/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  state_machine.c 
 * Author: Juan J. Rojas.
 * Version control in Git
 */
#include "state_machine.h"
#include "hardware.h"

unsigned char           start;                      //variable to receive the start command
//unsigned char           chem;                       //chem = 49 -> Li-Ion, chem = 50-> Ni-MH || Other options can be included in the future  
unsigned char           c_char;                     //option for charging current in C
unsigned char           c_disc;                     //option for discharging current in C
unsigned char           option;                     //choose between four options, different for each chemistry, look into Define_Parameter for details
unsigned int            capacity;                   //definition of capacity per cell according to each chemistry
unsigned int            i_char;                     //charging current in mA
unsigned int            i_disc;                     //discharging curretn in mA
//unsigned int            c_1_dcres;                  //current for first step of DC resistance measurement (0.2C for 10 seconds)
//unsigned int            c_2_dcres;                  //current for second step of DC resistance measurement (1C for 1 second)
unsigned char           cell_count;                 //cell counter from 1 to 4.
unsigned char           cell_max;                   //number of cells to be tested
unsigned int            EOCD_count;                 //counter for EOC 
//unsigned int            ocv_count;                  //counter for OCV measurement
unsigned int            wait_count;                 //counter for waiting time between states
//unsigned int            small_count;                //counter for waiting time inside states
unsigned int            dc_res_count;               //counter for DC resistance step
unsigned char           state;                      //used with enum "states", defined in state_machine.h
unsigned char           previous_state;             //used with enum "states" 
//unsigned int            state_counter;              //used to show numerical value of the state
unsigned int            EOC_current;                //end of charge current in mA
unsigned int            EOD_voltage;                //end of dischage voltage in mV
//double                  vmax;                       //maximum recorded voltage
//double                  In_ocv;
//double                  Pre_ocv;
unsigned int            v_1_dcres;
unsigned int            i_1_dcres;
unsigned int            v_2_dcres;
unsigned int            i_2_dcres;
unsigned short long     dc_res_val;

char const              wait_str[] = "STATE = WAIT";
char const              prechar_str[] = "STATE = PRECHARGE";
char const              idle_str[] = "STATE = IDLE";
char const              standby_str[] = "STATE = STANDBY";
char const              charge_str[] = "STATE = CHARGE";
char const              discharge_str[] = "STATE = DISCHARGE";
char const              fault_str[] = "STATE = FAULT";
char const              press_s_str[] = "Press 's' to start: ";
char const              starting_str[] = "Starting...";
char const              DS_DC_res_str[] = "STATE = DISCHARGED STATE DC RESISTANCE";
char const              CS_DC_res_str[] = "STATE = CHARGED STATE DC RESISTANCE";
char const              done_str[] = "STATE = DONE";
char const              state_res_str[] = "-------------S-";
char const              end_state_res_str[] = "-S-------------";
char const              DC_res_str[] = "------------>R";
char const              end_str[] = "<------------";
char const              num_1and2_str[] = "Please input a number between 1 and 2";
char const              num_1and3_str[] = "Please input a number between 1 and 3";
char const              num_1and4_str[] = "Please input a number between 1 and 4";         
char const              param_def_str[] = "---Parameter definition for charger and discharger---";
char const              restarting_str[] = "Restarting...";
char const              chem_def_liion[] = "Chemistry defined as Li-Ion";
char const              chem_def_nimh[] = "Chemistry defined as Ni-MH";
char const              mv_str[] = " mV";
char const              mAh_str[] = " mAh";
char const              mA_str[] = " mA";
char const              EOD_V_str[] = "End of discharge voltage: ";
char const              EOC_I_str[] = "End of charge current: ";
char const              cho_bet_str[] = "Chose between following options: ";
char const              quarter_c_str[] = "(1) 0.25C";
char const              half_c_str[] = "(2) 0.50C";
char const              one_c_str[] = "(3) 1C";
char const              cell1_str[] = "Cell 1"; 
char const              cell2_str[] = "Cell 2"; 
char const              cell3_str[] = "Cell 3"; 
char const              cell4_str[] = "Cell 4"; 
char const              dis_def_quarter_str[] = "Discharge current defined as 0.25C";
char const              dis_def_half_str[] = "Discharge current defined as 0.5C";
char const              dis_def_one_str[] = "Discharge current defined as 1C";
char const              char_def_quarter_str[] = "Charge current defined as 0.25C";
char const              char_def_half_str[] = "Charge current defined as 0.5C";
char const              char_def_one_str[] = "Charge current defined as 1C";
char const              cv_val_str[] = "Constant voltage value: ";
char const              nom_cap_str[] = "Nominal capacity: ";
char const              def_char_curr_str[] = "Define charge current (input the number): ";
char const              def_disc_curr_str[] = "Define discharge current (input the number): ";
char const              def_num_cell_str[] = "Define number of cells to be tested (input the number): ";
char const              num_cell_str[] = "Number of cells to be tested: ";
char const              one_str[] = "1";
char const              two_str[] = "2";
char const              three_str[] = "3";
char const              four_str[] = "4";
char const              li_ion_op_1_str[] = "(1) Precharge->Discharge->Charge";
char const              li_ion_op_2_str[] = "(2) Discharge->Charge";
char const              li_ion_op_3_str[] = "(3) Only Charge";
char const              li_ion_op_4_str[] = "(4) Only Discharge";
char const              li_ion_op_1_sel_str[] = "Precharge->Discharge->Charge selected...";
char const              li_ion_op_2_sel_str[] = "Discharge->Charge selected...";
char const              li_ion_op_3_sel_str[] = "Only Charge selected...";
char const              li_ion_op_4_sel_str[] = "Only Discharge selected...";
char const              cell_below_str[] = "Cell below 0.9V or not present";
char const              next_state_str[] = "---------->NEXT_STATE<----------";
char const              next_cell_str[] = "---------->NEXT_CELL<----------";
 
void Init_State_Machine()
{
    INTCONbits.GIE = 0;     //probar ponerlo en los registros
    start = 0;
    option = 0;
    cell_max = 0;
    c_char = 0;
    c_disc = 0;
//    vmax = 0;
    cell_count = 49;
    dc_res_count = 0;
    LOG_OFF();    
}

void State_Machine()
{
    if (state == STANDBY)
    {
        STOP_CONVERTER();   
        Init_State_Machine();
        Define_Parameters();
        if (cell_max != 0x1B)
        {
            state = IDLE;
        }
    }else
    {
        //LI-ION CASE 
        Li_Ion_states_p1();
        Li_Ion_states_p2();
        //NI-MH CASE
        //Ni_MH_states_p1();
    }
}

void Start_State_Machine()
{   
    if(cell_count == 49)
    {
        UART_send_string(press_s_str);
        LINEBREAK;
        while(start == 0)
        {
            start = UART_get_char();                    //Get the value in the terminal.
            if(start == 115)                            //If value = "s" then...
            {
                UART_send_string(starting_str);        
                LINEBREAK;                                 
                LINEBREAK;    
            }else if(start == 0x1B)
            {
                state = STANDBY;
            }else
            {
                UART_send_string(press_s_str);
                LINEBREAK;
                start = 0; 
            }
        }
        if (start != 0x1B)
        {
            LINEBREAK;
            UART_send_string(cell1_str);
            LINEBREAK;           
        }

    }else if(cell_count == 50)
    {
        UART_send_string(starting_str);        
        LINEBREAK;                                 
        LINEBREAK; 
        LINEBREAK;
        UART_send_string(cell2_str);
        LINEBREAK;        
    }else if(cell_count == 51)
    {
        UART_send_string(starting_str);        
        LINEBREAK;                                 
        LINEBREAK; 
        LINEBREAK;
        UART_send_string(cell3_str);
        LINEBREAK;        
    }else if(cell_count == 52)
    {
        UART_send_string(starting_str);        
        LINEBREAK;                                 
        LINEBREAK; 
        LINEBREAK;
        UART_send_string(cell4_str);
        LINEBREAK;        
    }      
}




void Li_Ion_states_p1()
{
    //State machine for Li-Ion case
    if (state == IDLE)
    {
        LINEBREAK;
        UART_send_string(idle_str);
        LINEBREAK;
        Start_State_Machine();
        UART_interrupt_enable();     //I CHANGE THE POSITION AND IT WAS A MESS PLEASE THINK BEFORE DOING ANYTHING TO THIS    
        if (start != 0x1B)
        {
            count = COUNTER;
            if (option == 49)
            {
                /*state = DS_DC_res; 
                PARAM_DCRES();
                START_CONVERTER();*///PRUEBA PARA VER LA RESISTENCIA

               state = PRECHARGE;
               PARAM_CHAR();
               START_CONVERTER();
            }
            else if (option == 50 || option == 52)
            {
                state = DISCHARGE;
                PARAM_DISC();
                START_CONVERTER();
            }
            else if (option == 51)
            {
                LINEBREAK;
                UART_send_string(charge_str);
                LINEBREAK;
                state = CHARGE;
                PARAM_CHAR();
                START_CONVERTER();
            }else
            {
                STOP_CONVERTER();
            }                
        }
    }
//PRECHARGE state definition
    if (state == PRECHARGE)
    {   
        LOG_ON();
        control_loop();
        if (!count)
        {
            if (vprom < 900)
            {
                state = FAULT;
                LINEBREAK;
                UART_send_string(fault_str);
                LINEBREAK;
                UART_send_string(cell_below_str);
                LINEBREAK;
            }
            if (iprom < EOC_current)
            {                
                if (!EOCD_count)
                {
                    LINEBREAK;
                    UART_send_string(next_state_str);
                    LINEBREAK;
                    state = WAIT;
                    previous_state = PRECHARGE;
                    wait_count = wait_time;
                    STOP_CONVERTER();                       
                }else EOCD_count--;
            }else state = PRECHARGE;
        }
    }
    if (state == DISCHARGE)
    {
        LOG_ON();
        control_loop();
        if (!count)
        {
            if (vprom < EOD_voltage && option == 49)
            {
                if (!EOCD_count)
                { 
                    LINEBREAK;
                    UART_send_string(next_state_str);
                    LINEBREAK;
                    state = WAIT;
                    previous_state = DISCHARGE;
                    wait_count = wait_time;
                    STOP_CONVERTER();
                }else EOCD_count--;
            }else if (vprom < EOD_voltage && option == 50)
            {
                if (!EOCD_count)
                {                 
                    LINEBREAK;
                    UART_send_string(next_state_str);
                    LINEBREAK;
                    state = WAIT;
                    previous_state = DISCHARGE;
                    wait_count = wait_time;
                    STOP_CONVERTER();
                }else EOCD_count--;
            }else if (vprom < EOD_voltage && option == 52)
            {
                state = ISDONE;
                LINEBREAK;
                UART_send_string(done_str);
                LINEBREAK;
                STOP_CONVERTER();            
            }
        }
    }

    if (state == CHARGE)
    {   
        LOG_ON();
        control_loop();
        if (!count)
        {
            if (vprom < 900)
            {
                state = FAULT;
                LINEBREAK;
                UART_send_string(fault_str);
                LINEBREAK;
                UART_send_string(cell_below_str);
                LINEBREAK;
            }           
            if (iprom < EOC_current && option == 49)
            {
                if (!EOCD_count)
                {
                    LINEBREAK;
                    UART_send_string(next_state_str);
                    LINEBREAK;
                    state = WAIT;
                    previous_state = CHARGE; //Esto lo cambie y estaba enredado
                    wait_count = wait_time;
                    STOP_CONVERTER(); 
                }else EOCD_count--;
            }else if (iprom < EOC_current && option == 50)
            {
                if (!EOCD_count)
                {
                    LINEBREAK;
                    UART_send_string(next_state_str);
                    LINEBREAK;
                    state = WAIT;
                    previous_state = CHARGE; //Esto lo cambie y estaba enredado
                    wait_count = wait_time;
                    STOP_CONVERTER(); 
                }else EOCD_count--;
            }else if (iprom < EOC_current && option == 51)
            {
                LINEBREAK;
                UART_send_string(done_str);
                LINEBREAK;
                state = ISDONE;
                STOP_CONVERTER(); 
            }else state = CHARGE; 
        }
    }
}

void Li_Ion_states_p2()
{
    if(state == CS_DC_res)
    {
        control_loop();
        Calculate_DC_res();
    }

    if(state == DS_DC_res)
    {
        control_loop();
        Calculate_DC_res();
    }


    if (state == WAIT)
    {
        STOP_CONVERTER();  ///MAYBEOK
        if (!count)
        {
            if(!wait_count)
            {           
                if (previous_state == PRECHARGE) //The maximum charging time here is still missing, better to let it to the end
                {
                    
                    LINEBREAK;
                    UART_send_string(next_state_str);
                    LINEBREAK;
                    state = DISCHARGE;
                    PARAM_DISC();
                    START_CONVERTER();
                }else if (previous_state == DISCHARGE)
                {
                    LINEBREAK;
                    UART_send_string(next_state_str);
                    LINEBREAK;
                    state = DS_DC_res; 
                    PARAM_DCRES();
                    START_CONVERTER();
                }else if (previous_state == CHARGE)
                {
                    LINEBREAK;
                    UART_send_string(next_state_str);
                    LINEBREAK;
                    state = CS_DC_res; 
                    PARAM_DCRES();
                    START_CONVERTER();                                     
                }else if (previous_state == CS_DC_res)
                {
                    LINEBREAK;
                    UART_send_string(done_str);
                    LINEBREAK;
                    state = ISDONE;
                    STOP_CONVERTER();
                }else if (previous_state == DS_DC_res)
                {
                    LINEBREAK;
                    UART_send_string(next_state_str);
                    LINEBREAK;
                    state = CHARGE;
                    PARAM_CHAR();
                    START_CONVERTER();
                }
            }else state = WAIT;
        }
    }

    if (state == ISDONE)
    {
        if (cell_count < cell_max)
        {
            __delay_ms(500);
            LINEBREAK;
            UART_send_string(next_cell_str);
            LINEBREAK;            
            cell_count++;
            state = IDLE;   
        }else
        {
            state = STANDBY;
        }
    }

    if (state == FAULT)
    {
        STOP_CONVERTER();
        UART_send_string(standby_str);
        state = STANDBY;
    }
}

void Define_Parameters()
{
    c_char = 0;
    c_disc = 0;
    option = 0;
    cell_max = 0;
    LINEBREAK; 
    UART_send_string(param_def_str);
    LINEBREAK;
    //LI-ION CASE
        ///*
        UART_send_string(chem_def_liion);
        LINEBREAK;
        LINEBREAK;  
        Li_Ion_param();
        //*/
    //END OF LI-ION CASE
    //NI-MH CASE
        /*
        UART_send_string(chem_def_nimh);
        LINEBREAK;
        LINEBREAK;  
        Ni_MH_param();
        */
    //END OF NI-MH CASE
}

void Li_Ion_param ()
{         
    LINEBREAK;
    SET_VOLTAGE(Li_Ion_CV);
    UART_send_string(cv_val_str);
    display_value(Li_Ion_CV);
    UART_send_string(mv_str);
    LINEBREAK;
    capacity = Li_Ion_CAP;
    UART_send_string(nom_cap_str);
    display_value(capacity);
    UART_send_string(mAh_str);
    LINEBREAK;
    LINEBREAK;
    //-------CHARGE CURRENT
    UART_send_string(def_char_curr_str);
    LINEBREAK;
    UART_send_string(quarter_c_str);
    LINEBREAK;
    UART_send_string(half_c_str);
    LINEBREAK;
    UART_send_string(one_c_str);
    LINEBREAK;
    LINEBREAK;
    while(c_char == 0)                               //Wait until the value of c_char is defined by the user
    {
        c_char = UART_get_char();                    //Get the value in the terminal.
        if (c_char == 49)
        {
            i_char = capacity/4;
            UART_send_string(char_def_quarter_str);
            LINEBREAK;
        }
        else if (c_char == 50)
        {
            i_char = capacity/2;
            UART_send_string(char_def_half_str);
            LINEBREAK;
        }
        else if (c_char == 51)
        {
            i_char = capacity;
            UART_send_string(char_def_one_str);
            LINEBREAK;
        }else if (c_char == 0x1B)
        {
            c_disc = 0x1B;
            option = 0x1B;
            cell_max = 0x1B;
            state = STANDBY;
            LINEBREAK;
            UART_send_string(restarting_str);
            LINEBREAK; 
        }else
        {
            c_char = 0;
            LINEBREAK;
            UART_send_string(num_1and3_str);
            LINEBREAK;
        }
    }
    if (c_char != 0x1B)     //Only show the message if the ESC was not pressed
    {
        EOC_current = Li_Ion_EOC_curr;
        UART_send_string(EOC_I_str);
        display_value(EOC_current);
        UART_send_string(mA_str);
        LINEBREAK; 
        LINEBREAK;
        //-------DISCHARGE CURRENT
        UART_send_string(def_disc_curr_str);
        LINEBREAK;
        UART_send_string(quarter_c_str);
        LINEBREAK;
        UART_send_string(half_c_str);
        LINEBREAK;
        UART_send_string(one_c_str);
        LINEBREAK;
        LINEBREAK;
    }
    while(c_disc == 0)
    {
        c_disc = UART_get_char();                    //Get the value in the terminal.
        if (c_disc == 49)
        {
            i_disc = capacity/4;
            UART_send_string(dis_def_quarter_str);
            LINEBREAK;
        }
        else if (c_disc == 50)
        {
            i_disc = capacity/2;
            UART_send_string(dis_def_half_str);
            LINEBREAK;
        }
        else if (c_disc == 51)
        {
            i_disc = capacity;
            UART_send_string(dis_def_one_str);
            LINEBREAK;
        }else if (c_disc == 0x1B)
        {
            option = 0x1B;
            cell_max = 0x1B;
            state = STANDBY;
            LINEBREAK;
            UART_send_string(restarting_str);
            LINEBREAK; 
        }else
        {
            c_disc = 0;
            LINEBREAK;
            UART_send_string(num_1and3_str);
            LINEBREAK;
        }
    }
    if (c_disc != 0x1B)
    {
        //-------EOD voltage
        EOD_voltage = Li_Ion_EOD_volt;
        UART_send_string(EOD_V_str);
        display_value(EOD_voltage);
        UART_send_string(mv_str);
        LINEBREAK;
        //-------Li-Ion case for options
        LINEBREAK;
        UART_send_string(cho_bet_str);
        LINEBREAK;
        UART_send_string(li_ion_op_1_str);
        LINEBREAK;
        UART_send_string(li_ion_op_2_str);
        LINEBREAK;
        UART_send_string(li_ion_op_3_str);
        LINEBREAK;
        UART_send_string(li_ion_op_4_str);
        LINEBREAK;
        LINEBREAK;
    }
    while(option == 0)
    {
        option = UART_get_char();                    //Get the value in the terminal.
        if (option == 49)
        {
            LINEBREAK;
            UART_send_string(li_ion_op_1_sel_str);
            LINEBREAK;
        }else if (option == 50)
        {
            LINEBREAK;
            UART_send_string(li_ion_op_2_sel_str);
            LINEBREAK;
        }else if (option == 51)
        {
            LINEBREAK;
            UART_send_string(li_ion_op_3_sel_str);
            LINEBREAK;
        }else if (option == 52)
        {
            LINEBREAK;
            UART_send_string(li_ion_op_4_sel_str);
            LINEBREAK;
        }else if (option == 0x1B)
        {
            cell_max = 0x1B;
            state = STANDBY;
            LINEBREAK;
            UART_send_string(restarting_str);
            LINEBREAK;
        }else
        {
            option = 0;
            LINEBREAK;
            UART_send_string(num_1and4_str);
            LINEBREAK;
        } 
    }                 
    if (option != 0x1B)
    {
        LINEBREAK;
        UART_send_string(def_num_cell_str);
        LINEBREAK;
    }
    while(cell_max == 0)
    {
        cell_max = UART_get_char();                 //Get the value in the terminal.
        if (cell_max == 49)
        {
            LINEBREAK;
            UART_send_string(num_cell_str);
            UART_send_string(one_str);
            LINEBREAK;
        }else if (cell_max == 50)
        {
            LINEBREAK;
            UART_send_string(num_cell_str);
            UART_send_string(two_str);
            LINEBREAK;
        }else if (cell_max == 51)
        {
            LINEBREAK;
            UART_send_string(num_cell_str);
            UART_send_string(three_str);
            LINEBREAK;
        }else if (cell_max == 52)
        {
            LINEBREAK;
            UART_send_string(num_cell_str);
            UART_send_string(four_str);
            LINEBREAK;
        }else if (cell_max == 0x1B)
        {
            state = STANDBY;
            LINEBREAK;
            UART_send_string(restarting_str);
            LINEBREAK;
        }else
        {
            cell_max = 0;
            LINEBREAK;
            UART_send_string(num_1and4_str);
            LINEBREAK;
        } 
    }
}

void Calculate_DC_res()
{
    //LOG_ON();  
    if(!count)
    {
        if (dc_res_count == 14)  // cambiar a constante
        {
            LINEBREAK;
            UART_send_string(state_res_str);
            display_value(state);
            UART_send_string(end_state_res_str);
            LINEBREAK;          
        } 
        if (dc_res_count == 4)
        {
            v_1_dcres = vprom;
            i_1_dcres = iprom;
            SET_CURRENT(capacity);                  
        }
        if (dc_res_count == 1)
        {
            v_2_dcres = vprom;
            i_2_dcres = iprom;
            STOP_CONVERTER();
            dc_res_val = (v_1_dcres - v_2_dcres) * 10000UL;
            dc_res_val = dc_res_val / (i_2_dcres - i_1_dcres);
            /*LINEBREAK;
            display_value(v_1_dcres);
            LINEBREAK;
            display_value(v_2_dcres);
            LINEBREAK;
            display_value(i_1_dcres);
            LINEBREAK;
            display_value(i_2_dcres);
            LINEBREAK;*/
            UART_send_string(DC_res_str);
            display_value(dc_res_val);
            UART_send_string(end_str);
            LINEBREAK; 
        }
        if (!dc_res_count)
        {                
            LINEBREAK;
            UART_send_string(next_state_str);
            LINEBREAK;
            if (state == DS_DC_res) previous_state = DS_DC_res;
            if (state == CS_DC_res) previous_state = CS_DC_res;
            state = WAIT;
            wait_count = wait_time;              
        }else dc_res_count--;
    }    
}