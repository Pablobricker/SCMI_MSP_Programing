void RTC_disabling(){
    RTCCTL01_H = RTCHOLD_H;
}

void RTC_setTime(int hour, int min){
    RTCSEC =  0x00;
    RTCHOUR = hour;
    RTCMIN = min;
}

void RTC_setDate(int day, int month, int year){
    RTCDAY = day;
    RTCMON = month;
    RTCYEAR = year;
}

void RTC_setAlarm(int min_A){
    RTCCTL01 |= RTCAIE;
    __enable_interrupt();
    RTCAMIN = min_A;
    RTCAMIN |= BIT7; //Activa la alarma de Min
    RTCAHOUR = 0x00;
    RTCADOW = 0x00;
    RTCADAY = 0x00;
}

void RTC_enable(){
    RTCCTL01_H &= ~(RTCHOLD_H);
}

#pragma vector =  RTC_VECTOR
__interrupt void RTC_ISR(){
    RTCCTL01 &= ~RTCAIFG;
    _low_power_mode_off_on_exit();
}
