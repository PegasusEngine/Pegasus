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
#include "PropertyTypes.h"

// Managers using the Qt original implementations

typedef QtBoolPropertyManager       PropertyGridBoolPropertyManager;
typedef QtIntPropertyManager        PropertyGridUIntPropertyManager;
typedef QtIntPropertyManager        PropertyGridIntPropertyManager;
typedef QtDoublePropertyManager     PropertyGridFloatPropertyManager;
typedef QtColorPropertyManager      PropertyGridColor8RGBPropertyManager;
typedef QtColorPropertyManager      PropertyGridColor8RGBAPropertyManager;
typedef QtStringPropertyManager     PropertyGridString64PropertyManager;
typedef QtEnumPropertyManager       PropertyGridEnumPropertyManager;


class PropertyGridVec2PropertyManager;
class PropertyGridVec2PropertyManagerPrivate
{
public:

    //! Slot called when the value of a vector component has changed
    //! \param property Property that has changed
    //! \param value New value of the vector component
    void slotDoubleChanged(QtProperty * property, double value);

    //! Slot called when a component property is destroyed
    //! \param property Component property being destroyed
    void slotPropertyDestroyed(QtProperty * property);

    typedef QMap<const QtProperty *, Vec2Property> PropertyValueMap;
    
    //! Map between properties and their corresponding values
    PropertyValueMap m_values;

    //! Manager used for individual components
    QtDoublePropertyManager * m_doublePropertyManager;

    //! Map between properties and component subproperties
    QMap<const QtProperty *, QtProperty *> m_propertyToComponent[2];

    //! Map between component properties and subproperties
    QMap<const QtProperty *, QtProperty *> m_componentToProperty[2];

private:

    //! Manager owning this private implementation
    PropertyGridVec2PropertyManager * q_ptr;

    Q_DECLARE_PUBLIC(PropertyGridVec2PropertyManager)
};


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
class PropertyGridVec3PropertyManager;
class PropertyGridVec3PropertyManagerPrivate
{
public:

    //! Slot called when the value of a vector component has changed
    //! \param property Property that has changed
    //! \param value New value of the vector component
    void slotDoubleChanged(QtProperty * property, double value);

    //! Slot called when a component property is destroyed
    //! \param property Component property being destroyed
    void slotPropertyDestroyed(QtProperty * property);

    typedef QMap<const QtProperty *, Vec3Property> PropertyValueMap;
    
    //! Map between properties and their corresponding values
    PropertyValueMap m_values;

    //! Manager used for individual components
    QtDoublePropertyManager * m_doublePropertyManager;

    //! Map between properties and component subproperties
    QMap<const QtProperty *, QtProperty *> m_propertyToComponent[3];

    //! Map between component properties and subproperties
    QMap<const QtProperty *, QtProperty *> m_componentToProperty[3];

private:

    //! Manager owning this private implementation
    PropertyGridVec3PropertyManager * q_ptr;

    Q_DECLARE_PUBLIC(PropertyGridVec3PropertyManager)
};


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

class PropertyGridVec4PropertyManager;
class PropertyGridVec4PropertyManagerPrivate
{
public:

    //! Slot called when the value of a vector component has changed
    //! \param property Property that has changed
    //! \param value New value of the vector component
    void slotDoubleChanged(QtProperty * property, double value);

    //! Slot called when a component property is destroyed
    //! \param property Component property being destroyed
    void slotPropertyDestroyed(QtProperty * property);

    typedef QMap<const QtProperty *, Vec4Property> PropertyValueMap;
    
    //! Map between properties and their corresponding values
    PropertyValueMap m_values;

    //! Manager used for individual components
    QtDoublePropertyManager * m_doublePropertyManager;

    //! Map between properties and component subproperties
    QMap<const QtProperty *, QtProperty *> m_propertyToComponent[4];

    //! Map between component properties and subproperties
    QMap<const QtProperty *, QtProperty *> m_componentToProperty[4];

private:

    //! Manager owning this private implementation
    PropertyGridVec4PropertyManager * q_ptr;

    Q_DECLARE_PUBLIC(PropertyGridVec4PropertyManager)
};


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
