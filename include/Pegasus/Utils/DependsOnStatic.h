/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	DependsOnStatic.h
//! \author	Karolyn Boulanger
//! \date	23rd January 2015
//! \brief	Base class to declare the dependency to a static class

#ifndef PEGASUS_UTILS_DEPENDSONSTATIC_H
#define PEGASUS_UTILS_DEPENDSONSTATIC_H

namespace Pegasus {
namespace Utils {


//! Base class to declare the dependency on a static instance of a class.
//! This typically happens when the static instance of a class requires to use
//! the static instance of another class as its initializer or in its constructor.
//! Example:
//! \code
//! // In class2.h
//! class Class2
//! {
//! public:
//!     static Class2 & GetInstance()
//!     {
//!         static Class2 sInstance;
//!         return sInstance;
//!     }
//!     ...
//! }
//!
//! // In class1.cpp, a static instance of Class1 is declared and depends on Class2
//! Class1 sInstance1(Class2::GetInstance());
//!
//! // In class1.h, the dependency should be declared
//! class Class1 : protected DependsOnStatic<Class2, Class2::GetInstance>
//! {
//! public:
//!     Class1();
//!     ~Class1();
//!     ...
//! }
//!
//! // In class1.cpp, it is now safe to use Class2::GetInstance() in the constructor
//! // and in the destructor
//! Class1::Class1()
//! {
//!     // safe here
//! }
//!
//! Class1::~Class1()
//! {
//!     // safe here
//! }
//! \endcode
//! \param T Name of the class to depend on
//! \param F Function that returns a reference to the instance of T
template <typename T, T & F()>
class DependsOnStatic
{
protected:
	DependsOnStatic()
	{
		F();
	}
};

//! Base class to declare the dependency on a static instance of a class.
//! \note This is the same class as DependsOnStatic, except that the function F
//!       returns a reference to the constant instance of T
//! \sa DependsOnStatic
template <typename T, T const & F()>
class DependsOnConstStatic
{
protected:
	DependsOnConstStatic()
	{
		F();
	}
};


}   // namespace Utils
}   // namespace Pegasus

#endif	// PEGASUS_UTILS_DEPENDSONSTATIC_H
