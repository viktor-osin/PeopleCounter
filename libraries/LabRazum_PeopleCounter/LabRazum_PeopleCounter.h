#pragma once
#include <Arduino.h>

class Room 
{   
    public:
        Room(byte pinSensor1, byte pinSensor2, byte pinRelay);
        Room(byte pinSensor1, byte pinSensor2, byte pinRelay, byte pinHandMode); 
        Room(byte pinSensor1, byte pinSensor2, byte pinRelay, byte pinHandMode, byte pinPhotoSensor, int photoSensorThreshold);
        byte lightControl();        //включает управление освещением по счетчику людей
        byte getCountPeople();      //возвращает текущее количество людей в комнате
        bool getLightRoom();        //возвращает текущее состояние света в комнате - вкл (1) или выкл (0)
        void begin();               //настройка портов МК
        void debug(bool onDebug);   //активация режима отладки (1), отключение режима (0)
        

    private:
        void start_timer_people();          //таймер на 500 мс для задержки между срабатываниями датчиков
        byte count_people_val();            //метод определяет очередность срабатывания датчиков и выдает количество людей в комнате
        void lightRoom(bool lightOn);       //метод изменяет значение количества людей в комнате, если оно изменилось
        void valuePeopleChange();           //меняет значение счетчика людей, если количество человек в комнате изменилось 
        byte _pinSensor1, _pinSensor2, _pinHandMode, _pinRelay, _pinPhotoSensor; 
        bool _firstSensor1, _firstSensor2, _IRsensor1, _IRsensor2, _darkLight, _lightOn, _onDebug;
        unsigned long _timerPeople, _oldTimerPeople;
        byte _valuePeople;
        int _photoSensorThreshold;       
};
