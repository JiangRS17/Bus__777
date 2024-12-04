// #ifndef SOFTBUS_H
// #define SOFTBUS_H

// #include "softbus_list.h"
// #include "../../controller/include/soft_control.h"
// #include <stdint.h>

// #define SOFTBUS_MAX_LISTS 100   // 最大软总线数

// extern SoftBusList *globalSoftBusList[SOFTBUS_MAX_LISTS];  // 存储所有软总线的数组
// static int busCounter = 0;     // 当前总线编号

// //SoftBus
// typedef pthread_mutex_t SoftBusMutex;

// //softbuscontroller



// typedef struct {
//     /* data */
//     void (*sendMessage)(SoftBusList *list, const void *message);   // 发送消息接口
//     void (*receiveMessage)(SoftBusList *list, void *message);      // 接收消息接口
//     int (*pendMessage)(SoftBusList *list);                       // 检查是否有待处理的消息
// } SoftBusAdapter;


// typedef struct {
//     SoftBusMutex lock;
//     unsigned int cnt; 
//     unsigned int busId;
//     ListNode list;
//     // 控制器，处理总线消息
//     SoftBusController *controller;  // 控制器用于处理消息
//     // 适配器，为不同的组件提供统一接口
//     SoftBusAdapter *adapter; // 适配器用于提供不同组件之间的适配层
// } SoftBusList;

// SoftBusList *CreateSoftBusList();

// int32_t DestroySoftBusList(unsigned int busId);

// int32_t SoftBusMgrInit(void);

// int32_t SoftBusMgrDeinit(int bridgeId);


// // typedef uintptr_t SoftBusCond;

// // /*cond*/
// // int32_t SoftBusCondInit(SoftBusCond *cond);
// // // int32_t SoftBusCondWait(SoftBusCond *cond, SoftBusSysTime *time);
// // int32_t SoftBusCondDestroy(SoftBusCond *cond);

// // typedef struct 
// // {
// //     int64_t sec;
// //     int64_t usec;
// // } SoftBusSysTime;

// // //timer;

// // void *SoftBusCreateTimer(void **timeId, void *timerFunc, unsigned int type);
// // int SoftBusStartTimer(void *timeId, unsigned int tickets);
// // int SoftBusDeleteTimer(void *timeId);
// // int32_t SoftBusGetTime(SoftBusSysTime *sysTime);

// #endif