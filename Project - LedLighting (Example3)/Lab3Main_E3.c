#include <stdint.h>
#include <stdlib.h>
//#include <tm4c123gh6pm.h> // I can use this library or i can write the definition of addresses :

#define SYSCTL_RCGC2_R					(*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGC2_GPIOF			0x00000020 	 // Port F Clock Gating Control

#define SYSCTL_PRGPIO_R         (*((volatile unsigned long *)0x400FEA08))

#define GPIO_PORTF_LOCK_R     	(*((volatile unsigned long *)0x40025520)) // The GPIOLOCK register enables write access to the GPIOCR register  (datasheet on 684) offset 0x520
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524)) // commit register (datasheet on 685) offset 0x524 

#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
 
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510)) // pull-up (datasheet on 677) offset 0x510 

#define GPIO_PORTF_DATA_R				(*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R				(*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_DEN_R				(*((volatile unsigned long *)0x4002551C))
	
#define PF1                     (*((volatile unsigned long *)0x40025008))  // pin specified address (RED LED)
#define PF4											(*((volatile unsigned long *)0x40025040))  // pin specified address  (USER SWITCH1)

void delay(int sec);
void initialize_PortF(void);
unsigned long readSwitch1();

int thePartOneOfDutyCycle; //yüzdelik dilim
int thePartTwoOfDutyCycle;
int dutyCycle;

int main(void)
{
	initialize_PortF();
	
	thePartOneOfDutyCycle=1;
	thePartTwoOfDutyCycle=99;
	dutyCycle = 125;
	
	while(1)
	{
		if(!(readSwitch1())) //active low switch , it's mean is that switch is cconnected to groud with pull up res.
		{
				PF1 |= 0x02; // red (pf1)
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				delay(dutyCycle*20);
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				PF1 ^= 0x02; // toggle
				delay(dutyCycle*80);			
		}
		
		else
		{
			for(;thePartOneOfDutyCycle<=99;thePartOneOfDutyCycle++)
			{
				thePartTwoOfDutyCycle--;
				PF1 |= 0x02; // red (pf1)
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				delay(dutyCycle*thePartOneOfDutyCycle);
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				PF1 ^= 0x02; // toggle
				delay(dutyCycle*thePartTwoOfDutyCycle);
			}
			for(;thePartTwoOfDutyCycle<=99;thePartTwoOfDutyCycle++)
			{
				thePartOneOfDutyCycle--;
				PF1 |= 0x02; // red (pf1)
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				delay(dutyCycle*thePartOneOfDutyCycle);
				// T=1/f , so 1/8 second = 8 Hz , 1/8 = 0.125 s = 125 ms
				PF1 ^= 0x02; // toggle
				delay(dutyCycle*thePartTwoOfDutyCycle);
			}				
		}
	}
	return 0;
}

void initialize_PortF(void)
{
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;
	while(!( SYSCTL_PRGPIO_R& 0x20)){ } //waiting to enable clk
	
	GPIO_PORTF_LOCK_R  = 0x4C4F434B;   // unlock GPIO Port F
  GPIO_PORTF_CR_R    = 0x1F;           // allow changes to PF4-0
																		// only PF0 needs to be unlocked, other bits can't be locked
	GPIO_PORTF_DIR_R   = 0x0E; //01110 IOOOI
  GPIO_PORTF_PUR_R   = 0x11;          // enable pull-up on PF0 and PF4
	GPIO_PORTF_DEN_R   = 0x1F; //0011111 PE0 (SW2) , PE1 , PE2 , PE3 , PE4 (SW1) ACTIVE
		
	GPIO_PORTF_DATA_R  = 0x00; // reset to initialize all pins
		
	PF1=0x00; //specified address
}

//@Def : to read User User_Switch1
//gives value when switch1 is pressed
unsigned long readSwitch1()
{
	return (PF4&0x10); // 0 b 0001 0000 
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
