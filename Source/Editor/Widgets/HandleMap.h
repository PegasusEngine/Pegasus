/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	HandleMap.h
//! \author	Kleber Garcia
//! \date	30th December, 2015
//! \brief	Efficient hashmap, that maps a certain handle to a piece of memory.
//!         the random access is O(1), insertion and deletion is O(n).
//!         this data structure is perfect for things that get inserted / removed 
//!         not that frequently, but retrieved very frequently.
//!         HandleType must have the function InternalValue with the following signature:
//!         int InternalValue() const;

#include <QVector>
#include "Assertion.h"
#ifndef HANDLE_MAP_H
#define HANDLE_MAP_H

template <typename HandleType, typename DataType>
class HandleMap
{

public:

    //! Constructor
    HandleMap() {}

    //! Destructor
    ~HandleMap() {}

    //! Gets the data type. If its not found this will assert.
    //! Always use Has() prior to calling this.
    //! \param h the handle.
    //! \return the data that this handle represents
    DataType& Get(const HandleType& h);

    
    //! O(1) function.
    //! \param h the handle.
    //! \return true if this handle is valid and does have data. False otherwise
    bool Has(const HandleType& h) const;

    //! O(n) function. inserts a new piece of data. Returns the handle represented.
    //! \param d data to insert.
    //! \return the handle representing this type.
    HandleType Insert(DataType& d);

    //! removes the data represented by this handle.
    //! \param h handle of data to be removed
    void Remove(HandleType& h);
    

private:
    struct DataState
    {
        bool isValid;
        DataType d;
        DataState() : isValid(false) {}
    };

    QVector<DataState> mSet;
};

template<typename HandleType, typename DataType>
DataType& HandleMap<HandleType,DataType>::Get(const HandleType& h)
{
    DataState& ds = mSet[h.InternalValue()];
    return ds.d;
}

template<typename HandleType, typename DataType>
bool HandleMap<HandleType,DataType>::Has(const HandleType& h) const
{
    return h.InternalValue() >= 0 && h.InternalValue() < mSet.size() && mSet[h.InternalValue()].isValid;
}

template<typename HandleType, typename DataType>
HandleType HandleMap<HandleType,DataType>::Insert(DataType& d)
{
    //has this one been inserted?
    for (int i = 0; i < mSet.size(); ++i)
    {
        if (mSet[i].isValid && d == mSet[i].d)
        {
            return HandleType(i);
        }
    }
    
    //first find empty space
    for (int i = 0; i < mSet.size(); ++i)
    {
        if (!mSet[i].isValid)
        {
            mSet[i].isValid = true;
            mSet[i].d = d;
            return HandleType(i);
        }
    }

    DataState ds;
    ds.isValid = true;
    ds.d = d;
    mSet.push_back(ds);
    return HandleType(mSet.size() - 1);
}

template<typename HandleType, typename DataType>
void HandleMap<HandleType,DataType>::Remove(HandleType& h)
{
    mSet[h.InternalValue()].isValid = false;
}

#endif