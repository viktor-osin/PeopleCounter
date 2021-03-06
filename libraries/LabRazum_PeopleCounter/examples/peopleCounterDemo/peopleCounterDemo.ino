/* Библиотека для автоматического управления светом с подсчетом количества людей в комнате
 * Сайт проекта: https://edu.labrazum.ru/people-counter
 * Свежая версия на GitHub: https://github.com/viktor-osin/PeopleCounter
*/
#include "LabRazum_PeopleCounter.h"

Room hall(2, 3, 4);
/*  Аргументы:
 *  (ИК-датчик1, ИК-датчик2, реле(PIN), кнопка "ручной режим"(PIN), фоторезистор(аналог-й PIN без буквы А), порог фоторезистора)
 *  не обязательно указывать все аргументы:
 *  если вы не хотите ставить кнопку руч. реж., но хотите фоторезистор - подключите к соответствующему пину кнопки +5В
 *  и система будет постоянно в автоматическом режиме
 *  если вы не хотите ставить фоторезистор - не вводите два последних аргумента
 *  если вы не хотите ставить и кнопку, и фоторезистор - не вводите последние три аргумента 
 */
Room kitchen(6, 7, 8, 9); //кухня - работает без использования фоторезистора но с ручным режимом
Room toilet(10, 11, 12);  //туалет - работает всегда в автоматическом режиме без фоторезистора

void setup() 
{
  hall.begin();     //инициализация PIN-ов
  kitchen.begin();
  toilet.begin();
  //hall.debug(1);  //режим отладки - раскомментируй и в Serial-порт будут выводиться показания датчиков и др. информация 
  Serial.begin(9600);
}
void loop() 
{
  hall.lightControl();      //контроль освещения в комнате (должен быть всегда в loop)
  kitchen.lightControl();
  toilet.lightControl();

  //ПРИМЕЧАНИЕ: Для быстродействия системы, любой вывод в Serial-порт используйте только при отладке! 
  
  //Serial.print("Hall:" + String(hall.getCountPeople()));          //метод возвращает количество человек в этой комнате
  //Serial.println(" Hall Light:" + String(hall.getLightRoom()));   //метод возвращает 1 - свет вкл, 0 - выкл в этой комнате
}
