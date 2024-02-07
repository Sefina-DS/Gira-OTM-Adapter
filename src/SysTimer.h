#pragma once

struct TIMER
{
    unsigned long alarm = 0;
    unsigned long funktion = 0;
    
};
extern TIMER timer;

void timer_funktion();