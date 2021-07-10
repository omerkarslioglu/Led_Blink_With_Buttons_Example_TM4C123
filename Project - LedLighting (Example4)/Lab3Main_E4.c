#include <stdint.h>
#include <stdlib.h>
//#include <tm4c123gh6pm.h> // I can use this library or i can write the definition of addresses :

#define SYSCTL_RCGC2_R					(*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGC2_GPIOE			0x00000010 	 // Port E Clock Gating Control

#define SYSCTL_PRGPIO_R         (*((volatile unsigned long *)0x400FEA08))

#define GPIO_PORTE_LOCK_R     	(*((volatile unsigned long *)0x40024520)) // The GPIOLOCK register enables write access to the GPIOCR register  (datasheet on 684) offset 0x520
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524)) // commit register (datasheet on 685) offset 0x524 

#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
 
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510)) // pull-up (datasheet on 677) offset 0x510 

#define GPIO_PORTE_DATA_R				(*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R				(*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_DEN_R				(*((volatile unsigned long *)0x4002451C))
	
#define PE_1_2_3                (*((volatile unsigned long *)0x40024038))  // pin specified address (LEDS)
#define PE4											(*((volatile unsigned long *)0x40024040))  // pin specified address  (SWITCH)

void delay(int sec);
void initialize_PortE(void);
unsigned long readSwitch1();

int thePartOneOfDutyCycle; //yüzdelik dilim
int thePartTwoOfDutyCycle;
int dutyCycle;

int main(void)
{
	initialize_PortE();
	
	thePartOneOfDutyCycle=1;
	thePartTwoOfDutyCycle=99;
	dutyCycle = 125;
	
	while(1)
	{
		if(!(readSwitch1())) //active low switch , it's mean is that switch is cconnected to groud with pull up res.
		{
				PE_1_2_3 |= 0x0F; // LIGHT LED
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				delay(dutyCycle*20);
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				PE_1_2_3 ^= 0x0F; // toggle
				delay(dutyCycle*80);			
		}
		
		else
		{
			for(;thePartOneOfDutyCycle<=99;thePartOneOfDutyCycle++)
			{
				thePartTwoOfDutyCycle--;
				PE_1_2_3 |= 0x0F; // LIGHT LED
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				delay(dutyCycle*thePartOneOfDutyCycle);
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				PE_1_2_3 ^= 0x0F; // toggle
				delay(dutyCycle*thePartTwoOfDutyCycle);
			}
			for(;thePartTwoOfDutyCycle<=99;thePartTwoOfDutyCycle++)
			{
				thePartOneOfDutyCycle--;
				PE_1_2_3 |= 0x0F; // LIGHT LED
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				delay(dutyCycle*thePartOneOfDutyCycle);
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				PE_1_2_3 ^= 0x0F; // toggle
				delay(dutyCycle*thePartTwoOfDutyCycle);
			}				
		}
	}
	return 0;
}

void initialize_PortE(void)
{
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOE;
	while(!( SYSCTL_PRGPIO_R& 0x10)){ } //waiting to enable clk
	
	GPIO_PORTE_LOCK_R  = 0x4C4F434B;   // unlock GPIO Port E
  GPIO_PORTE_CR_R    = 0x1F;           // allow changes to PE-0
																		// only PF0 needs to be unlocked, other bits can't be locked
	GPIO_PORTE_DIR_R   = 0x0F; //01111 IOOOO
  //GPIO_PORTE_PUR_R   = 0x00;          // NOT enable pull-up on PF0 and PF4 (RXTERNAL PULL UP)
	GPIO_PORTE_DEN_R   = 0x1F; //0011111  PE1 , PE2 , PE3 , PE4  ACTIVE
		
	//GPIO_PORTE_DATA_R  = 0x00; // reset to initialize all pins
		
	PE_1_2_3=0x00; //specified address
}

//@Def : to read User User_Switch1
//gives value when switch1 is pressed
unsigned long readSwitch1()
{
	return (PE4&0x10); // 0 b 0001 0000 
}

//Our mcu is working on 80 MHz
// Specified Delay Function
// @Def : 
// 1 ms delay function nearly
// if sec equals to 10 this means 1 duty cycle 
void delay(int sec)
{
	int x=80; //x=(8000);// I delete two zeros for specify process
	while(sec>0)
	{
		sec--;
		while(x>0)
		{
			x--;
		}
	}
}
