#include "taskFlyport.h"
#include "grovelib.h"
#include "rgb.h"
#include "pushingbox.h"

#define MAX_ANALOG_VALUE 1000 
// #define UART_DEBUG

void batery_life(void* rgb, int analVal);

BOOL flagBatteryLow = FALSE;
extern BOOL flagWebTest; //trigger by the Webserver

void FlyportTask()
{	
	#ifdef UART_DEBUG
	char str[20];				// String to write the potentiometer values
	#endif
	int val;
	int notifResult = 0;
	
	int i=0;
	BOOL flagTempoNotif = FALSE;
	
	vTaskDelay(20);
	UARTWrite(1,"Welcome to GROVE NEST example!\r\n");
	
	// Connection to Network
	#if defined (FLYPORT)
	WFConnect(WF_DEFAULT);
	while (WFStatus != CONNECTED);
	#endif
	#if defined (FLYPORTETH)
	while(!MACLinked);
	#endif
						
	// GROVE board
	void *board = new(GroveNest);
 
	// GROVE devices	
	void *myPot = new(An_i); //potentiometer			
	void *rgb = new(Rgb, 1); // RGB leds
	
	// Attach devices
	attachToBoard(board, myPot, AN1);
	attachToBoard(board, rgb, DIG1);
	
	// Initialize devices
	configure(rgb, 1, 0, 0, 0);
	set(rgb, ON);
 
	//	Main loop
	while(1)
	{
		val = get(myPot); // Reading potentiometer value
		#ifdef UART_DEBUG
		sprintf(str,"***\r\nValue: %d\r\n",val);
		UARTWrite(1, str);
		#endif
		batery_life(rgb,val);	//change color of RGB LED
		
		
		//if battery low, push a notification with PushingBox Web service
		if(flagBatteryLow && !flagTempoNotif)
		{
			//you need to change the parameters
			notifResult = sendNotification("v0123456789ABCDE"	,"&firstname=Antoine&lastname=Monmarche");
			if(!notifResult)
				UARTWrite(1,"Notif success!\r\n");
			else
				UARTWrite(1,"Notif error!\r\n");
				
			flagTempoNotif = TRUE;
			i=0;
		}
		//in the webserver there is a simple but which trigger the flaWebTest
		if(flagWebTest)
		{
			flagWebTest = FALSE;
			//you need to change the parameters
			notifResult = sendNotification("v0123456789ABCDE"	,"");
			if(!notifResult)
				UARTWrite(1,"Notif success!\r\n");
			else
				UARTWrite(1,"Notif error!\r\n");
		}
		
		vTaskDelay(100);
		
		if(flagTempoNotif)
			i++;
		if(i>=60)
			flagTempoNotif = FALSE;
		
		
	}
}


/*
function use to simulate the status of a battery with a RGB LED
A potentiometer will be used instaed of a real battery.
this are the steps for the color
battery 0% -> black (0,0,0)
battery 1%-19% -> red (255,0,0)
battery 20%-29% -> orange (255,128,0)
battery 30%-39% -> yellow (255,255,0)
battery >40% -> green (0,255,0)
*/
void batery_life(void* rgb, int analVal)
{
	if(analVal==0)
	{
		configure(rgb, 1, 0, 0, 0);
		set(rgb, ON);
	}else if((analVal >=1) && (analVal <= MAX_ANALOG_VALUE*0.2))
	{
		//need to notify battery low
		flagBatteryLow = TRUE;
		
		configure(rgb, 1, 255, 0, 0);
		set(rgb, ON);
	}else if((analVal >= MAX_ANALOG_VALUE*0.2) && (analVal <= MAX_ANALOG_VALUE*0.3) )
	{
		configure(rgb, 1, 255, 128, 0);
		set(rgb, ON);
	}	else if((analVal >= MAX_ANALOG_VALUE*0.3) && (analVal <= MAX_ANALOG_VALUE*0.4) )
	{
		configure(rgb, 1, 255, 255, 0);
		set(rgb, ON);
	}else if(analVal >= MAX_ANALOG_VALUE*0.4)
	{
		configure(rgb, 1, 0, 255, 0);
		set(rgb, ON);
	}
}
