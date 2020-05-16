#include <LabRazum_PeopleCounter.h>
#include <Arduino.h> 

Room::Room(byte pinSensor1, byte pinSensor2, byte pinRelay)
{   
    _pinSensor1 = pinSensor1;
    _pinSensor2 = pinSensor2;
    _pinRelay = pinRelay;
}


Room::Room(byte pinSensor1, byte pinSensor2, byte pinRelay, byte pinHandMode)
{   
    #define HAND_MODE_BUTTON_CONNECT
    _pinSensor1 = pinSensor1;
    _pinSensor2 = pinSensor2;
    _pinHandMode = pinHandMode;
    _pinRelay = pinRelay;
}


Room::Room(byte pinSensor1, byte pinSensor2, byte pinRelay, byte pinHandMode, byte pinPhotoSensor, int photoSensorThreshold)
{  
    #define PHOTO_SENSOR_CONNECT
    #define HAND_MODE_BUTTON_CONNECT 
    _pinSensor1 = pinSensor1;
    _pinSensor2 = pinSensor2;
    _pinHandMode = pinHandMode;
    _pinRelay = pinRelay;
    _pinPhotoSensor = pinPhotoSensor;
    _photoSensorThreshold = photoSensorThreshold;
}

void Room::begin()
{   
    pinMode(_pinSensor1, INPUT);
    pinMode(_pinSensor2, INPUT);
    pinMode(_pinRelay, OUTPUT);
    digitalWrite(_pinRelay, LOW);

    pinMode(_pinHandMode, INPUT);
    pinMode(_pinPhotoSensor, INPUT);
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
  if(_onDebug == 1)
  {
    Serial.print("IR_sensor_1:" + String(_IRsensor1) + " IR_sensor_2:" + String(_IRsensor2));
  }
  
  //определяем, какой датчик сработал первым
  if((_firstSensor2 == 0) && (_firstSensor1 == 0))
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
        }
      }
      if(_firstSensor2)
      {
        if((_IRsensor1 == 0) && (_valuePeople > 0))
        {
          _valuePeople--;
          _firstSensor2 = 0;
        }
      }
    }
    else
    {
       _firstSensor1 = 0;
       _firstSensor2 = 0;
    }         
  }
}

void Room::start_timer_people()
{
    _oldTimerPeople = millis();
    _timerPeople = 0;
}

byte Room::lightControl()
{
    #ifdef HAND_MODE_BUTTON_CONNECT
    if(digitalRead(_pinHandMode) == 0) //если с кнопки пришел 0, включаем "ручной" режим
    {
        lightRoom(0);     //выключить свет в этой комнате
        _valuePeople = 0; //обнулить счетчик людей
    } 
    #endif

    #ifdef HAND_MODE_BUTTON_CONNECT
    if(digitalRead(_pinHandMode) == 1) //если с кнопки пришла 1, включаем автоматический режим
    {
    #endif
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
    #ifdef HAND_MODE_BUTTON_CONNECT
    }   
    #endif
}

byte Room::getCountPeople()
{
    return _valuePeople;
}


void Room::lightRoom(bool lightOn)
{
    _lightOn = lightOn;
    #ifdef PHOTO_SENSOR_CONNECT
    int sunlightValue = analogRead(_pinPhotoSensor);

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
    #endif
    #ifndef PHOTO_SENSOR_CONNECT
         digitalWrite(_pinRelay, _lightOn);
    #endif
    if(_onDebug == 1)
    {
      Serial.print(" PhotoSensor:" + String(sunlightValue));
      if(_darkLight == 1)
          Serial.println(" Light in Room: "+ String(_lightOn));
      else
          Serial.println(" Light in Room: 0");
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