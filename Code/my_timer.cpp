#include "my_timer.h"
#include <assert.h>

MyTimer::MyTimer(MyTIMER_TYPE init_type){
    type_ = init_type;
    switch (type_) {
        case HOUR:
            break;
        case MINUTE:
            break;
        case SECOND:
            break;
        case MILLISECOND:
            break;
        case MICROSECOND:
            break;
        case NANOSECOND:
            break;
        default:
            throw "undefined MyTimer type";
    }
    last_record_time_point_ = init_time_point_ = chrono::steady_clock::now();
}

void MyTimer::SetRecordTimePoint() {
    last_record_time_point_ = chrono::steady_clock::now();
}
double MyTimer::GetDuration(chrono::steady_clock::time_point last_time_point) {
    now_time_point_ = chrono::steady_clock::now();
    switch (type_) {
        case HOUR:
            return chrono::duration_cast<hour_type>(now_time_point_ - last_time_point).count();
            break;
        case MINUTE:
            return chrono::duration_cast<minute_type>(now_time_point_ - last_time_point).count();
            break;
        case SECOND:
            return chrono::duration_cast<second_type>(now_time_point_ - last_time_point).count();
            break;
        case MILLISECOND:
            return chrono::duration_cast<millisecond_type>(now_time_point_ - last_time_point).count();
            break;
        case MICROSECOND:
            return chrono::duration_cast<microsecond_type>(now_time_point_ - last_time_point).count();
            break;
        case NANOSECOND:
            return chrono::duration_cast<nanosecond_type>(now_time_point_ - last_time_point).count();
            break;
        default:
            throw "undefined MyTimer type";
    }
}
double MyTimer::GetRecordDuration(bool set_record_time_point) {
    double duration = GetDuration(last_record_time_point_);
    if (set_record_time_point) last_record_time_point_ = now_time_point_;
    return duration;
}
double MyTimer::GetAllDuration(bool set_record_time_point ) {
    double duration = GetDuration(init_time_point_);
    if (set_record_time_point) last_record_time_point_ = now_time_point_;
    return duration;
}
const char * MyTimer::GetTypeName() {
    return type_name[type_];
}
const char * MyTimer::GetTypeUnitName() {
    return type_unit_name[type_];
}

MyTimer::MyTIMER_TYPE MyTimer::GetType() {
    return type_;
}