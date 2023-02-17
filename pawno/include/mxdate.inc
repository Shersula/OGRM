#include <a_samp>
#include <sscanf2>
 
#define SPLITTER .
 
new MonthTimes[12][4] = 
{
    { 31, 31, 2678400, 2678400 },
    { 28, 29, 2419200, 2505600 },
    { 31, 31, 2678400, 2678400 },
    { 30, 30, 2592000, 2592000 },
    { 31, 31, 2678400, 2678400 },
    { 30, 30, 2592000, 2592000 },
    { 31, 31, 2678400, 2678400 },
    { 31, 31, 2678400, 2678400 },
    { 30, 30, 2592000, 2592000 },
    { 31, 31, 2678400, 2678400 },
    { 30, 30, 2592000, 2592000 },
    { 31, 31, 2678400, 2678400 }
};
 
stock IsLeapYear(year)
{
    if(year % 4 == 0) return 1;
    else return 0;
}
 
stock TimestampToDate(Timestamp, &year, &month, &day, &hour, &minute, &second, HourGMT, MinuteGMT = 0)
{
    new tmp = 2;
    year = 1970;
    month = 1;
    Timestamp -= 172800; // Delete two days from the current timestamp. This is necessary, because the timestamp retrieved using gettime() includes two too many days.
    for(;;)
    {
        if(Timestamp >= 31536000)
        {
            year ++;
            Timestamp -= 31536000;
            tmp ++;
            if(tmp == 4)
            {
                if(Timestamp >= 31622400)
                {
                    tmp = 0;
                    year ++;
                    Timestamp -= 31622400;
                }
                else break;
            }
        }
        else break;
    }       
    for(new i = 0; i < 12; i ++)
    {
        if(Timestamp >= MonthTimes[i][2 + IsLeapYear(year)])
        {
            month ++;
            Timestamp -= MonthTimes[i][2 + IsLeapYear(year)];
        }
        else break;
    }
    day = 1 + (Timestamp / 86400);
    Timestamp %= 86400;
    hour = HourGMT + (Timestamp / 3600);
    Timestamp %= 3600;
    minute = MinuteGMT + (Timestamp / 60);
    second = (Timestamp % 60);
    if(minute > 59)
    {
        minute = 0;
        hour ++;
    }
    if(hour > 23)
    {
        hour -= 24;
        day ++;
    }   
    if(day > MonthTimes[month][IsLeapYear(year)])
    {
        day = 1;
        month ++;
    }
    if(month > 12)
    {
        month = 1;
        year ++;
    }
    return 1;
}
 
stock DateToTimestamp(str[11])
{
    new date[3]; // date[0] = day       date[1] = month         date[2] = year
    if(!sscanf(str,"p<"#SPLITTER">ddd",date[0],date[1],date[2]))
    {
        new total = 0, tmp = 0;
        total += date[0] * 86400;
        if(date[1] == 2 && date[0] < 29) tmp = ((date[2] - 1968) / 4 - 2);
        else tmp = ((date[2] - 1968) / 4 - 1);
        total += tmp * 31622400;
        total += (date[2] - 1970 - tmp) * 31536000;
        for(new i = 1; i < date[1]; i ++) total += MonthTimes[i][0 + IsLeapYear(date[2])] * 86400;
        return total;
    }
    else return -1;
}

/*
	Заменяет в указанной строке условные обозначения времени на их значения
	в соответствии с указанным UNIX TIMESTAMP

	ПОДРОБНЕЕ
		UNIX TIMESTAMP это кол-во секунд со дня 01.01.1970, 00:00:00 до
		указанной в параметрах даты. Если он правильно указан, то в
		строке format все условные обозначения времени будут заменены
		на их значения в соответствии с указанным UNIX TIMESTAMP.

		Функция ищет:	Заменяет на:
		%yyyy			четырехзначное значение года
		%yy			двузначное значение года с ведущим нулём
		%m			значение месяца
		%mm			значение месяца с ведущим нулём
		%d			значение дня
		%dd			значение дня с ведущим нулём
		%h			значение часа
		%hh			значение часа с ведущим нулём
		%i			значение минуты
		%ii			значение минуты с ведущим нулём
		%s			значение секунды
		%ss			значение секунды с ведущим нулём

	ВЕРНЕТ
		строку с численными значениями даты вместо их псевдонимов
		размер строки = sizeOfOutput
*/
stock date (timestamp = 0, GMT = 0, const formatStr[] = "%dd.%mm.%yyyy, %hh:%ii:%ss")
{
	// ------------
	const sizeOfOutput = 128; // размер возвращаемой строки

	new yyyy, mm, dd, h, m, s,
		pos, foundPos, searchStartPos, outStrLen,
		tmpNumStr[5], outStr[sizeOfOutput];

	TimestampToDate(timestamp, yyyy, mm, dd, h, m, s, GMT);// превратим штамп в дату
	// ------------


	// -------------------
	// скопируем formatStr в outStr
	memcpy( outStr, formatStr, 0, (sizeOfOutput - 1)*4 );
	outStr[sizeOfOutput - 1] = 0; // правильный обрез строки в целях безопасности

	outStrLen = strlen(outStr);
	// -------------------


	// YEAR
	// -------------------
	searchStartPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%yyyy", false, searchStartPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 5, "%04d", yyyy ); // tmpNumStr = строкое значение yyyy

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 4; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		for ( pos += foundPos; pos < outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];

		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		searchStartPos = foundPos + 4; // позиция начала след. поиска += 4

		if ( searchStartPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%yyyy", false, searchStartPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// -------------------
	searchStartPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%yy", false, searchStartPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 5, "%04d", yyyy ); // tmpNumStr = строкое значение yyyy

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos + 2];

		// сдвиг назад на 1 дальнейшего контента строки
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];

		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		searchStartPos = foundPos + 2; // позиция начала след. поиска изменилась

		if ( searchStartPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%yy", false, searchStartPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// MONTH
	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%mm", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", mm ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];

		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%mm", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%m", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", mm ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		if ( mm < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];

			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%m", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// DAY
	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%dd", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", dd ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];

		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%dd", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%d", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", dd ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		if ( dd < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];

			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%d", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// HOUR
	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%hh", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", h ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];

		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%hh", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%h", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", h ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		if ( h < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];

			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%h", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// MINUTE
	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%ii", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", m ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];

		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%ii", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%i", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", m ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		if ( m < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];

			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%i", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// SECOND
	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%ss", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%02d", s ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		for ( pos += foundPos; pos <= outStrLen; pos++ )
			outStr[pos] = outStr[pos + 1];

		outStr[pos] = 0; // правильный обрез строки
		outStrLen = strlen(outStr); // размер строки изменился
		foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%ss", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	// -------------------
	foundPos = 0; // поз начала поиска = 0
	foundPos = strfind( outStr, "%s", false, foundPos ); // ищем первую позицию

	while ( foundPos != -1 )
	{
		format( tmpNumStr, 3, "%d", s ); // tmpNumStr = строкое значение числа

		for ( pos = 0; tmpNumStr[pos] != 0 && pos < 2; pos++ ) // замена псевдонима на значение
			outStr[foundPos + pos] = tmpNumStr[pos];

		// сдвиг назад на 1 дальнейшего контента строки
		if ( s < 10 )
		{
			for ( pos += foundPos; pos <= outStrLen; pos++ )
				outStr[pos] = outStr[pos + 1];

			outStr[pos] = 0; // правильный обрез строки
			outStrLen = strlen(outStr); // размер строки изменился
			foundPos++; // позиция начала след. поиска изменилась
		}
		else
			foundPos += 2; // позиция начала след. поиска изменилась

		if ( foundPos < outStrLen ) // если поз начала поиска меньше длины строки
			foundPos = strfind( outStr, "%s", false, foundPos ); // продолжим поиск
		else break; // иначе - остановим поиск и замену
	}
	// -------------------


	return outStr;
}
