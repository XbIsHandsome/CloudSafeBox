
#ifndef __DOOR_H
#define __DOOR_H

extern unsigned char FlagDefense;//������־��1-������0-����
extern unsigned char FlagLockSta;//����״̬��1-������0-����
extern unsigned long int LockTimeCount;//�����ʱ���ʱ

void doorInit(void);
unsigned char isDoorOpen(void);

void doorOpen(void);
	
#endif

