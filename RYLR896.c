#include "RYLR896.h"
#include "string.h"
#include "STR_Chelo.h"

int LoRa_Send(struct LoRa *a, UART_HandleTypeDef * SERIAL)
{

	char items[4];

	if(a->txitems > 241)
	{
		return ERROR_BUFFER_SIZE;
	}
	else
	{

		items[0]='\0';
		itoa(a->txitems, items,10);
		a->txbuff2SND[0]='\0'; 													//Borro el vector
		strncat(a->txbuff2SND,"AT+SEND=",strlen("AT+SEND="));					//AT+SEND=
		strncat(a->txbuff2SND,a->dest_address,strlen(a->dest_address));			//AT+SEND=1
		strncat(a->txbuff2SND,",",1);											//AT+SEND=1,
		strncat(a->txbuff2SND,items,strlen(items));								//AT+SEND=1,4
		strncat(a->txbuff2SND,",",1);											//AT+SEND=1,4,

		int la=strlen(a->txbuff2SND);
		int lb=la+a->txitems;
		int lc=0;
		while(la < lb )
		{
			a->txbuff2SND[la]=a->txbuff[lc];
			la++;
			lc++;
		}
		a->txbuff2SND[la++]='\r';
		a->txbuff2SND[la++]='\n';
		HAL_UART_Transmit_IT(SERIAL, a->txbuff2SND, la);
	}

}

void LoRa_set_sndTIMER(struct LoRa *a, int msec)
 {
	 	a->tmr_dly_ON=0;
		a->tmr_dly=msec;
		a->tmr_dly_en=1;
		a->tmr_dlyCNT=0;
 }

void LoRa_reset_sndTIMER(struct LoRa *a,int msec)
 {
	 	a->tmr_dly_ON=0;
		a->tmr_dly=msec;
		a->tmr_dly_en=0;
		a->tmr_dlyCNT=0;
 }
void Lora_snd_CMND(struct LoRa *a,UART_HandleTypeDef * SERIAL, uint8_t cmd)
{
	switch(cmd)
	{
		case CMD_AT:
			{
				a->estado=_AT_SENT;
				a->txbuff2SND[0]='\0'; 													//Borro el vector
				strncat(a->txbuff2SND,"AT",2);											
				a->txbuff2SND[2]='\r';
				a->txbuff2SND[3]='\n';
				HAL_UART_Transmit_IT(SERIAL, a->txbuff2SND, 4);
			}
		break;
		case CMD_RESET:
			{
				a->estado=_RESET_SENT;
				a->txbuff2SND[0]='\0'; 													//Borro el vector
				strncat(a->txbuff2SND,"AT+RESET",8);											
				a->txbuff2SND[8]='\r';
				a->txbuff2SND[9]='\n';
				HAL_UART_Transmit_IT(SERIAL, a->txbuff2SND, 9);
			}
		break;
		case CMD_FACTORY:
			{
				a->estado=_FACTORY_SENT;
				a->txbuff2SND[0]='\0'; 													//Borro el vector
				strncat(a->txbuff2SND,"AT+FACTORY",10);											
				a->txbuff2SND[10]='\r';
				a->txbuff2SND[11]='\n';
				HAL_UART_Transmit_IT(SERIAL, a->txbuff2SND, 11);
			}
		break;
		case CMD_UID:
			{
				a->estado=_UID_SENT;
				a->txbuff2SND[0]='\0'; 													//Borro el vector
				strncat(a->txbuff2SND,"AT+UID",6);											
				a->txbuff2SND[6]='\r';
				a->txbuff2SND[7]='\n';
				HAL_UART_Transmit_IT(SERIAL, a->txbuff2SND, 7);
			}
		break;
		case CMD_VER:
			{
				a->estado=_VER_SENT;
				a->txbuff2SND[0]='\0'; 													//Borro el vector
				strncat(a->txbuff2SND,"AT+VER",6);											
				a->txbuff2SND[6]='\r';
				a->txbuff2SND[7]='\n';
				HAL_UART_Transmit_IT(SERIAL, a->txbuff2SND, 7);
			}
		break;
		default:
		{
			
		}
		break;
	}
}

void LoRa_decode(struct LoRa *b)
 {
	switch (Lora_idSTR(b))
	{
		case _OK:
		{
			switch(b->estado)
			{
				case _AT_SENT:
					b->estado=_IDLE;
					break;
				case _MODE_SENT:
					b->estado=_MODE_OK;
					break;
				case _PARAMETER_SENT:
					b->estado=_PARAMETER_OK;
					break;
				case _BAND_SENT:
					b->estado=_BAND_OK;
					break;
				case _ADDR_SENT:
					b->estado=_ADDR_OK;
					break;
				case _NETWORK_SENT:
					b->estado=_NETWORK_OK;
					break;
				case _CPIN_SENT:
					b->estado=_CPIN_OK;
					break;
				case _CRFOP_SENT:
					b->estado=_CRFOP_OK;
					break;
				case _SENT:
					{
						b->estado=_SENT_OK;
						b->rxbuff[0]='\0';
						ITM0_Write("\r\nLora _SENT_OK\r\n",strlen("\r\nLora _SENT_OK\r\n"));
					}
					break;

			}

			ITM0_Write("\r\nLora _OK\r\n",strlen("\r\nLora _OK\r\n"));
						//Clean rxbuff_hld
		}
			break;
		case _RCV:
		{
			ITM0_Write("\r\nLora _RCV\r\n",strlen("\r\nLora _RCV\r\n"));
				b->estado=_DATO_RECIBIDO;
				b->_data_available=1;
					int i=0;
					int j=0;
					int lngt=0;
					while (b->rxbuff[i] != '=')
					{ i++;}
					i++;
					while (b->rxbuff[i]!= ',')
					{
						b->rxaddress[j]=b->rxbuff[i];			//Save the address
						i++;
						j++;
						if (j > 6)
						{
							Lora_rx_del(b);
							break;
						}
					}
					b->rxaddress[j]='\0';
					j=0;
					i++;
					while (b->rxbuff[i]!= ',')
					{
						b->rx_items[j]=b->rxbuff[i];			//Save the qty of data
						i++;
						j++;
						if (j > 4)
						{
							Lora_rx_del(b);
							break;
						}
					}
					b->rx_items[j]='\0';
					j=0;
					i++;
					lngt=atoi(b->rx_items,10);
					//lngt=lngt+i;
					//while(lngt>i)   //Ahota busco leer lo datos recibid y luego verificar si ok
					while (b->rxbuff[i]!= ',')
					{
						b->dataRCV[j]=b->rxbuff[i];			//Save the data
						j++;
						i++;
						if (j > lngt)
						{
							Lora_rx_del(b);
							break;
						}
					}
					b->_n_dataRCV=j;							//Save amount of data received
					j=0;
					i++;
					while (b->rxbuff[i]!= ',')
					{
						b->rx_convert[j]=b->rxbuff[i];			//save RSSI
						i++;
						j++;
						if (j > 5)
						{
							Lora_rx_del(b);
							break;
						}
					}
					b->rxRSSI=atoi(b->rx_convert,10);
					b->rx_convert[j]='\0';
					j=0;
					i++;
					while (b->rxbuff[i]!= '\r')
					{
						b->rx_convert[j]=b->rxbuff[i];			// Save SNR
						i++;
						j++;
						if (j > 5)
						{
							Lora_rx_del(b);
							break;
						}
					}

					b->rx_convert[j]='\0';
					b->rxSNR=atoi(b->rx_convert,10);
					b->rx_convert[0]='\0';


			    //Timer de enviar data cuando antes se recibe
				//LoRa_set_sndTIMER(b, 3000);
				b->rxbuff[0]='\0';
		}
			break;
		case _ERR:
			ITM0_Write("\r\nLora _ERR\r\n",strlen("\r\nLora _ERR\r\n"));
			break;
		case _RESET:
			ITM0_Write("\r\nLora _RESET\r\n",strlen("\r\nLora _RESET\r\n"));
			break;
		case _READY:
			ITM0_Write("\r\nLora _READY\r\n",strlen("\r\nLora _READY\r\n"));
			break;
		case _MODE:
			ITM0_Write("\r\nLora _MODE\r\n",strlen("\r\nLora _MODE\r\n"));
			break;
		case _IPR:
			ITM0_Write("\r\nLora _IPR\r\n",strlen("\r\nLora _IPR\r\n"));
			break;
		case _PARAMETER:
			ITM0_Write("\r\nLora _PARAMETER\r\n",strlen("\r\nLora _PARAMETER\r\n"));
			break;
		case _BAND:
			ITM0_Write("\r\nLora _BAND\r\n",strlen("\r\nLora _BAND\r\n"));
			break;
		case _ADDRESS:
			ITM0_Write("\r\nLora _ADDRESS\r\n",strlen("\r\nLora _ADDRESS\r\n"));
			break;
		case _NETWORK:
			ITM0_Write("\r\nLora _NETWORK\r\n",strlen("\r\nLora _NETWORK\r\n"));
			break;
		case _CPIN:
			ITM0_Write("\r\nLora _CPIN\r\n",strlen("\r\nLora _CPIN\r\n"));
			break;
		case _CRFOP:
			ITM0_Write("\r\nLora _CRFOP\r\n",strlen("\r\nLora _CRFOP\r\n"));
			break;
		case _SEND:
			ITM0_Write("\r\nLora _SEND\r\n",strlen("\r\nLora _SEND\r\n"));
			break;
		case _UID:
			ITM0_Write("\r\nLora _UID\r\n",strlen("\r\nLora _UID\r\n"));
			break;
		case _VER:
			ITM0_Write("\r\nLora _VER\r\n",strlen("\r\nLora _VER\r\n"));
			break;
		case _ERR_RCV_OF:
			ITM0_Write("\r\nLora _ERR_RCV_OF\r\n",strlen("\r\nLora _ERR_RCV_OF\r\n"));
			break;
		default:
			ITM0_Write("\r\nLora default\r\n",strlen("\r\nLora default\r\n"));
			break;
	}
 }
int Lora_idSTR(struct LoRa *a)
{		int f,g,h,i;
		if(a->rxitems > 270) return _ERR_RCV_OF;
		char data[10];
		int ndata=0;
		i=strlen("+OK=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+OK=",&i,data,&ndata,&g,&h, 0)==1) return _OK ;
		i=strlen("+RCV=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+RCV=",&i,data,&ndata,&g,&h, 0)==1) return _RCV ;
		i=strlen("+ERR=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+ERR=",&i,data,&ndata,&g,&h, 0)==1) return _ERR ;
		i=strlen("+RESET");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+RESET",&i,data,&ndata,&g,&h, 0)==1)return _RESET;
		i=strlen("+READY");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+READY",&i,data,&ndata,&g,&h, 0)==1) return _READY ;
		i=strlen("+MODE=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+MODE=",&i,data,&ndata,&g,&h, 0)==1) return _MODE ;
		i=strlen("+IPR=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+IPR=",&i,data,&ndata,&g,&h, 0)==1) return _IPR ;
		i=strlen("+PARAMETER=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+PARAMETER=",&i,data,&ndata,&g,&h, 0)==1) return _PARAMETER ;
		i=strlen("+BAND=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+BAND=",&i,data,&ndata,&g,&h, 0)==1) return _BAND ;
		i=strlen("+ADDRESS=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+ADDRESS=",&i,data,&ndata,&g,&h, 0)==1) return _ADDRESS ;
		i=strlen("+NETWORK=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+NETWORK=",&i,data,&ndata,&g,&h, 0)==1) return _NETWORK ;
		i=strlen("+CPIN=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+CPIN=",&i,data,&ndata,&g,&h, 0)==1) return _CPIN ;
		i=strlen("+CRFOP=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+CRFOP=",&i,data,&ndata,&g,&h, 0)==1) return _CRFOP ;
		i=strlen("+SEND=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+SEND=",&i,data,&ndata,&g,&h, 0)==1) return _SEND ;
		i=strlen("+UID=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+UID=",&i,data,&ndata,&g,&h, 0)==1) return _UID ;
		i=strlen("+VER=");
		if(FT_String_ND(a->rxbuff,&a->rxitems,"+VER=",&i,data,&ndata,&g,&h, 0)==1) return _VER ;
}
void Lora_rx_del(struct LoRa *a)
{
	a->rxbuff[0]='\0';
	a->rx_items[0]='\0';
	//a->dataRCV[0]='\0';
	a->rxRSSI=0;
	a->rxSNR=0;
}

