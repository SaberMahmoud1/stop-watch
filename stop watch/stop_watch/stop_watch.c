#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
int tick_seconds=0;		//integer as it will reach maximum=356400
unsigned char minutes1=0,minutes2=0,hours1=0,hours2=0,seconds1=0,seconds2=0;
void timer1_clock()
{
	TCNT1=0; //timer one is initially zero
	OCR1A=15625;	//the compare value is 15625
	TIMSK|=(1<<OCIE1A);	//enable timer1A compare interrupt
	TCCR1A=(1<<FOC1A); 	//ENABLE NON PWM MODE
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS11); //ENABLE PRESCALER 64 AND CHOOSE MODE 4 (COMPARE_MODE)

}
ISR(TIMER1_COMPA_vect)
{
	tick_seconds++;
	sort();
}
void sort()		//to sort the timer variable to seconds and minutes and hours
{
	seconds1++;
	if(seconds1==10)
	{
		seconds2++;
		seconds1=0;
	}
	if(seconds2==6)
	{

		minutes1++;
		seconds2=0;
	}
	if(minutes1==10)
	{
		minutes2++;
		minutes1=0;
	}
	if(minutes2==6)
	{
		hours1++;
		minutes2=0;
	}
	if(hours1==10)
	{
		hours2++;
		hours1=0;
	}
	if(hours2==10)
	{

		tick_seconds=0;
		seconds1=0;seconds2=0;minutes1=0;minutes2=0;hours1=0;hours2=0;

	}
}
ISR(INT0_vect)			//INT0 will make every variable =0 then the stop watch is restarted
{
	TCNT1=0;
	tick_seconds=0;
	seconds1=0;seconds2=0;minutes1=0;minutes2=0;hours1=0;hours2=0;
}
ISR(INT1_vect)	//will turn off the clock so the timer will not increment the seconds variable and the stop watch stops
{
	TCCR1B&=~(1<<CS10);
	TCCR1B&=~(1<<CS11);
	TCCR1B&=~(1<<CS12);
}
ISR(INT2_vect)			//will start the clock again with PRESCALEER /64
{
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS11);
}
int main()
{
	DDRC|=0x0f;		//make PORTC output port
	PORTC&=0xf0;	//port c initially zero
	DDRA|=0x3f;		//first 6 pins in port A are output to control the 7_segment
	PORTA=1;		//initially only the first one 7_segment is on
	DDRD&=~(1<<PD2);	//make PD2 input pin for interrupt 0
	PORTD|=(1<<PD2);	//enable internal pull up resistor
	DDRD&=~(1<<PD3);	//make PD3 input pin for interrupt 1
	DDRB&=~(1<<PB2);		//make PB2 input pin for interrupt 2
	PORTB|=(1<<PB2);	//enable internal  pull up resistor
	MCUCR|=(ISC01)|(1<<ISC11)|(1<<ISC10);	//let INT0 and INT1 and INT2 detects the falling ,rising ,falling edges
	MCUCR&=~(1<<ISC00);
	MCUCSR&=~(1<<ISC2);
	GICR|=(1<<INT0)|(1<<INT1)|(1<<INT2);	//enable general interrupts 0 and 1 and 2
	SREG|=(1<<7);			//enable global interrupt
	timer1_clock();			//start the timer
	while(1)			//displays the time on the 7_segment
	{
		PORTA=(1<<PA0);
		PORTC=seconds1;
		_delay_us(5);
		PORTA=(1<<PA1);
		PORTC=seconds2;
		_delay_us(5);
		PORTA=(1<<PA2);
		PORTC=minutes1;
		_delay_us(5);
		PORTA=(1<<PA3);
		PORTC=minutes2;
		_delay_us(5);
		PORTA=(1<<PA4);
		PORTC=hours1;
		_delay_us(5);
		PORTA=(1<<PA5);
		PORTC=hours2;
		_delay_us(5);
	}

}
