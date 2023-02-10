#ifndef BADDTREE_MY_TIMER_H
#define BADDTREE_MY_TIMER_H

#include <chrono>
using namespace std;
class MyTimer {
public:
    const char * type_name[6] = {"hour", "minute", "second", "millisecond", "microsecond", "nanosecond"};
    const char * type_unit_name[6] = {"h", "m", "s", "ms", "us", "ns"};
    enum MyTIMER_TYPE {
        HOUR,
        MINUTE,
        SECOND,
        MILLISECOND,
        MICROSECOND,
        NANOSECOND
    };
    typedef chrono::duration<double, ratio<3600, 1> > hour_type;
    typedef chrono::duration<double, ratio<60, 1> > minute_type;
    typedef chrono::duration<double, ratio<1, 1> > second_type;
    typedef chrono::duration<double, ratio<1, 1000> > millisecond_type;
    typedef chrono::duration<double, ratio<1, 1000000> > microsecond_type;
    typedef chrono::duration<double, ratio<1, 1000000000> > nanosecond_type;
    MyTimer(MyTIMER_TYPE init_type = SECOND);
    void SetRecordTimePoint();
    double GetRecordDuration(bool set_record_time_point = 0);
    double GetAllDuration(bool set_record_time_point = 0);
    const char * GetTypeName();
    const char * GetTypeUnitName();
    MyTIMER_TYPE GetType();
private:
    double GetDuration(chrono::steady_clock::time_point last_time_point);
    chrono::steady_clock::time_point last_record_time_point_, init_time_point_, now_time_point_;
    void SetTypeName();
    MyTIMER_TYPE type_;
};


#endif //BADDTREE_MY_TIMER_H
