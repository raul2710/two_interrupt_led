// Entrar no site https://www.electronicwings.com/avr-atmega/atmega1632-timer

#include <inttypes.h>			//tipos inteiros
#include <avr/io.h>             // E/S padrão
#include <avr/interrupt.h>    //controle das interrrupções
#include <util/delay.h>  	

#define LED (7)

#define F_CPU		16000000UL    // frequencia da CPU em Hz
#define TIMER_CLOCK       2     // frequencia do LED em 1Hz
#define tbi(x,y) x ^= _BV(y) 

static volatile uint8_t led;     // o termo 'volatile' é usado quando a
static volatile uint8_t led2;    // variável é acessada em interrupções

ISR(TIMER1_COMPA_vect)    // chamda quando OC1 gerar 'overflow interrupt'
{
	PORTB = led;
	led <<= 1;                   // move para o proximo LED
	if (!led)                    // overflow: começa no bit 0 de novo
	led = 1;
}

ISR(TIMER2_COMPA_vect)    // chamda quando OC2 gerar 'overflow interrupt'
{
	PORTC = led2;
	led2 <<= 1;                   // move para o proximo LED
	if (!led2)                    // overflow: começa no bit 0 de novo
	led2 = 1;
}


int main(void)
{
    DDRD = 0xff;
	PORTD = 0x00;
	
	DDRB  = 0xff;                // todos os pinos da porta B como OUTPUT
	PORTB = 0x00;  
	  
	DDRC  = 0xff;                // todos os pinos da porta B como OUTPUT
	PORTC = 0x00;   
	            // desliga todos os LEDs
	TCCR1B = _BV(CS10) | _BV(CS11)  | _BV(WGM12); // prescaler=64, clear timer/counter e compareA match
	OCR1A = ((F_CPU/2/64/TIMER_CLOCK) - 1 );
	// habilita a saida de OC1 (Output Compare 1) para 'overflow interrupt'
	TCCR2B = _BV(CS22) | _BV(CS20) | _BV(CS21)  | _BV(WGM12); // prescaler=64, clear timer/counter e compareA match
	OCR2A = ((F_CPU/2/1024/TIMER_CLOCK) - 1 );
	
	TIMSK1  = _BV(OCIE1A);
	TIMSK2  = _BV(OCIE2A);
	
	led = 1;                     // estado inicial da variavel LED
	led2 = 1;
	
	sei();                       // habilita as interrupções
	
	unsigned int i;
	while(1==1) // loop Infinito
	{
		PORTD |= (1<<LED);   // NIVEL LOGICO 1
		_delay_ms(500);         // delay 100 ms
		PORTD &= ~(1<<LED);  // NIVEL LOGICO 0
		_delay_ms(500);
	}
}
