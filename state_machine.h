/* Hardware related definitions for Charge and Discharge System. */

/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  state_machine.h 
 * Author: Juan J. Rojas.
 * Comments: Release
 * Date: 06/14/17. Version: Git
 */

#ifndef STATE_MACHINE_H
    #define STATE_MACHINE_H
	enum states { STANDBY = 0, IDLE = 1, FAULT = 2, ISDONE = 3, WAIT = 4, PRECHARGE = 5, DISCHARGE = 6, CHARGE = 7, POSTDISCHARGE = 8, DS_DC_res = 9, CS_DC_res = 10};
    
    extern unsigned char                start; //variable to receive the start command
    //extern unsigned char                chem_message;
    extern unsigned char                chem;
    extern unsigned char                c_char;
    extern unsigned char                c_disc;
    extern unsigned char                option;
    extern unsigned int                 capacity; 
    extern unsigned int                 i_char;
    extern unsigned int                 i_disc;
    //extern unsigned int                 c_1_dcres;                 
    //extern unsigned int                 c_2_dcres;
    extern unsigned char                cell_count;
    extern unsigned int                 EOC_count;                             
    //extern unsigned int                 ocv_count;
    extern unsigned int                 wait_count;
    //extern unsigned int                 small_count;               
    extern unsigned int                 dc_res_count;             
    extern unsigned char                state;
    extern unsigned char                previous_state;
    //extern unsigned int                 state_counter;
    extern unsigned int                 EOC_current;
    extern unsigned int                 EOD_voltage;
    //extern double                       vmax;
    //extern double                       In_ocv;
    //extern double                       Pre_ocv;
    
    void Init_State_Machine(void);
    void Start_State_Machine(void);
    void Parameter_Set(void);
    void State_Machine(void);
    void Li_Ion_states_p1(void);
    void Li_Ion_states_p2(void);
    void option_50(void);
    void Define_Parameters(void);
    void Li_Ion_param(void);
    void Ni_MH_param(void);
    void Calculate_DC_res(void);


   
    //General definitions
    #define OCV_time            976
    #define wait_time           600

    //Li-Ion definitions
    #define Li_Ion_CV           4200
    #define Li_Ion_CAP          3250
    #define Li_Ion_EOC_curr     100
    #define Li_Ion_EOD_volt     3000

    //Ni-MH definitions
    #define Ni_MH_CAP           2500
    #define Ni_MH_EOC_DV        10
    #define Ni_MH_EOD_volt      1000
    
#endif /* STATE_MACHINE_H*/


