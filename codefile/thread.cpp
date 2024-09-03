//
// Created by storm on 24-9-3.
//
#include "thread.h"
void Ta(){while(1){cout<<1;}}
void Tb(){while(1){cout<<2;}}

int main(){
    create(Ta);
    create(Tb);

}