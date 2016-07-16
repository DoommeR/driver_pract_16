#include "hx.h"

#define PIN_SCK RPI_V2_GPIO_P1_16
#define PIN_DAT RPI_V2_GPIO_P1_18

unsigned long x;
float x1=8433477.000000,x2=8364942.000000;
float y1=0.000000, y2=8.000000;
float weight;
float a[2];
float b[2];

unsigned long HX711_Read()
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

	bcm2835_close();
	//printf("count=%d",(int)count);
	return(count);
}

void Callibration(int shelf_number){


    int Kg=0; //kilogramm
   	int Gr=0; //gramm

	printf("please, empty shelf");
    printf("When ready press enter");
 	//пока не нажат enter ждем....

    x1 = HX711_Read();
 	printf("Put something and write it's Weight in Kg");
   
    //kg= тому что ввели с клавы
    printf("Now write Weight in gramm");
    //gr= тому что ввели с клавы
   
    x2 = HX711_Read();
    y2 = (float)Kg+(float)Gr/1000;

    printf("Calibrated!");
      b[shelf_number] = (y2*x1-y1*x2)/(x1-x2);
      a[shelf_number] = (y2*1000000 - b[shelf_number]*1000000)/x2;
}


int main(){
	int i;

	//тупо забиваем фигней переменные калибровки
	for (i=0; i<3; i++){
	 	b[i] = (y2*x1-y1*x2)/(x1-x2);
	  	a[i] = (y2*1000000 - b[i]*1000000)/x2;
  	}

  	while(1){
  		//вычисляем вес на каждую полку по индивидуальным значениям a и b
  		for (i=0; i<3; i++){
  			x=HX711_Read(); // чуть позже я добавлю чтение конкретной полки. 
  			weight=a[i]*(float)x/1000+b[i]*1000;
  			printf("Shelf_%d = %d \n",i, (int)weight);
  		}
	
		printf("weight= %d\n",(int)weight);
	}

	return 0;
}
