#include<stdio.h>
#include<stdlib.h>
#define NUM 11
#define SIZE 4

int main() {
    int packets[NUM] = {1,2,3,5,6,8,11,12,15,16,19};
    int x,x_new,t,capacity;
    int lct;
    int i;

    x=0;
    capacity = 10;
    lct = packets[0];

    while(i<NUM) {
        t = packets[i];
        x_new = x -(t-lct);
        if(x_new < 0){
            x_new = 0;
        }
        else if(x_new > capacity) {
            printf("Packet %d arriving at %d is non conforming\n",i,packets[i]);
            i++;
            continue;
        }
        x = x_new + SIZE;
        lct = t;
        printf("Packet %d arriving at %d is conforming\n",(i+1),packets[i]);
        i++;
    }
}