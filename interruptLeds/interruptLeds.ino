// Entrar no site https://www.electronicwings.com/avr-atmega/atmega1632-timer

#include <inttypes.h>			//tipos inteiros
#include <avr/io.h>             // E/S padrão
#include <avr/interrupt.h>    	//controle das interrrupções
#include <util/delay.h>  	

// define os pinos dos leds que irá ser utilizado
#define LED (7)
#define LED_2 (3)
#define LED_3 (4)
#define LED_4 (5)

#define F_CPU		16000000UL    	// frequencia da CPU em Hz
#define TIMER_CLOCK       2     	// frequencia do LED em 1Hz
#define TIMER_CLOCK_2	 100		// frequencia interrupt2 em 100Hz
#define F_LED_2 		  1			// frequencia do led no interrupt 2

#define tbi(x,y) x ^= _BV(y) 

static volatile uint8_t led;     	// o termo 'volatile' é usado quando a
static volatile uint8_t count = 0;	// variável é acessada em interrupções

ISR(TIMER1_COMPA_vect)   			// chamda quando OC1 gerar 'overflow interrupt'
{
	PORTB = led;
	led <<= 1;                   	// move para o proximo LED
	if (!led)                    	// overflow: começa no bit 0 de novo
	led = 1;
}

ISR(TIMER2_COMPA_vect)    			// chamada quando OC2 gerar 'overflow interrupt'
{
	if (count == TIMER_CLOCK_2/F_LED_2){			// verifica se o count é igual a freq
		PORTC |= (1<<LED_3);   						// NIVEL LOGICO 1
		PORTC &= ~(1<<LED_4);						// NIVEL LOGICO 0
	}	
	else if ( count >= TIMER_CLOCK_2*2/F_LED_2) {
		PORTC &= ~(1<<LED_3);  						// NIVEL LOGICO 0
		PORTC |= (1<<LED_4); 						// NIVEL LOGICO 1
		count = 0;									// reinicia o count
	}
	
	
	// para mostrar a frequencia de saida que a interrupt está gerando
	if (count%2 == 0)
		PORTC |= (1<<LED_2);   // NIVEL LOGICO 1
	else 
		PORTC &= ~(1<<LED_2);  // NIVEL LOGICO 0
		
	count++;
}

int main(void)
{
    DDRD  = 0xff;				// todos os pinos da porta D como OUTPUT
	PORTD = 0x00;				// inicia os leds da porta D desligados
	
	DDRB  = 0xff;               // todos os pinos da porta B como OUTPUT
	PORTB = 0x00;  				// inicia os leds da porta B desligados
	  
	DDRC  = 0xff;               // todos os pinos da porta C como OUTPUT
	PORTC = 0x00;   			// inicia os leds da porta C desligados
	// desliga todos os LEDs
	
	TCCR1B = _BV(CS10) | _BV(CS11)  | _BV(WGM12); // prescaler=64, clear timer/counter e compareA match
	OCR1A = ((F_CPU/2/64/TIMER_CLOCK) - 1 );
	// habilita a saida de OC1 (Output Compare 1) para 'overflow interrupt'
	
	TCCR2A = _BV(WGM21);						// clear timer/counter e compareA match
	TCCR2B = _BV(CS22) | _BV(CS20) | _BV(CS21); // prescaler=1024
	OCR2A = ((F_CPU/2/1024/TIMER_CLOCK_2) - 1 );
	// habilita a saida de OC2 (Output Compare 2) para 'overflow interrupt'
	
	TIMSK1 = _BV(OCIE1A);
	TIMSK2 = _BV(OCIE2A);
	
	led = 1;                    // estado inicial da variavel LED
	count = 0;					// inicia o count em 0
	
	sei();                      // habilita as interrupções
	
	unsigned int i;
	while(1==1) // loop Infinito
	{
		PORTD |= (1<<LED);   	// NIVEL LOGICO 1
		_delay_ms(500);         // delay 500 ms
		PORTD &= ~(1<<LED);  	// NIVEL LOGICO 0
		_delay_ms(500);			// delay 500 ms
	}
}
