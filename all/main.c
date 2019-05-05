#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "card_ctrl.h"
#include "tty.h"
#include "camera.h"

//02 20 52
int  Card_Request(){
	uchar txbuff[11];	
	uchar rxbuff[11];	
	int i = 0;
	uchar len = 0;
	txbuff[0]=0x02;
	txbuff[1]=0x20;
	txbuff[2]=0x52;
        tty_fflush();
	tty_write(txbuff,3);
	usleep(100);
	//read
        tty_read(&len,1);
        tty_read(rxbuff,len);
        fflush(stdout);
        if(len > 1){
                return 1;
        }else{  
                return -1;
        }
}
//01 21
int  Card_Anticoll(uchar *card_NO){
	uchar txbuff[11];	
	uchar rxbuff[11];	
	int i = 0;
	uchar len = 0;
	txbuff[0]=0x01;
	txbuff[1]=0x21;
        tty_fflush();
	tty_write(txbuff,2);
	usleep(100);
	//read
        tty_read(&len,1);
        tty_read(rxbuff,len);
        fflush(stdout);
        if(len > 1){
		memcpy(card_NO,&rxbuff[1],4);
                return 1;
        }else{  
                return -1;
        }
}
//01 22
int  Card_Select(){
        uchar txbuff[11];
        uchar rxbuff[11];
        int i = 0;
        uchar len = 0;
        txbuff[0]=0x01;
        txbuff[1]=0x22;
        tty_fflush();
        tty_write(txbuff,2);
	usleep(100);
        //read
        tty_read(&len,1);
        tty_read(rxbuff,len);
        fflush(stdout);
        if(len > 1){
                return 1;
        }else{ 
                return -1;
        }
}

//04 23 60 00 00
int  Card_Auth_EE(uchar addr,uchar block){
        uchar txbuff[11];
        uchar rxbuff[11];
        int i = 0;
        uchar len = 0;
        txbuff[0]=0x04;
        txbuff[1]=0x23;
        txbuff[2]=0x60;
        txbuff[3]=addr;
        txbuff[4]=block;
        tty_fflush();
        tty_write(txbuff,5);
	usleep(100);
        //read
        tty_read(&len,1);
        tty_read(rxbuff,len);
        fflush(stdout);
        if(rxbuff[1] == 0x00){
                return 1;
        }else{ 
                return -1;
        }
}

//09 24 60 00 FF FF FF FF FF FF 
int  Card_Load_Key_EE(uchar addr,uchar *key){
        uchar txbuff[11];
        uchar rxbuff[11];
        int i = 0;
        uchar len = 0;
        txbuff[0]=0x09;
        txbuff[1]=0x24;
        txbuff[2]=0x60;
        txbuff[3]=addr;
	memcpy(&txbuff[4],key,6);
        tty_fflush();
        tty_write(txbuff,10);
	usleep(100);
        //read
        tty_read(&len,1);
        tty_read(rxbuff,len);
        fflush(stdout);
        if(rxbuff[0] == 0x00){
                return 1;
        }else{ 
                return -1;
        }       
}

//02 25 01
int  Card_Read(uchar block,uchar *data){
        uchar txbuff[11];
        uchar rxbuff[24];
        int i = 0;
        uchar len = 0;
        txbuff[0]=0x02;
        txbuff[1]=0x25;
        txbuff[2]=block;
        tty_fflush();
        tty_write(txbuff,3);
	usleep(100);
        //read
        tty_read(&len,1);
        tty_read(rxbuff,len);
	//for(i = 0;i < len;i ++)
	//	printf("rxbuff[%d] = 0x%x\n",i,rxbuff[i]);
        fflush(stdout);
        if(len > 1){
		memcpy(data,&rxbuff[1],16);
                return 1;
        }else{ 
                return -1;
        }
}
//12 26 01  FF EE DD CC BB AA 99 88 77 66 55 44 33 22 11 00
int  Card_Write(uchar block,uchar *data){
        uchar txbuff[24];
        uchar rxbuff[11];
        int i = 0;
        uchar len = 0;
        txbuff[0]=0x12;
        txbuff[1]=0x26;
        txbuff[2]=block;
        memcpy(&txbuff[3],data,16);
	
	//for(i = 0;i < 19;i ++)
	//	printf("txbuff[%d] = 0x%x\n",i,txbuff[i]);
        tty_fflush();
        tty_write(txbuff,19);
	usleep(100);
        //read
        tty_read(&len,1);
	//printf("len = %d\n",len);
        tty_read(rxbuff,len);
        fflush(stdout);
        if(rxbuff[0] == 0x00){
		//printf("write data OK\n");
                return 1;
        }else{ 
		//printf("write data faild\n");
                return -1;
        }
}
//01 10
int Card_Beep(uchar delay){
	uchar txbuff[11];
        uchar rxbuff[11];
        int i = 0;
        uchar len = 0;
        txbuff[0]=0x02;
        txbuff[1]=0x10;
        txbuff[2]=delay;
        tty_fflush();
        tty_write(txbuff,3);
        usleep(100);
}

int cmd_state;
static void* cmd(){
	while(1){
		getchar();
		cmd_state = 1;
	}
}


int quit_cmmand = 0;

void * quit() {
        while (1) {
                char q = getchar();
                if (q == 'q' || q == 'Q') {
                        quit_cmmand = 1;
                }
        }
}

/*
 *ic卡控制
 */

static int ic_card_state = 0;

void * ic_card() {
        int i =0;
        pthread_t th_cmd;
        uchar block = 0x1;
        uchar addr = 0x0;
        uchar key[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
        uchar data[16] = {0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00};
        uchar card_NO[4] = {0x00,0x00,0x00,0x00};
        tty_init();
        /* Create the threads */
        pthread_create(&th_cmd, NULL, cmd, 0);
        Card_Beep(1);
        sleep(1);
        
        while(1){
                //printf("Press any key to contiue...\n");
                
                if((Card_Request() < 0))
                        continue;
                if(Card_Anticoll(card_NO) < 0)
                        continue;
                printf("CARD NO:\t");
                for(i = 0;i < 4;i ++){
                        printf("%02X ",card_NO[i]);
                }
                printf("\n");

                //printf("BLOCK NO:\t%d\n",block);

                if(Card_Select() < 0)
                        continue;

                if(Card_Load_Key_EE(addr,key) < 0)
                        continue;

                if(Card_Auth_EE(addr,block) < 0)
                        continue;

                if(Card_Read(block,data) < 0)
                        continue;       
                
                if(Card_Write(block,data) < 0)
                        continue;

                if(Card_Read(block,data) < 0)
                        continue;
                

                ic_card_state = 1;

                sleep(2);       //防止过度刷卡
        }

        tty_end();
        /*
        while(1){
                printf("Press any key to contiue...\n");
                while(1){
                        if((Card_Request() < 0))
                                continue;
                        if(cmd_state == 1){
                                cmd_state = 0;
                                break;
                        }
                };
                if(Card_Anticoll(card_NO) < 0)
                        continue;
                printf("CARD NO:\t");
                for(i = 0;i < 4;i ++){
                        printf("%02X ",card_NO[i]);
                }
                printf("\n");
                printf("BLOCK NO:\t%d\n",block);
#if 1
                if(Card_Select() < 0)
                        continue;
                if(Card_Load_Key_EE(addr,key) < 0)
                        continue;
                if(Card_Auth_EE(addr,block) < 0)
                        continue;
                if(Card_Read(block,data) < 0)
                        continue;       
                printf("INDEX DATA:\t",block);
                for(i = 0;i < 16;i ++){
                        printf("%2X|",i);
                }
                printf("\n");
                printf("\t\t",block);
                for(i = 0;i < 16;i ++){
                        printf("---",i);
                }
                printf("\n");
                printf("Block%d data:\t",block);
                for(i = 0;i < 16;i ++){
                        printf("%02x ",data[i]);
                }
                printf("\n");
                printf("Write data:\t");
                for(i = 0 ;i < 16;i ++){
                        data[i] = ~data[i];
                        printf("%02x ",data[i]);        
                }
                printf("\n");
#if 1
                if(Card_Write(block,data) < 0)
                        continue;

                if(Card_Read(block,data) < 0)
                        continue;       
                printf("Block%d data:\t",block);
                for(i = 0;i < 16;i ++){
                        printf("%02x ",data[i]);
                }
                printf("\n");
#endif  
#endif
        }  
        */     
        return NULL;
}

/*
 *led 控制
*/
void horizotal(int fd) {
        unsigned char data[10] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
        int i;
        for (i = 0; i < 8; i++) {
                write(fd, data, 10);
        }

}
void vertical(int fd) {
        unsigned char data[10] = {0x0, 0x0, 0x0, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0,0x0};
        int i;
        for (i = 0; i < 8; i++) {
                write(fd, data, 10);
        }
}

void * led() {
        int fd=open("/dev/s3c2440_led0",O_RDWR);
        if (fd < 0) {
                printf("####Led device open fail####\n");
                return;
        }
        unsigned char LEDCODE[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
        unsigned int ledword;
        int i, j;
        int change = 1;
        horizotal(fd);
        ledword = (LEDCODE[3]<<8) | LEDCODE[0];
        ioctl(fd, 0x12, ledword);
        sleep(1);
        while (1) {
                for (i = 2; i >= 0; i--) {
                        for (j = 9; j >= 0; j--) {
                                if (ic_card_state) {
                                        break;
                                }
                                ledword = (LEDCODE[i]<<8) | LEDCODE[j];
                                ioctl(fd, 0x12, ledword);
                                sleep(1);
                        }
                        if (ic_card_state) {
                                break;
                        }
                }
                change = !change;
                if (ic_card_state) {
                        ic_card_state = 0;
                }
                if (change) {
                        horizotal(fd);
                } else {
                        vertical(fd);
                }
                ledword = (LEDCODE[3]<<8) | LEDCODE[0];
                ioctl(fd, 0x12, ledword);
                sleep(1);
        }
        close(fd);
        printf("Bye\n");
        return NULL;
}

void * start_camera() {
        printf("camera thread running...\n");
        return run_camera();
}
void * start_ic_card() {
        printf("ic card thread running...\n");
        return ic_card();
}
void * start_led() {
        printf("led thread running...\n");
        return led();
}


int main(void){
	pthread_t ic_thread;
        pthread_t camera_thread;
        pthread_t led_thread;

        if (pthread_create(&ic_thread, NULL, start_ic_card, NULL) != 0) {
                printf("can't create ic thread");
        }
        if (pthread_create(&camera_thread, NULL, start_camera, NULL) != 0) {
                printf("can't create camera thread");
        }
        if (pthread_create(&led_thread, NULL, start_led, NULL) != 0) {
                printf("can't create led thread");
        }

        pthread_join(ic_thread, NULL);
        pthread_join(camera_thread, NULL);
        pthread_join(led_thread, NULL);

        return 0;
}





