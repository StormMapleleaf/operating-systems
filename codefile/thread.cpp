//
// Created by storm on 24-9-3.
//
#include <iostream>
#include <thread>  // 使用标准库的线程功能
using namespace std;

// 定义线程执行的函数
void Ta() {
    while (1) {cout << 1;}
}

void Tb() {
    while (1) {cout << 2;}
}

int main() {
    // 创建两个线程，分别执行 Ta 和 Tb
    thread t1(Ta);
    thread t2(Tb);//因为创建了两个执行流，所以不会循环某一个函数，而是两个同时执行

    // 等待两个线程完成
    t1.join();
    t2.join();

    return 0;
}
