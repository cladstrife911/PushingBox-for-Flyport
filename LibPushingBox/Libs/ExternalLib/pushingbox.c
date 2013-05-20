/* **************************************************************************
 *																					
 *  FileName:        pushingbox.c
 *  Module:          FlyPort WI-FI - FlyPort ETH
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Antoine Monmarché	     1.0     2013/05/15		   First release  
 *  antoine@giga-concept.fr
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by 
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *  
 *  ***NOTE*** The exception to the GPL is included to allow you to distribute
 *  a combined work that includes OpenPicus code without being obliged to 
 *  provide the source code for proprietary components outside of the OpenPicus
 *  code. 
 *  OpenPicus software is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details. 
 * 
 * 
 * Warranty
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 **************************************************************************/
 
#include "pushingbox.h"

#define UART_DEBUG


/*
exemple: "http://api.pushingbox.com/pushingbox?devid=v0123456789ABCDE&temperature=23"

devid is the ID of the pushingbox scenario. must be 16 bytes long
params is for optional parameters, exemple: "&temperature=19.5", don't forget the first '&'
return 0 - OK
*/
int sendNotification(char* devid, char* params)
{
	char* serverName= "api.pushingbox.com";
	char* destPort="80";
	char data[128]="/pushingbox?devid=";
	
	int result=0;
	
	if(strlen(devid)==16)
	{
		strcat(data,devid);
		if(strlen(params)!=0)
			strcat(data,params);
		
		result = sendHTTPGETRequest(serverName, destPort, data);
		if(result != 0)
			return -1;
	}else
	{
		UARTWrite(1,"Error in parameters\r\n\"");
		return -1;
	}
	
	return 0;
}


/**
* this function sends a GET request to a server
* http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
* return 0 - OK
*/
int sendHTTPGETRequest(char* ipDestination, char* portDestination, char* data)
{
	int count = 0;
	
	char REQ[512]="GET ";
	strcat(REQ,data);
	strcat(REQ," HTTP/1.1\r\nHost: ");
	strcat(REQ,ipDestination);	//destination IP
	strcat(REQ,"\r\nUser-Agent: Flyport");	//destination IP
	strcat(REQ,"\r\n\r\n"); //do not fogert the \r\n or it will not work
	
	#ifdef UART_DEBUG
	UARTWrite(1,"Request to send:\r\n\"");
	vTaskDelay(5);
	UARTWrite(1,REQ);
	vTaskDelay(5);
	UARTWrite(1,"\"\r\n");
	#endif
	
	TCP_SOCKET client_sock;
	client_sock = TCPClientOpen(ipDestination,portDestination);
	vTaskDelay(5);
	
	while ( (!TCPisConn(client_sock)) && (count < 20) )
	{
		vTaskDelay(5);
		count++;
	}
	if (TCPisConn(client_sock))
	{
		UARTWrite(1, "socket connected\r\n");
		TCPWrite ( client_sock, REQ, strlen(REQ));
		vTaskDelay(15);
	}
	else
	{
		UARTWrite(1,"Connection timed out!\r\n");
		return -1;
	}
	TCPClientClose(client_sock);
	
	return 0;
} 
