
#include "main.h"


struct LoRa
{
	char address[6];
	char rxaddress[6];
	char rx_items[4];
	char rx_convert[6];
	char dest_address[6];
	char network[3];
	char band[10];
	char parameter[9];
	char password[33];
	char power[3];
	char rxbuff[384];
	char dataRCV[384];
	char dataRCV_hld[384];
	int  _n_dataRCV;
	char txbuff[384];
	char txbuff2SND[384];
	int rxitems;
	int txitems;
	int rxRSSI;
	int rxSNR;
	uint8_t estado;
	uint8_t tmr_dly_en;
	uint8_t tmr_dly_ON;
	int tmr_dly;
	int tmr_dlyCNT;
	int timeAIR;
	int test;
	uint8_t _data_available;
};


#define ERROR_BUFFER_SIZE 1
#define NO_RN		"1",
#define NO_AT		"2",
#define NO_EQUAL	"3",
#define UNKNOWN_CMD	"4",
#define TK_OT		"10",
#define RX_OT		"11",
#define	CRC_ERROR	"12",
#define TX_OVERFOLW "13",
#define UNKNOWN_ERR	"14"

enum estado{
	_IDLE,
	_AT_SENT,
	_RESET_SENT,
	_FACTORY_SENT,
	_UID_SENT,
	_VER_SENT,

	_MODE_OK,
	_MODE_SENT,
	_PARAMETER_SENT,
	_PARAMETER_OK,
	_BAND_SENT,
	_BAND_OK,
	_ADDR_SENT,
	_ADDR_OK,
	_NETWORK_SENT,
	_NETWORK_OK,
	_CPIN_SENT,
	_CPIN_OK,
	_CRFOP_SENT,
	_CRFOP_OK,
	_SENT,
	_SENT_OK,

	IDLE,
	ENVIANDO,
	ENVIADO,
	LORA_ERR,
	_DATO_RECIBIDO
};
// Used to recognize string type
enum lora_STR{
	_OK,
	_RCV,
	_RESET,
	_READY,
	_MODE,
	_IPR,
	_PARAMETER,
	_BAND,
	_ADDRESS,
	_NETWORK,
	_CPIN,
	_CRFOP,
	_SEND,
	_VER,
	_UID,
	_ERR,
	_ERR_RCV_OF
};

//LoRa commands
enum lora_cmd{
	CMD_AT,
	CMD_RESET,
	CMD_FACTORY,
	CMD_UID,
	CMD_VER
};


int Lora_idSTR(struct LoRa *); 								//Identifica el tipo de string recibido
int LoRa_Send(struct LoRa *, UART_HandleTypeDef *);
void LoRa_reset_sndTIMER(struct LoRa *, int);
void LoRa_set_sndTIMER(struct LoRa *, int);

void Lora_RESET(struct LoRa *);
void Lora_snd_CMND(struct LoRa *,UART_HandleTypeDef *, uint8_t);		//char *  es el comando UID,VER,RESET,FACTORY,AT

/*void Lora_set_MODE(struct LoRa *, uint8_t ); 
void Lora_set_IPR(struct LoRa *, int );
void Lora_set_PARAMETER(struct LoRa *, uint8_t, uint8_t, uint8_t, uint8_t );
void Lora_set_BAND(struct LoRa *, long);
void Lora_set_ADDRESS(struct LoRa *, int);
void Lora_set_NETID(struct LoRa *, uint8_t);
void Lora_set_CPIN(struct LoRa *, char *);
void Lora_set_POWER(struct LoRa *, uint8_t);
void Lora_set_RESET(struct LoRa *);
char * Lora_rd_UID(struct LoRa *);
char * Lora_rd_VER(struct LoRa *);*/







