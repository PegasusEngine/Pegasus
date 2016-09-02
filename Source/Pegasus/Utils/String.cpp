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

char * Pegasus::Utils::Strcat(char * dst, const char * src)
{
    int len = Pegasus::Utils::Strlen(dst);
    const char * srcPtr = src;
    char * dstPtr = dst + len;
    *dstPtr = *srcPtr;
    if (*srcPtr != 0) do
    {
        *(++dstPtr) = *(++srcPtr);
    } while(*srcPtr != 0);
    return dst;
}

char * Pegasus::Utils::Strcat(char * dst, int i)
{
    int len = Pegasus::Utils::Strlen(dst);
    char * dstPtr = dst + len;

    if (i == 0)
    {
        dstPtr[0] = '0'; dstPtr[1] = '\0';
    }
    else
    {
        if (i < 0) *dstPtr++ = '-';
        char * beginning = dstPtr;
        while (i)
        {
            *dstPtr++ = (i % 10) + '0';
            i /= 10;
        }
        char * end = dstPtr - 1;
        //flip
        while (beginning < end)
        {
            char tmp = *beginning;
            *beginning++ = *end;
            *end-- = tmp;
        }
        *dstPtr = 0;
    }
    return dst;
}

int Pegasus::Utils::Atoi(const char * str)
{
    if (*str != '\0')
    {
        int sign = *str == '-' ? -1 : 1;
        if (sign < 0) ++str;
        int val = 0;
        while (*str && *str >= '0' && *str <= '9')
        {
            val = 10*val + (*str - '0');
            ++str;
        }
        return sign * val;
    }
    return 0;
}

float Pegasus::Utils::Atof(const char * str)
{
    const char * p = str;
    float sign = 1.0f;
    if (*str == '-')
    {
        sign = -1.0f;
        ++p;
    }
    float f = 0.0;
    float r = 10.0;
    bool pastPoint = false;
    
    while (*p != '\0')
    {
        if (*p == '.')
        {
            pastPoint = true;
        }
        else
        {
            float n = static_cast<float>(*p - '0');
            f = (pastPoint ? (n / r) : n ) + (pastPoint ? f : (f * 10.0f));
            r = pastPoint ? r * 10.0f : r;
        }
		++p;
    }

    return f * sign;
}

unsigned int Pegasus::Utils::HashStr(const char* str)
{
    //! djb2
    //! Source: http://www.cse.yorku.ca/~oz/hash.html
    //! this algorithm (k=33) was first reported by dan bernstein many years ago in comp.lang.c. another version of this algorithm (now favored by bernstein) uses xor: hash(i) = hash(i - 1) * 33 ^ str[i]; the magic of number 33 (why it works better than many other constants, prime or not) has never been adequately explained.
    unsigned int hash = 5381;
    int c;
    while (c = *str++)
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

