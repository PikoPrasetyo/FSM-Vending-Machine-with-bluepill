/*
 * task.c
 *
 *  Created on: Nov 20, 2019
 *      Author: Piko Prasetyo
 */
#include "task.h"
#include "stm32f1xx.h"
#include "UART.h"
#include "main.h"
//#include ""

unsigned char Button500_ok(void);
unsigned char Button1000_ok(void);
unsigned char ButtonProses_ok(void);
unsigned char ButtonBatal_ok(void);

#define Plus500 				HAL_GPIO_WritePin(S500_GPIO_Port, S500_Pin, GPIO_PIN_SET)
#define Plus1000 				HAL_GPIO_WritePin(S1000_GPIO_Port, S1000_Pin, GPIO_PIN_SET)
#define KeluarKembalian			HAL_GPIO_WritePin(KeluarKoin_GPIO_Port, KeluarKoin_Pin, GPIO_PIN_SET)
#define KeluarKaleng			HAL_GPIO_WritePin(KeluarKaleng_GPIO_Port, KeluarKaleng_Pin, GPIO_PIN_SET)
#define offPlus500 				HAL_GPIO_WritePin(S500_GPIO_Port, S500_Pin, GPIO_PIN_RESET)
#define offPlus1000 			HAL_GPIO_WritePin(S1000_GPIO_Port, S1000_Pin, GPIO_PIN_RESET)
#define offKeluarKembalian		HAL_GPIO_WritePin(KeluarKoin_GPIO_Port, KeluarKoin_Pin, GPIO_PIN_RESET)
#define offKeluarKaleng			HAL_GPIO_WritePin(KeluarKaleng_GPIO_Port, KeluarKaleng_Pin, GPIO_PIN_RESET)

#define Yes	1
#define No	0
enum state {start, startDelay, enterCoin, s500, s1000, s1500, s2000, kembali500, kembali1000, Proses, Batal, jeda1, jeda2, jeda3, jeda4} keadaan;
volatile uint8_t debounce1=0xFF;
volatile uint8_t debounce2=0xFF;
volatile uint8_t debounce3=0xFF;
volatile uint8_t debounce4=0xFF;
volatile uint8_t button=0;
volatile uint16_t timeout;
volatile uint16_t batas = 1000;
volatile uint16_t jeda01;
volatile uint16_t jeda02;
volatile uint16_t jeda03;
volatile uint16_t jeda04;
char terpilih[10];
int menu;

void Task_Inisialisasi(void){
	timeout=0;
	keadaan=start;
}

void Task_Run(void){
	switch(keadaan){
	case start:
	{
		offKeluarKembalian;
		offKeluarKaleng;
		UART_Print("**** VENDING MACHINE 2019 copyright PIKO ***** \n");
		keadaan=startDelay;
		break;
	}
	case startDelay:
	{
		if (++timeout > 1000) // dari scheduler sebesar 2ms*1000= 2 detik
		{
			UART_Print("Enter Coin \n"); //current state
			timeout=0;
			keadaan=enterCoin;
		}
		break;
	}
	case enterCoin:
	{
		if(Button500_ok()){
			keadaan=s500;
			UART_Print("koin 500 telah masuk \n");
			UART_Print("saldo: Rp500,- \n");
		}
		if(Button1000_ok()){
			keadaan=s1000;
			UART_Print("koin 1000 telah masuk \n");
			UART_Print("saldo: Rp1000,- \n");
		}
		if(ButtonProses_ok()||ButtonBatal_ok()){
			UART_Print("Coin belum cukup \n");
			keadaan=startDelay;
		}
		break;
	}
	case s500:
	{
		if(Button500_ok()){
			keadaan=s1000;
			UART_Print("koin 500 telah masuk \n");
			UART_Print("saldo: Rp1000,- \n");
		}
		if(Button1000_ok()){
			keadaan=s1500;
			UART_Print("koin 1000 telah masuk \n");
			UART_Print("saldo: Rp1500,- \n");
		}
		if(ButtonProses_ok()){
			UART_Print("Coin belum cukup \n");
			keadaan=s500;
		}
		if(ButtonBatal_ok()){
			keadaan=Batal;
			UART_Print("500 KELUAR \n");
		}
		break;
	}
	case s1000:
	{
		//UART_Receive(terpilih);
		if(Button500_ok()){
			keadaan=s1500;
			UART_Print("koin 500 telah masuk \n");
			UART_Print("saldo: Rp1500,- \n");
		}
		if(Button1000_ok()){
			keadaan=s2000;
			UART_Print("koin 1000 telah masuk \n");
			UART_Print("saldo: Rp2000,- \n");
		}
		if(ButtonProses_ok()){
			keadaan=Proses;
			menu=1;
		}
		if(ButtonBatal_ok()){
			keadaan=Batal;
			UART_Print("1000 KELUAR \n");
		}
		break;
	}
	case s1500:
	{
		//terpilih=UART_Receive(*pData);
		if(Button500_ok()){
			keadaan=s2000;
			UART_Print("koin 500 telah masuk \n");
			UART_Print("saldo: Rp2000,- \n");
		}
		if(Button1000_ok()){
			keadaan=kembali500;
			UART_Print("500 KELUAR \n");
			UART_Print("saldo: Rp2000,- \n");
			Plus500;
		}
		if(ButtonProses_ok()){
			keadaan=Proses;
			menu=2;
		}
		if(ButtonBatal_ok()){
			keadaan=Batal;
			UART_Print("1500 KELUAR \n");
		}
		break;
	}
	case s2000:
	{
		if(Button500_ok()){
			keadaan=kembali500;
			UART_Print("500 KELUAR \n");
			UART_Print("saldo: Rp2000,- \n");
			Plus500;
		}
		if(Button1000_ok()){
			keadaan=kembali1000;
			UART_Print("1000 KELUAR \n");
			UART_Print("saldo: Rp2000,- \n");
			Plus1000;
		}
		if(ButtonProses_ok()){
			keadaan=Proses;
			menu=3;
		}
		if(ButtonBatal_ok()){
			keadaan=Batal;
			UART_Print("2000 KELUAR \n");
		}
		break;
	}
	case kembali500:{
		Plus500;
		keadaan=jeda1;
		break;
	}
	case kembali1000:{
		Plus1000;
		keadaan=jeda1;
		break;
	}
	case Proses:
	{
		UART_Print("Pilih minuman \n");
		HAL_UART_Receive(&huart1, (uint8_t *)terpilih, 10, 5000);
		if( strcmp( terpilih, "0x00000001" ) == 0 ){
			if(menu==1){
				KeluarKaleng;
				UART_Print("Kaleng Minuman Keluar \n");
				UART_Print("Coke \n");
				memset( terpilih, 0, 10 );
				menu=0;
				keadaan=jeda2;
			}
			else{
				if(menu==2){
					KeluarKaleng;
					UART_Print("Kaleng Minuman Keluar \n");
					UART_Print("Coke \n");
					UART_Print("kembali 500 \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda2;
				}
				if(menu==3){
					KeluarKaleng;
					UART_Print("Kaleng Minuman Keluar \n");
					UART_Print("Coke \n");
					UART_Print("kembali 1000 \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda2;
				}
			}
		}
		if( strcmp( terpilih, "0x00000002" ) == 0 ){
			if(menu==1){
				KeluarKaleng;
				UART_Print("Kaleng Minuman Keluar \n");
				UART_Print("Fanta \n");
				memset( terpilih, 0, 10 );
				menu=0;
				keadaan=jeda2;
			}
			else{
				if(menu==2){
					KeluarKaleng;
					UART_Print("Kaleng Minuman Keluar \n");
					UART_Print("Fanta \n");
					UART_Print("kembali 500 \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda2;
				}
				if(menu==3){
					KeluarKaleng;
					UART_Print("Kaleng Minuman Keluar \n");
					UART_Print("Fanta \n");
					UART_Print("kembali 1000 \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda2;
				}
			}
		}
		if( strcmp( terpilih, "0x00000003" ) == 0 ){
			if(menu==2){
				KeluarKaleng;
				UART_Print("Kaleng Minuman Keluar \n");
				UART_Print("Sprite \n");
				memset( terpilih, 0, 10 );
				menu=0;
				keadaan=jeda2;
			}
			else{
				if(menu==1){
					HAL_GPIO_WritePin(S500_GPIO_Port, S500_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(S500_GPIO_Port, S1000_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(KeluarKoin_GPIO_Port, KeluarKoin_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(KeluarKaleng_GPIO_Port, KeluarKaleng_Pin, GPIO_PIN_SET);
					UART_Print("uang kurang \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda3;
				}
				if(menu==3){
					KeluarKaleng;
					UART_Print("Kaleng Minuman Keluar \n");
					UART_Print("Sprite \n");
					UART_Print("kembali 500 \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda2;
				}
			}
		}
		if( strcmp( terpilih, "0x00000004" ) == 0 ){
			if(menu==2){
				KeluarKaleng;
				UART_Print("Kaleng Minuman Keluar \n");
				UART_Print("Pepsi \n");
				memset( terpilih, 0, 10 );
				menu=0;
				keadaan=jeda2;
			}
			else{
				if(menu==1){
					HAL_GPIO_WritePin(S500_GPIO_Port, S500_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(S500_GPIO_Port, S1000_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(KeluarKoin_GPIO_Port, KeluarKoin_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(KeluarKaleng_GPIO_Port, KeluarKaleng_Pin, GPIO_PIN_SET);
					UART_Print("uang kurang \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda3;
				}
				if(menu==3){
					KeluarKaleng;
					UART_Print("Kaleng Minuman Keluar \n");
					UART_Print("Pepsi \n");
					UART_Print("kembali 500 \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda2;
				}
			}
		}
		if( strcmp( terpilih, "0x00000005" ) == 0 ){
			if(menu==3){
				KeluarKaleng;
				UART_Print("Kaleng Minuman Keluar \n");
				UART_Print("Pocari Sweat \n");
				memset( terpilih, 0, 10 );
				menu=0;
				keadaan=jeda2;
			}
			else{
				if(menu==1){
					HAL_GPIO_WritePin(S500_GPIO_Port, S500_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(S500_GPIO_Port, S1000_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(KeluarKoin_GPIO_Port, KeluarKoin_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(KeluarKaleng_GPIO_Port, KeluarKaleng_Pin, GPIO_PIN_SET);
					UART_Print("uang kurang \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda3;
				}
				if(menu==2){
					HAL_GPIO_WritePin(S500_GPIO_Port, S500_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(S500_GPIO_Port, S1000_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(KeluarKoin_GPIO_Port, KeluarKoin_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(KeluarKaleng_GPIO_Port, KeluarKaleng_Pin, GPIO_PIN_SET);
					UART_Print("uang kurang \n");
					memset( terpilih, 0, 10 );
					menu=0;
					keadaan=jeda4;
				}
			}
		}
		break;
	}
	case Batal:
	{
		KeluarKembalian;
		UART_Print("Koin Yang Telah Masuk Keluar \n");
		keadaan=jeda2;
		break;
	}
	case jeda1:
	{
		if(++jeda01==batas){
			offPlus500;
			offPlus1000;
			jeda01=0;
			keadaan=s2000;
		}
		break;
	}
	case jeda2:
	{
		if(++jeda02==batas){
			offKeluarKembalian;
			offKeluarKaleng;
			jeda02=0;
			keadaan=start;
		}
		break;
	}
	case jeda3:
	{
		if(++jeda03==batas){
			HAL_GPIO_WritePin(S500_GPIO_Port, S500_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(S500_GPIO_Port, S1000_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(KeluarKoin_GPIO_Port, KeluarKoin_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(KeluarKaleng_GPIO_Port, KeluarKaleng_Pin, GPIO_PIN_RESET);
			jeda03=0;
			keadaan=s1000;
		}
		break;
	}
	case jeda4:
	{
		if(++jeda04==batas){
			HAL_GPIO_WritePin(S500_GPIO_Port, S500_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(S500_GPIO_Port, S1000_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(KeluarKoin_GPIO_Port, KeluarKoin_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(KeluarKaleng_GPIO_Port, KeluarKaleng_Pin, GPIO_PIN_RESET);
			jeda04=0;
			keadaan=s1500;
		}
		break;
	}
	}
}

unsigned char Button500_ok(void){
	static unsigned char debounce1=0xFF;
	unsigned char detectedFLag=0;
	if(HAL_GPIO_ReadPin(a500_GPIO_Port, a500_Pin)== GPIO_PIN_RESET){
		debounce1=(debounce1<<1);
	} else {
		debounce1= (debounce1<<1)|1;
	}
	if (debounce1==0x03) {
		//printf(" \n\r");
		detectedFLag=1;
	}
	return detectedFLag;
}

unsigned char Button1000_ok(void){
	static unsigned char debounce2=0xFF;
	unsigned char detectedFLag=0;
	if(HAL_GPIO_ReadPin(a1000_GPIO_Port, a1000_Pin)== GPIO_PIN_RESET){
		debounce2=(debounce2<<1);
	} else {
		debounce2= (debounce2<<1)|1;
	}
	if (debounce2==0x03) {
		//printf(" \n\r");
		detectedFLag=1;
	}
	return detectedFLag;
}

unsigned char ButtonProses_ok(void){
	static unsigned char debounce3=0xFF;
	unsigned char detectedFLag=0;
	if(HAL_GPIO_ReadPin(PROSES_GPIO_Port, PROSES_Pin)== GPIO_PIN_RESET){
		debounce3=(debounce3<<1);
	} else {
		debounce3=(debounce3<<1)|1;
	}
	if (debounce3==0x03) {
		//printf(" \n\r");
		detectedFLag=1;
	}
	return detectedFLag;
}

unsigned char ButtonBatal_ok(void){
	static unsigned char debounce4=0xFF;
	unsigned char detectedFLag=0;
	if(HAL_GPIO_ReadPin(BATAL_GPIO_Port, BATAL_Pin)== GPIO_PIN_RESET){
		debounce4=(debounce4<<1);
	} else {
		debounce4= (debounce4<<1)|1;
	}
	if (debounce4==0x03) {
		//printf(" \n\r");
		detectedFLag=1;
	}
	return detectedFLag;
}
