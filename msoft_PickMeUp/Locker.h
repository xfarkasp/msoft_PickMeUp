#pragma once
#include <vector>
#include "DataHandler.h"

enum class LockerSize
{
    SMALL,
    MEDIUM,
    BIG
};

class Locker
{
public:

    Locker(int32_t id, bool occupation, time_t deadline, LockerSize size) 
    :m_id(id), m_occupation(occupation), m_picupDeadline(deadline), m_size(size)
    {}
    bool checkInsertion() { return true; }
    void openLocker() {}

private:
    int32_t m_id;
    bool m_occupation;
    time_t m_picupDeadline;
    LockerSize m_size;
};
