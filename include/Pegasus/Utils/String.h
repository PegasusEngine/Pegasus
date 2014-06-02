/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	String.h
//! \author	Kleber Garcia
//! \date	3rd Match 2014
//! \brief  String utilities	

#ifndef PEGASUS_STRING_H
#define PEGASUS_STRING_H

namespace Pegasus
{
namespace Utils
{
    //! Standard unsafe 8bit ascii null terminated string case sensitive comparison function
    int Strcmp(const char * str1, const char * str2);  

    //! Standard unsafe 8bit ascii null terminated string case insensitive comparison function
    int Stricmp(const char * str1, const char * str2);

    //! Standard unsafe 8bit ascii null terminated string size function
    unsigned Strlen(const char * str);

    //! Standard unsafe 8bit ascii Strchr function
    const char * Strchr ( const char * str, char character);

    //! Standard unsafe 8bit ascii Strrchr function
    const char * Strrchr (const char * str, char character);

    //! Standard unsafe 8bit ascii Strchr function
    char * Strchr (char * str, char character);

    //! Standard unsafe 8bit ascii Strrchr function
    char * Strrchr (char * str, char character);

    //! standard, unsafe string concatenation function
    char * Strcat(char * dst, const char * src);

    //! standard, unsafe string concatenation function
    char * Strcat(char * dst, int i);

    //! convert a string to int
    int Atoi(const char * str);
    
}
}

#endif //PEGASUS_STRING_H
