/* Библиотека для автоматического управления светом с подсчетом количества людей в комнате
 * Сайт проекта: https://edu.labrazum.ru/people_counter
 * Свежая версия на GitHub: https://github.com/viktor-osin/PeopleCounter
*/
#include <Arduino.h>
#include "LabRazum_PeopleCounter.h"

Room toilet(1, 2, 0);
/*  Аргументы:
 *  (ИК-датчик1, ИК-датчик2, реле(PIN), кнопка "ручной режим"(PIN), фоторезистор(аналог-й PIN без буквы А), порог фоторезистора)
 *  не обязательно указывать все аргументы:
 *  если вы не хотите ставить кнопку руч. реж., но хотите фоторезистор - подключите к соответствующему пину кнопки +5В
 *  и система будет постоянно в автоматическом режиме
 *  если вы не хотите ставить фоторезистор - не вводите два последних аргумента
 *  если вы не хотите ставить и кнопку, и фоторезистор - не вводите последние три аргумента 
 */
void setup() 
{
  toilet.begin();
  
  //hall.debug(1);  //режим отладки - раскомментируй и в Serial-порт будут выводиться показания датчиков и др. информация 
}
void loop() 
{
  toilet.lightControl();

  //ПРИМЕЧАНИЕ: Для быстродействия системы, любой вывод в Serial-порт используйте только при отладке! 
  
  //Serial.print("Hall:" + String(hall.getCountPeople()));          //метод возвращает количество человек в этой комнате
  //Serial.println(" Hall Light:" + String(hall.getLightRoom()));   //метод возвращает 1 - свет вкл, 0 - выкл в этой комнате
}
