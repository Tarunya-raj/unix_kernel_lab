#ifndef PROCESS_H
#define PROCESS_H
#include <cstdint>
constexpr int NPROC=10;
enum PROCESS_STATE
{
    P_RUNNING,
    P_SLEEP_INMEMORY,
    P_SLEEP_SWAPPED,
    P_ZOMBIE,
    P_USER_MODE,
    P_KERNEL_MODE,
    P_CREATED,
    P_READY_TO_RUN_SWAPED,
    P_READY_TO_RUN_INMEMORY

};


class ContextLayer{};
ContextLayer contextLayersStack[NPROC];
class UFDT
{

};
class perProcess
{

};
class PageTable
{

};
class proc;
class UserArea
{
    uint32_t currentDirectory{};
    uint32_t currentRoot{};
    UFDT ufdt;
    uint32_t UID;
    uint32_t PID;
    proc    *processEntry;




};
using KStack =  uint64_t;// 64 bit machine stack pointer
class EventDescriptor{};
class proc
{
    //spinlock
    int PID{0};
    int UID{0};
    int PPID{0};
    PROCESS_STATE pState{PROCESS_STATE::P_CREATED};
    PageTable *pgTable;
    KStack    k_Stack;
    uint32_t   pSize;
    UserArea u_area{};
    EventDescriptor evntdes;
    ContextLayer* ContextLayers[0];




};


#endif