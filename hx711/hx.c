#include <bcm2835.h>
#include <stdio.h>


#define PIN_SCK RPI_V2_GPIO_P1_16
#define PIN_DAT RPI_V2_GPIO_P1_18

int main()
{	
	unsigned long count=0;
	unsigned char i;

	if (!bcm2835_init()){
		printf("init err");
		return 1;
	}

	bcm2835_gpio_fsel(PIN_SCK, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_DAT, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(PIN_DAT, BCM2835_GPIO_PUD_UP);
	
	bcm2835_delayMicroseconds(1);
	bcm2835_gpio_write(PIN_SCK, HIGH);
	bcm2835_delayMicroseconds(1);
	
	while (bcm2835_gpio_lev(PIN_DAT))
		for(i=0; i<24; i++){
			bcm2835_gpio_write(PIN_SCK, HIGH);
			bcm2835_delayMicroseconds(1);
			count=count<<1;
			bcm2835_gpio_write(PIN_SCK, LOW);
			bcm2835_delayMicroseconds(1);
			
			if(bcm2835_gpio_lev(PIN_DAT)) count++;			
		}
	

	bcm2835_gpio_write(PIN_SCK, HIGH);
	count^=0x800000;
	bcm2835_delayMicroseconds(1);
	bcm2835_gpio_write(PIN_SCK, LOW);
	bcm2835_delayMicroseconds(1);

	printf("count=%d",(int)count);
	
	bcm2835_close();
	return 0;
}