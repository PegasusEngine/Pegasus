/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridPropertyManagers.h
//! \author	Karolyn Boulanger
//! \date	August 6th, 2015
//! \brief	List of property managers, one per property type

#ifndef EDITOR_PROPERTYGRIDPROPERTYMANAGERS_H
#define EDITOR_PROPERTYGRIDPROPERTYMANAGERS_H

#include "PropertyGrid/qtpropertybrowser/qtpropertymanager.h"


// Managers using the Qt original implementations

typedef QtBoolPropertyManager       PropertyGridBoolPropertyManager;
typedef QtIntPropertyManager        PropertyGridUIntPropertyManager;
typedef QtIntPropertyManager        PropertyGridIntPropertyManager;
typedef QtDoublePropertyManager     PropertyGridFloatPropertyManager;
typedef QtColorPropertyManager      PropertyGridColor8RGBPropertyManager;
typedef QtColorPropertyManager      PropertyGridColor8RGBAPropertyManager;
typedef QtStringPropertyManager     PropertyGridString64PropertyManager;

//----------------------------------------------------------------------------------------

// Additional types for properties (only used for storage in the widget,
// we cannot use Pegasus types here

struct Vec2Property
{
    float value[2];

    Vec2Property()
        {   value[0] = value[1] = 0.0f; }

    bool operator==(const Vec2Property & v) const
        {   return (value[0] == v.value[0]) && (value[1] == v.value[1]);    }
};

struct Vec3Property
{
    float value[3];

    Vec3Property()
        {   value[0] = value[1] = value[2] = 0.0f;  }

    bool operator==(const Vec3Property & v) const
        {   return (value[0] == v.value[0]) && (value[1] == v.value[1]) && (value[2] == v.value[2]);    }
};

struct Vec4Property
{
    float value[4];

    Vec4Property()
        {   value[0] = value[1] = value[2] = value[3] = 0.0f; }

    bool operator==(const Vec4Property & v) const
        {   return (value[0] == v.value[0]) && (value[1] == v.value[1]) && (value[2] == v.value[2]) && (value[3] == v.value[3]);    }
};

//----------------------------------------------------------------------------------------

class PropertyGridVec2PropertyManagerPrivate;

class PropertyGridVec2PropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT

public:

    PropertyGridVec2PropertyManager(QObject * parent = 0);
    ~PropertyGridVec2PropertyManager();

    //! Get the manager used for individual components
    //! \return Pointer to the manager for components
    QtDoublePropertyManager * getSubDoublePropertyManager() const;

    //! Get the value of a property
    //! \param property Property to get the value from
    //! \return Vector value of the property
    Vec2Property value(const QtProperty * property) const;

public Q_SLOTS:

    //! Set the value of a property
    //! \param property Property to set the value to
    //! \param val New value of the vector
    void setValue(QtProperty * property, const Vec2Property & val);

Q_SIGNALS:

    //! Sent when any component of the property has changed
    //! \param property Property that has changed
    //! \param val New value of the vector
    void valueChanged(QtProperty * property, const Vec2Property & val);

protected:

    //! Convert a property to text, for the vector representation
    //! \param property Property to convert
    //! \return String version of the full vector
    QString valueText(const QtProperty * property) const;

    //! Register a property in the manager
    //! \param property Property to register
    virtual void initializeProperty(QtProperty * property) override;

    //! Unregister a property from the manager
    //! \param property Property to unregister
    virtual void uninitializeProperty(QtProperty * property) override;

private:

    //! Private implementation
    QScopedPointer<PropertyGridVec2PropertyManagerPrivate> d_ptr;

    Q_DECLARE_PRIVATE(PropertyGridVec2PropertyManager)
    Q_DISABLE_COPY(PropertyGridVec2PropertyManager)

    Q_PRIVATE_SLOT(d_func(), void slotDoubleChanged(QtProperty *, double))
    Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty *))
};

//----------------------------------------------------------------------------------------

class PropertyGridVec3PropertyManagerPrivate;

class PropertyGridVec3PropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT

public:

    PropertyGridVec3PropertyManager(QObject * parent = 0);
    ~PropertyGridVec3PropertyManager();

    //! Get the manager used for individual components
    //! \return Pointer to the manager for components
    QtDoublePropertyManager * getSubDoublePropertyManager() const;

    //! Get the value of a property
    //! \param property Property to get the value from
    //! \return Vector value of the property
    Vec3Property value(const QtProperty * property) const;

public Q_SLOTS:

    //! Set the value of a property
    //! \param property Property to set the value to
    //! \param val New value of the vector
    void setValue(QtProperty * property, const Vec3Property & val);

Q_SIGNALS:

    //! Sent when any component of the property has changed
    //! \param property Property that has changed
    //! \param val New value of the vector
    void valueChanged(QtProperty * property, const Vec3Property & val);

protected:

    //! Convert a property to text, for the vector representation
    //! \param property Property to convert
    //! \return String version of the full vector
    QString valueText(const QtProperty * property) const;

    //! Register a property in the manager
    //! \param property Property to register
    virtual void initializeProperty(QtProperty * property) override;

    //! Unregister a property from the manager
    //! \param property Property to unregister
    virtual void uninitializeProperty(QtProperty * property) override;

private:

    //! Private implementation
    QScopedPointer<PropertyGridVec3PropertyManagerPrivate> d_ptr;

    Q_DECLARE_PRIVATE(PropertyGridVec3PropertyManager)
    Q_DISABLE_COPY(PropertyGridVec3PropertyManager)

    Q_PRIVATE_SLOT(d_func(), void slotDoubleChanged(QtProperty *, double))
    Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty *))
};

//----------------------------------------------------------------------------------------

class PropertyGridVec4PropertyManagerPrivate;

class PropertyGridVec4PropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT

public:

    PropertyGridVec4PropertyManager(QObject * parent = 0);
    ~PropertyGridVec4PropertyManager();

    //! Get the manager used for individual components
    //! \return Pointer to the manager for components
    QtDoublePropertyManager * getSubDoublePropertyManager() const;

    //! Get the value of a property
    //! \param property Property to get the value from
    //! \return Vector value of the property
    Vec4Property value(const QtProperty * property) const;

public Q_SLOTS:

    //! Set the value of a property
    //! \param property Property to set the value to
    //! \param val New value of the vector
    void setValue(QtProperty * property, const Vec4Property & val);

Q_SIGNALS:

    //! Sent when any component of the property has changed
    //! \param property Property that has changed
    //! \param val New value of the vector
    void valueChanged(QtProperty * property, const Vec4Property & val);

protected:

    //! Convert a property to text, for the vector representation
    //! \param property Property to convert
    //! \return String version of the full vector
    QString valueText(const QtProperty * property) const;

    //! Register a property in the manager
    //! \param property Property to register
    virtual void initializeProperty(QtProperty * property) override;

    //! Unregister a property from the manager
    //! \param property Property to unregister
    virtual void uninitializeProperty(QtProperty * property) override;

private:

    //! Private implementation
    QScopedPointer<PropertyGridVec4PropertyManagerPrivate> d_ptr;

    Q_DECLARE_PRIVATE(PropertyGridVec4PropertyManager)
    Q_DISABLE_COPY(PropertyGridVec4PropertyManager)

    Q_PRIVATE_SLOT(d_func(), void slotDoubleChanged(QtProperty *, double))
    Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty *))
};


#endif // EDITOR_PROPERTYGRIDPROPERTYMANAGERS_H
