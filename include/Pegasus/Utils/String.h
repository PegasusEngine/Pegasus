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
}
}

#endif //PEGASUS_STRING_H
