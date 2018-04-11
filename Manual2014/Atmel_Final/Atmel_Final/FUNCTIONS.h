uint8_t ReversePWMForPP = 40, i = 0;
int MaxonSpeed = 550;

//-----------------INITIALISATION-------------------
void pwm_init(void)
{
	TCCR4A = 0xA8;
	TCCR4B = 0x11;
	ICR4 = 737;
	TCCR5A = 0xA8;
	TCCR5B = 0x11;
	ICR5 = 737;
}

void port_init(void)
{
	DDRJ |= (1 << PJ0) | (1 << PJ2) | (1 << PJ1) | (1 << PJ3);
	DDRE |= (1 << PE3) | (1 << PE4) | (1 << PE5);
	DDRL |= (1 << PL0) | (1 << PL3) |(1 << PL4)| (1 << PL1) | (1<<PL5);
	DDRH |= (1 << PH4) | (1 << PH3) | (1 << PH5) | (1 << PH6);
	DDRG |= (1 << PG4) | (1<<PG2);
	DDRA = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF;
	PORTC = 0xFF;
	CLEARBIT(PORTA, BIT(0));
	CLEARBIT(PORTE,BIT(5));		//pump

	CLEARBIT(DDRF,BIT(0));
	SETBIT(PORTF,BIT(0));
}
//-------------------------------------------------------------


//--------------------UART----------------------------
void UartInit(void)
{
	DDRH |= (1<<PH1);
	DDRH &= ~(1<<PH0);
	
	PORTH |= (1<<PH1);
	PORTH &= ~(1<<PH0);
	
	UCSR2B |= (1<<RXEN2) | (1<<TXEN2);
	UCSR2C |= (1<<UCSZ20) | (1<<UCSZ21);
	UCSR2A |= (1<<U2X2);
	
	UBRR2 = 191;
	// RX - PJ0
	// TX - PJ1
	
	// Using Channel 2 of 640 Present at PORTH
}

void UartTransmit(uint8_t data)
{
	while ( !( UCSR2A & (1<<UDRE2)) );
	UDR2 = data;
}
uint8_t UartReceive(void)
{
	while ( !( UCSR2A & (1<<RXC2)) );
	return UDR2;
}
//---------------------------------------------------


//------------------ADC-----------------------------
void ADCInit(void)
{
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	ADMUX |= (1<<ADLAR);
	ADCSRA |= (1<<ADSC);
}

int ADCConvert(void)
{
	ADCSRA |= (1<<ADSC);
	
	while( !(ADCSRA & (1<<ADIF)));
	return ADCH;
}
//--------------------------------------------------

//------------MAXON FUNCTIONS----------------------------------
void lft_fwd(void)
{
	PORTJ |= (1 << PJ1);
	PORTJ &= ~(1 << PJ0);
	OCR5A = MaxonSpeed;
}

void lft_rev(void)
{
	PORTJ |= (1 << PJ0);
	PORTJ &= ~(1 << PJ1);
	OCR5A = MaxonSpeed;
}

void rgt_fwd(void)
{
	PORTJ |= (1 << PJ2);
	PORTJ &= ~(1 << PJ3);
	OCR5B = MaxonSpeed;
}

void rgt_rev(void)
{
	PORTJ |= (1 << PJ3);
	PORTJ &= ~(1 << PJ2);
	OCR5B = MaxonSpeed;
}


void rgt_stop(void)
{
	OCR5B = 100;
	TOGGLEBIT(PORTJ, BIT(2));
	TOGGLEBIT(PORTJ, BIT(3));
}

void lft_stop(void)
{
	OCR5A = 100;
	TOGGLEBIT(PORTJ, BIT(0));
	TOGGLEBIT(PORTJ, BIT(1));
}
//---------------------------------------------------


//-------------PARALLELOGRAM FUMCTIONS--------------
void ppup(void)
{
	CLEARBIT(PORTH, BIT(7));			
	OCR5C = 737;
	SETBIT(PORTE, BIT(4));				//Break bit
}

void ppdown(void)
{
	SETBIT(PORTH, BIT(7));			
	OCR5C = 737;
	SETBIT(PORTE, BIT(4));				//Break bit
}

void ppstop(void)
{
	CLEARBIT(PORTH, BIT(7));		
	OCR5C = ReversePWMForPP;
	SETBIT(PORTE, BIT(4));			//Break bit
}
//--------------------------------------------------


void seesaw(void)
{
	SETBIT(PORTA,BIT(5));		//seesaw
	SETBIT(PORTA,BIT(4));		//succup	
	_delay_ms(100);
	SETBIT(PORTE,BIT(5));	//pump
	_delay_ms(100);
	for(i = 0; i < 3; i++)
	{
		SETBIT(PORTA,BIT(5));		//seesaw 
		_delay_ms(2000);
		CLEARBIT(PORTA,BIT(5));
		_delay_ms(1000);
	}
	CLEARBIT(PORTE,BIT(5));		//pump
	_delay_ms(100);
	CLEARBIT(PORTA,BIT(4));
	CLEARBIT(PORTA,BIT(5));		
}