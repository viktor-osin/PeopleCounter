#include <LabRazum_PeopleCounter.h>
#include <Arduino.h> 

Room::Room(byte pinSensor1, byte pinSensor2, byte pinRelay)
{   
    _pinSensor1 = pinSensor1;
    _pinSensor2 = pinSensor2;
    _pinRelay = pinRelay;
    _handModeInit = 0;
    _photoSensorInit = 0;
}


Room::Room(byte pinSensor1, byte pinSensor2, byte pinRelay, byte pinHandMode)
{   
    _pinSensor1 = pinSensor1;
    _pinSensor2 = pinSensor2;
    _pinHandMode = pinHandMode;
    _pinRelay = pinRelay;
    _handModeInit = 1;
    _photoSensorInit = 0;
}


Room::Room(byte pinSensor1, byte pinSensor2, byte pinRelay, byte pinHandMode, byte pinPhotoSensor, int photoSensorThreshold)
{  
    _pinSensor1 = pinSensor1;
    _pinSensor2 = pinSensor2;
    _pinHandMode = pinHandMode;
    _pinRelay = pinRelay;
    _pinPhotoSensor = pinPhotoSensor;
    _photoSensorThreshold = photoSensorThreshold;
    _handModeInit = 1;
    _photoSensorInit = 1;
}

void Room::begin()
{   
    pinMode(_pinSensor1, INPUT);
    pinMode(_pinSensor2, INPUT);
    pinMode(_pinRelay, OUTPUT);
    digitalWrite(_pinRelay, LOW);

  if(_handModeInit == 1)
  {
    pinMode(_pinHandMode, INPUT);
  }
  
  if(_photoSensorInit== 1)
  {
    pinMode(_pinPhotoSensor, INPUT);
  }
  
}
void Room::debug(bool onDebug)
{
  _onDebug = onDebug;
}

//функция считывает показания двух ИК-датчиков, определяет, какой сработал первым 
byte Room::count_people_val()
{
  _IRsensor1 = digitalRead(_pinSensor1);
  _IRsensor2 = digitalRead(_pinSensor2);

  //фильтрация помех датчика №1 - при помехе (старт/остановка двигателя, включение/отключение мощной нагрузки) 
  //сигнал датчика переключается на 3-5мс и восстанавливается обратно, поэтому ставим таймер на 10 мс (с запасом)
  if((_IRsensor1 == 0) && (_timerIRsensor1On == 0)) //если пришел сигнал с датчика и таймер помехи не был запущен
  {
      _timerIRsensor1On = 1; //запускаем таймер и ставим флаг запуска
      timerIRsensor1(1);
  }

  if((_IRsensor1 == 1) && (_timerIRsensor1On == 1)) //если во время работы  таймера (10 мс), датчик меняет состояние, значит это была помеха 
  {
        timerIRsensor1(0); //останавливаем 
        _timerIRsensor1On = 0;
  }

  if((_IRsensor1 == 0) && (timerIRsensor1(2) >= 10)) //если сигнал "держится" на нуле больше 10 мс
  {
      _signalSensorGood = 1; //говорим, что сигнал "хороший"
      timerIRsensor1(0);     //останавливаем таймер
      _timerIRsensor1On = 0;
  }

  //фильтрация помех датчика №2
  if((_IRsensor2 == 0) && (_timerIRsensor2On == 0)) //если пришел сигнал с датчика и таймер помехи не был запущен
  {
      _timerIRsensor2On = 1; //запускаем таймер и ставим флаг запуска
      timerIRsensor2(1);
  }

  if((_IRsensor2 == 1) && (_timerIRsensor2On == 1)) //если во время работы  таймера помехи (10 мс), датчик меняет состояние, значит это была помеха 
  {
        timerIRsensor2(0); //останавливаем таймер
        _timerIRsensor2On = 0;
  }

  if((_IRsensor2 == 0) && (timerIRsensor2(2) >= 10)) //если сигнал "держится" на нуле больше 10 мс
  {
      _signalSensorGood = 1; //говорим, что сигнал "хороший"
      timerIRsensor2(0);     //останавливаем таймер
      _timerIRsensor2On = 0;
  }


  if(_onDebug == 1)
  {
    Serial.print("IR_sensor_1:" + String(_IRsensor1) + " IR_sensor_2:" + String(_IRsensor2));
  }
  
  //определяем, какой датчик сработал первым
  if((_firstSensor2 == 0) && (_firstSensor1 == 0) && (_signalSensorGood == 1))
  {
    if((_IRsensor1 == 0) && (_IRsensor2 == 1)) 
    {  
      start_timer_people();
      _firstSensor1 = 1;
    }
    
    if((_IRsensor2 == 0) && (_IRsensor1 == 1))
    {
      start_timer_people();
      _firstSensor2 = 1;
    }
  }
   
  valuePeopleChange();

  return _valuePeople;
}

//меняет значение счетчика людей, если количество человек в комнате изменилось 
void Room::valuePeopleChange()
{
  if((_firstSensor1 == 1) || (_firstSensor2 == 1))
  {
    _timerPeople = millis() - _oldTimerPeople;
    if(_timerPeople < 500)
    {
      if(_firstSensor1)
      {
        if(_IRsensor2 == 0)
        {
          _valuePeople++;
          _firstSensor1 = 0;
          _signalSensorGood = 0;
        }
      }
      if(_firstSensor2)
      {
        if((_IRsensor1 == 0) && (_valuePeople > 0))
        {
          _valuePeople--;
          _firstSensor2 = 0;
          _signalSensorGood = 0;
        }
      }
    }
    else
    {
       _firstSensor1 = 0;
       _firstSensor2 = 0;
       _signalSensorGood = 0;
    }         
  }
}

void Room::start_timer_people()
{
    _oldTimerPeople = millis();
    _timerPeople = 0;
}

byte Room::timerIRsensor1(byte mode)
{
  switch (mode)
  {
    case 0: //сброс счетчика
    _valTimerIRsensor1 = 0;
    return 0;
    break; 
    
    case 1: //старт счетчика - запоминаем, чему был равен millis()
    _oldTimerIRsensor1 = millis();
    _valTimerIRsensor1 = 0;
    return 1;
    break;

    case 2: //показания счетчика
    _valTimerIRsensor1 = millis() - _oldTimerIRsensor1;
    return _valTimerIRsensor1;
    break;
  }
}

byte Room::timerIRsensor2(byte mode)
{
  switch (mode)
  {
    case 0: //сброс счетчика
    _valTimerIRsensor2 = 0;
    return 0;
    break; 

    case 1: //старт счетчика - запоминаем, чему был равен millis()
    _oldTimerIRsensor2 = millis();
    _valTimerIRsensor2 = 0;
    return 1;
    break;

    case 2: //показания счетчика
    _valTimerIRsensor2 = millis() - _oldTimerIRsensor2;
    return _valTimerIRsensor2;
    break;
  }
}


byte Room::lightControl()
{
     if(_handModeInit == 1)
     {
        if(digitalRead(_pinHandMode) == 0) //если с кнопки пришел 0, включаем "ручной" режим
        {
            lightRoom(0);     //выключить свет в этой комнате
            _valuePeople = 0; //обнулить счетчик людей
        }
        else
        {
          if (count_people_val() > 0)  //если есть люди в комнате
          {
              lightRoom(1);    //включить свет в этой комнате
          }
          else
          {
              lightRoom(0);    //выключить свет в этой комнате
              _valuePeople = 0;
          }
          
          return _valuePeople;
        }
     }
     else
     {
          if (count_people_val() > 0)  //если есть люди в комнате
          {
              lightRoom(1);    //включить свет в этой комнате
          }
          else
          {
              lightRoom(0);    //выключить свет в этой комнате
              _valuePeople = 0;
          }          
          return _valuePeople;
     }
       
}

byte Room::getCountPeople()
{
    return _valuePeople;
}


void Room::lightRoom(bool lightOn)
{
    _lightOn = lightOn;
    if(_photoSensorInit == 1)
    {
      int sunlightValue = analogRead(_pinPhotoSensor);
      if(_onDebug == 1)
      {
        Serial.print(" PhotoSensor:" + String(sunlightValue));
        if(_darkLight == 1)
            Serial.println(" Light in Room: "+ String(_lightOn));
        else
            Serial.println(" Light in Room: 0");
      }

      if(sunlightValue > _photoSensorThreshold)
      {
          _darkLight = 1;
      }
      
      if(sunlightValue < (_photoSensorThreshold-100))
      {
          _darkLight = 0;
          digitalWrite(_pinRelay, 0);
      }

      if(_darkLight == 1)
      {
          digitalWrite(_pinRelay, _lightOn);
      }
    }
    else
    {
      digitalWrite(_pinRelay, _lightOn);
    }
}

bool Room::getLightRoom()
{
    if(_darkLight == 1)
    {
         return _lightOn;
    }
    else
    {
        return 0;
    }
    
}