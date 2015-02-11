/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ASTree.h
//! \author Kleber Garcia
//! \date   8th February 2015
//! \brief  Asset Script Tree

#ifndef  PEGASUS_ASTREE_H
#define PEGASUS_ASTREE_H

#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{
    namespace Alloc {
        class IAllocator;
    }
};

namespace Pegasus
{
namespace AssetLib
{
    class Array;
    class Object;

    template <typename T>
    struct Touple
    {
        T mEl;
        const char* mName;
    };


    class Object
    {
    public: 
        explicit Object(Alloc::IAllocator* alloc);     
        ~Object();

        int         GetIntCount() const { return mInts.GetSize(); }
        int         GetInt(int i) const;
        int         FindInt(const char* name) const;
        const char* GetIntName(int i) const;
        void        AddInt(const char* name, int i);

        int         GetFloatCount() const { return mFloats.GetSize(); }
        float       GetFloat(int i) const;
        int         FindFloat(const char* name) const;
        const char* GetFloatName(int i) const; 
        void        AddFloat(const char* name, float f);

        int         GetStringCount() const { return mStrings.GetSize(); }
        const char* GetString(int i) const;
        int         FindString(const char* name) const;
        const char* GetStringName(int i) const;
        void        AddString(const char* name, const char* string);

        int         GetObjectCount() const { return mObjects.GetSize(); }
        Object*     GetObject(int i) const;
        int         FindObject(const char* name) const;
        const char* GetObjectName(int i) const;
        void        AddObject(const char* name, Object* o);

        int         GetArrayCount() const { return mArrays.GetSize(); }
        Array*      GetArray(int i) const;
        int         FindArray(const char* name) const;
        const char* GetArrayName(int i) const;
        void        AddArray(const char* name, Array* arr);

    private:
        Utils::Vector<Touple<int> >          mInts;
        Utils::Vector<Touple<float> >        mFloats;
        Utils::Vector<Touple<const char*> >  mStrings;
        Utils::Vector<Touple<Object*> >      mObjects;
        Utils::Vector<Touple<Array*> >       mArrays;
        
    };

    class Array
    {
    public:
        enum Type
        {
            AS_TYPE_NULL,
            AS_TYPE_STRING,
            AS_TYPE_INT,
            AS_TYPE_FLOAT,
            AS_TYPE_OBJECT,
            AS_TYPE_ARRAY
        };

        union Element
        {
            int         i;
            float       f;
            Array*      a;
            Object*     o;
            const char* s;
        };

        explicit Array(Alloc::IAllocator* alloc);
        ~Array();
        void CommitType(Type t); 
        int GetSize() const { return mElements.GetSize(); }
        Type GetType() const { return mType; }
        const Element& GetElement(int i) const;
        void     PushElement(Element& el);

    private:
        Type mType;
        Utils::Vector<Element> mElements;
    };

    struct VariantType
    {
        Array::Element v;
        Array::Type mType;
    };
}
}

#endif
