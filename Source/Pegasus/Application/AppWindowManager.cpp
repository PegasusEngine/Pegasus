/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AppWindowManager.cpp
//! \author David Worsham
//! \date   16th October 2013
//! \brief  Window manager for a Pegasus app.

#include "Pegasus/Application/AppWindowManager.h"
#include <string.h>

namespace Pegasus {
namespace Application {

//! Window type table entry
struct TypeTableEntry
{
public:
    // Ctor / dtor
    TypeTableEntry();
    TypeTableEntry(const TypeTableEntry& other);
    TypeTableEntry& operator=(const TypeTableEntry& other);
    ~TypeTableEntry();

    // Helpers
    void Clear();
    inline bool TypeEqual(const TypeTableEntry& other) { return (mTypeTag == other.mTypeTag); }
    inline bool ClassEqual(const TypeTableEntry& other) { return (strcmp(mClassName, other.mClassName) == 0); }
    inline bool Empty() { return (mClassName[0] == '\0'); }

    WindowTypeTag mTypeTag;
    char mDescription[AppWindowManager::APPWINDOW_DESC_LENGTH]; // Human-friendly description
    char mClassName[AppWindowManager::APPWINDOW_CLASS_LENGTH]; // Class name
    Window::WindowFactoryFunc mCreateFunc; // Factory function
};

//----------------------------------------------------------------------------------------

//! Window type table
struct TypeTable
{
public:
    // Ctor / dtor
    TypeTable(unsigned int max, Memory::IAllocator* alloc);
    ~TypeTable();

    // Helpers
    bool Contains(const char* typeName) const;
    bool Contains(WindowTypeTag typeTag) const;
    TypeTableEntry* Get(const char* typeName) const;
    void Insert(const TypeTableEntry& entry);
    void Remove(const char* typeName);


    Memory::IAllocator* mAllocator; //! Allocator to use when creating this object
    TypeTableEntry* mTable; // Table members
    unsigned int mCurrentSize; // Current table size
    unsigned int mMaxSize; // Maximum table size
    int mMainWindowIndex; // Index for MAIN window type
};

//----------------------------------------------------------------------------------------

//! Window table
struct WindowTable
{
public:
    // ctor / dtor
    WindowTable(unsigned int max, Memory::IAllocator* alloc);
    ~WindowTable();

    // Helpers
    bool Contains(const Window::Window* entry) const;
    void Insert(Window::Window* entry);
    void Remove(Window::Window* entry);


    Memory::IAllocator* mAllocator; //! Allocator to use when creating this object
    Window::Window** mTable; // Table members
    unsigned int mCurrentSize; // Table size
    unsigned int mMaxSize; // Max table size
};


//----------------------------------------------------------------------------------------

TypeTableEntry::TypeTableEntry()
    : mTypeTag(WINDOW_TYPE_INVALID),
      mCreateFunc(nullptr)
{
    mDescription[0] = '\0';
    mClassName[0] = '\0';
}

//----------------------------------------------------------------------------------------

TypeTableEntry::~TypeTableEntry()
{
}

//----------------------------------------------------------------------------------------

TypeTableEntry::TypeTableEntry(const TypeTableEntry& other)
{
    if (&other != this)
    {
        mTypeTag = other.mTypeTag;
        mCreateFunc = other.mCreateFunc;
        strncpy_s(mDescription, other.mDescription, AppWindowManager::APPWINDOW_DESC_LENGTH);
        mDescription[AppWindowManager::APPWINDOW_DESC_LENGTH - 1] = '\0';
        strncpy_s(mClassName, other.mClassName, AppWindowManager::APPWINDOW_CLASS_LENGTH);
        mDescription[AppWindowManager::APPWINDOW_CLASS_LENGTH - 1] = '\0';
    }
}

//----------------------------------------------------------------------------------------

TypeTableEntry& TypeTableEntry::operator=(const TypeTableEntry& other)
{
    if (&other != this)
    {
        mTypeTag = other.mTypeTag;
        mCreateFunc = other.mCreateFunc;
        strncpy_s(mDescription, other.mDescription, AppWindowManager::APPWINDOW_DESC_LENGTH);
        mDescription[AppWindowManager::APPWINDOW_DESC_LENGTH - 1] = '\0';
        strncpy_s(mClassName, other.mClassName, AppWindowManager::APPWINDOW_CLASS_LENGTH);
        mDescription[AppWindowManager::APPWINDOW_CLASS_LENGTH - 1] = '\0';
    }

    return *this;
}

//----------------------------------------------------------------------------------------

void TypeTableEntry::Clear()
{
    mTypeTag = WINDOW_TYPE_INVALID;
    mDescription[0] = '\0';
    mClassName[0] = '\0';
    mCreateFunc = nullptr;
}

//----------------------------------------------------------------------------------------

TypeTable::TypeTable(unsigned int max, Memory::IAllocator* alloc)
    : mAllocator(alloc), mCurrentSize(0), mMaxSize(max), mMainWindowIndex(-1)
{
    mTable = PG_NEW_ARRAY(mAllocator, "TypeTableEntries", Pegasus::Memory::PG_MEM_PERM, TypeTableEntry, mMaxSize);
}

//----------------------------------------------------------------------------------------

TypeTable::~TypeTable()
{
    PG_ASSERTSTR(mCurrentSize == 0, "Type table not empty upon destruction!");

    PG_DELETE_ARRAY(mAllocator, mTable);
}

//----------------------------------------------------------------------------------------

bool TypeTable::Contains(const char* typeName) const
{
    bool found = false;

    // Iterate over table
    for (unsigned int i = 0; i < mCurrentSize; i++)
    {
        if (strcmp(typeName, mTable[i].mClassName) == 0)
        {
            found = true;
            break;
        }
    }

    return found;
}

//----------------------------------------------------------------------------------------

bool TypeTable::Contains(WindowTypeTag typeTag) const
{
    bool found = false;

    // Iterate over table
    for (unsigned int i = 0; i < mCurrentSize; i++)
    {
        if (typeTag == mTable[i].mTypeTag)
        {
            found = true;
            break;
        }
    }

    return found;
}

//----------------------------------------------------------------------------------------

TypeTableEntry* TypeTable::Get(const char* typeName) const
{
    int index = -1;

    // Iterate over table
    for (unsigned int i = 0; i < mCurrentSize; i++)
    {
        if (strcmp(typeName, mTable[i].mClassName) == 0)
        {
            index = i;
            break;
        }
    }

    // Return suitable entry
    if (index < 0)
    {
        return nullptr;
    }
    else
    {
        return (mTable + index);
    }
}

//----------------------------------------------------------------------------------------

void TypeTable::Insert(const TypeTableEntry& entry)
{
    PG_ASSERTSTR(!Contains(entry.mClassName), "Type table already contains this class type!");
    PG_ASSERTSTR(mCurrentSize < mMaxSize, "Type table is full!");

    if (entry.mTypeTag == WINDOW_TYPE_MAIN)
    {
        PG_ASSERTSTR(!Contains(WINDOW_TYPE_MAIN), "Type table already contains a MAIN window!");

        mMainWindowIndex = (int) mCurrentSize;
    }
    mTable[mCurrentSize++] = entry;
}

//----------------------------------------------------------------------------------------

void TypeTable::Remove(const char* typeName)
{
    int index = -1;

    // Iterate over table
    for (unsigned int i = 0; i < mCurrentSize; i++)
    {
        if (strcmp(typeName, mTable[i].mClassName) == 0)
        {
            index = i;
            break;
        }
    }

    PG_ASSERTSTR(index != -1, "Trying to remove unknown window type!");
    // Check for MAIN
    if (mTable[index].mTypeTag == WINDOW_TYPE_MAIN)
    {
        mMainWindowIndex = -1;
    }
    memmove(mTable + index, mTable + index + 1, sizeof(TypeTableEntry) * (mCurrentSize - index - 1)); // Fill hole
    mCurrentSize--;
}

//----------------------------------------------------------------------------------------

WindowTable::WindowTable(unsigned int max, Memory::IAllocator* alloc)
: mAllocator(alloc), mCurrentSize(0), mMaxSize(max)
{
    mTable = PG_NEW_ARRAY(mAllocator, "WindowTableEntries", Pegasus::Memory::PG_MEM_PERM, Window::Window*, mMaxSize);

    // Zero out window table
    for (unsigned int i = 0; i < mMaxSize; i++)
    {
        mTable[i] = nullptr;
    }
}

//----------------------------------------------------------------------------------------

WindowTable::~WindowTable()
{
    PG_ASSERTSTR(mCurrentSize == 0, "Window table not empty upon destruction!");

    PG_DELETE_ARRAY(mAllocator, mTable);
}

//----------------------------------------------------------------------------------------

bool WindowTable::Contains(const Window::Window* entry) const
{
    bool found = false;

    // Iterate over table
    for (unsigned int i = 0; i < mCurrentSize; i++)
    {
        if (entry == mTable[i])
        {
            found = true;
            break;
        }
    }

    return found;
}

//----------------------------------------------------------------------------------------

void WindowTable::Insert(Window::Window* entry)
{
    PG_ASSERTSTR(mCurrentSize < mMaxSize, "Window table is full!");

    mTable[mCurrentSize++] = entry;
}

//----------------------------------------------------------------------------------------

void WindowTable::Remove(Window::Window* entry)
{
    int index = -1;

    // Iterate over table
    for (unsigned int i = 0; i < mCurrentSize; i++)
    {
        if (entry == mTable[i])
        {
            index = i;
            break;
        }
    }

    PG_ASSERTSTR(index != -1, "Trying to remove unknown window!");
    PG_DELETE(mAllocator, mTable[index]);
    memmove(mTable + index, mTable + index + 1, sizeof(Window::Window*) * (mCurrentSize - index - 1)); // Fill hole
    mCurrentSize--;
}

//----------------------------------------------------------------------------------------

AppWindowManager::AppWindowManager(const AppWindowManagerConfig& config)
    : mAllocator(config.mAllocator)
{
    mTypeTable = PG_NEW(mAllocator, "WindowManager-TypeTable", Pegasus::Memory::PG_MEM_PERM) TypeTable(config.mMaxWindowTypes, mAllocator);
    mWindowTable = PG_NEW(mAllocator, "WindowManager-TypeTable", Pegasus::Memory::PG_MEM_PERM) WindowTable(config.mMaxNumWindows, mAllocator);
}

//----------------------------------------------------------------------------------------

AppWindowManager::~AppWindowManager()
{
    // Clean up tables
    PG_DELETE(mAllocator, mWindowTable);
    PG_DELETE(mAllocator, mTypeTable);
}

//----------------------------------------------------------------------------------------

void AppWindowManager::RegisterWindowClass(const char* className, const WindowRegistration& classReg)
{
    TypeTableEntry entry;

    // Set up and insert
    entry.mTypeTag = classReg.mTypeTag;
    entry.mCreateFunc = classReg.mCreateFunc;
    strncpy_s(entry.mDescription, classReg.mDescription, AppWindowManager::APPWINDOW_DESC_LENGTH);
    entry.mDescription[AppWindowManager::APPWINDOW_DESC_LENGTH - 1] = '\0';
    strncpy_s(entry.mClassName, className, AppWindowManager::APPWINDOW_CLASS_LENGTH);
    entry.mClassName[AppWindowManager::APPWINDOW_CLASS_LENGTH - 1] = '\0';

    mTypeTable->Insert(entry);
}

//----------------------------------------------------------------------------------------

void AppWindowManager::UnregisterWindowClass(const char* typeName)
{
    mTypeTable->Remove(typeName);
}

//----------------------------------------------------------------------------------------

const char* AppWindowManager::GetMainWindowType() const
{
    if (mTypeTable->mMainWindowIndex != -1)
    {
        return mTypeTable->mTable[mTypeTable->mMainWindowIndex].mClassName;
    }

    return nullptr;
}

//----------------------------------------------------------------------------------------

Window::Window* AppWindowManager::CreateWindow(const char* typeName, const Window::WindowConfig& config)
{
    TypeTableEntry* entry = nullptr;
    Window::Window* ret = nullptr;

    // Find an entry and use it
    entry = mTypeTable->Get(typeName);
    PG_ASSERTSTR(entry != nullptr, "Trying to create window of unknown type!")
    ret = entry->mCreateFunc(config, mAllocator);

    // Add it to the window table
    mWindowTable->Insert(ret);

    return ret;
}

//----------------------------------------------------------------------------------------

void AppWindowManager::DestroyWindow(Window::Window* window)
{
    // Remove it from the window table
    mWindowTable->Remove(window);
}

}   // namespace Application
}   // namespace Pegasus
