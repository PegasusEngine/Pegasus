/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	String.cpp
//! \author	Kleber Garcia
//! \date	3rd March 2014
//! \brief	String utilities

#include "Pegasus/Utils/String.h"

int Pegasus::Utils::Strcmp(const char * str1, const char * str2)
{
    while(*str1 && *str2)
    {
        int c = *str1++ - *str2++; 
        if (c != 0)
        {
            return c;
        }
    }
    return *str1 ? -*str1 : (*str2 ? *str2 : 0 );
}


int Pegasus::Utils::Stricmp(const char * str1, const char * str2)
{
    while(*str1 && *str2)
    {
        char c1  = *str1++;
        char c2  = *str2++; 
        c1 = c1 >= 'A' && c1 <= 'Z' ? c1 - 'A' + 'a' : c1;
        c2 = c2 >= 'A' && c2 <= 'Z' ? c2 - 'A' + 'a' : c2;
        int c = c1 - c2;
        if (c != 0)
        {
            return c;
        }
    }
    return *str1 ? -*str1 : (*str2 ? *str2 : 0 );
}

unsigned Pegasus::Utils::Strlen(const char * str)
{
    int counter = 0;
    for (; *str; ++str, ++counter);
    return counter;
}

const char * Pegasus::Utils::Strchr ( const char * str, char character)
{
    do 
    {
        if (*str == character) return str;
    } while(*(str++));
    return nullptr;
}

const char * Pegasus::Utils::Strrchr (const char * str, char character)
{
    int len = Pegasus::Utils::Strlen(str);
    while (len >= 0)
    {
        if (str[len] == character) return str + len;
        --len;
    }
    return nullptr;
}

char * Pegasus::Utils::Strchr (char * str, char character)
{
    do 
    {
        if (*str == character) return str;
    } while(*(str++));
    return nullptr;
}

char * Pegasus::Utils::Strrchr (char * str, char character)
{
    int len = Pegasus::Utils::Strlen(str);
    while (len >= 0)
    {
        if (str[len + 1] == character) return str + len + 1;
        --len;
    }
    return nullptr;
}
