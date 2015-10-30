/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridPropertyManagers.cpp
//! \author	Karolyn Boulanger
//! \date	August 6th, 2015
//! \brief	List of property managers, one per property type

#include "PropertyGrid/PropertyGridPropertyManagers.h"


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

//----------------------------------------------------------------------------------------

void PropertyGridVec2PropertyManagerPrivate::slotDoubleChanged(QtProperty * property, double value)
{
    for (unsigned int c = 0; c < 2; ++c)
    {
        if (QtProperty * prop = m_componentToProperty[c].value(property, nullptr))
        {
            Vec2Property p = m_values[prop];
            p.value[c] = value;
            q_ptr->setValue(prop, p);
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridVec2PropertyManagerPrivate::slotPropertyDestroyed(QtProperty * property)
{
    for (unsigned int c = 0; c < 2; ++c)
    {
        if (QtProperty * vecProp = m_componentToProperty[c].value(property, nullptr))
        {
            m_propertyToComponent[c][vecProp] = nullptr;
            m_componentToProperty[c].remove(property);
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

PropertyGridVec2PropertyManager::PropertyGridVec2PropertyManager(QObject * parent)
:   QtAbstractPropertyManager(parent)
,   d_ptr(new PropertyGridVec2PropertyManagerPrivate)
{
    d_ptr->q_ptr = this;

    d_ptr->m_doublePropertyManager = new QtDoublePropertyManager(this);
    connect(d_ptr->m_doublePropertyManager, SIGNAL(valueChanged(QtProperty *, double)),
            this, SLOT(slotDoubleChanged(QtProperty *, double)));
    connect(d_ptr->m_doublePropertyManager, SIGNAL(propertyDestroyed(QtProperty *)),
            this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

//----------------------------------------------------------------------------------------

PropertyGridVec2PropertyManager::~PropertyGridVec2PropertyManager()
{
    clear();
}

//----------------------------------------------------------------------------------------

QtDoublePropertyManager * PropertyGridVec2PropertyManager::getSubDoublePropertyManager() const
{
    return d_ptr->m_doublePropertyManager;
}

//----------------------------------------------------------------------------------------

Vec2Property PropertyGridVec2PropertyManager::value(const QtProperty * property) const
{
    return d_ptr->m_values.value(property, Vec2Property());
}

//----------------------------------------------------------------------------------------

QString PropertyGridVec2PropertyManager::valueText(const QtProperty * property) const
{
    const PropertyGridVec2PropertyManagerPrivate::PropertyValueMap::const_iterator it = d_ptr->m_values.constFind(property);
    if (it == d_ptr->m_values.constEnd())
    {
        return QString();
    }

    const Vec2Property v = it.value();
    
    return QString("%1/%2").arg(QString::number(v.value[0]))
                           .arg(QString::number(v.value[1]));
}

//----------------------------------------------------------------------------------------

void PropertyGridVec2PropertyManager::setValue(QtProperty * property, const Vec2Property & val)
{
    const PropertyGridVec2PropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
    {
        return;
    }

    if (it.value() == val)
    {
        return;
    }

    it.value() = val;

    for (unsigned int c = 0; c < 2; ++c)
    {
        d_ptr->m_doublePropertyManager->setValue(d_ptr->m_propertyToComponent[c][property], val.value[c]);
    }

    emit propertyChanged(property);
    emit valueChanged(property, val);
}

//----------------------------------------------------------------------------------------

void PropertyGridVec2PropertyManager::initializeProperty(QtProperty * property)
{
    d_ptr->m_values[property] = Vec2Property();

    static const char * const componentName[2] = { "X", "Y" };
    for (unsigned int c = 0; c < 2; ++c)
    {
        QtProperty * prop = d_ptr->m_doublePropertyManager->addProperty();
        prop->setPropertyName(componentName[c]);
        d_ptr->m_doublePropertyManager->setValue(prop, 0);
        d_ptr->m_propertyToComponent[c][property] = prop;
        d_ptr->m_componentToProperty[c][prop] = property;
        property->addSubProperty(prop);
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridVec2PropertyManager::uninitializeProperty(QtProperty * property)
{
    for (unsigned int c = 0; c < 2; ++c)
    {
        QtProperty * prop = d_ptr->m_propertyToComponent[c][property];
        if (prop != nullptr)
        {
            d_ptr->m_componentToProperty[c].remove(prop);
            delete prop;
        }
        d_ptr->m_propertyToComponent[c].remove(property);
    }

    d_ptr->m_values.remove(property);
}

//----------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------

void PropertyGridVec3PropertyManagerPrivate::slotDoubleChanged(QtProperty * property, double value)
{
    for (unsigned int c = 0; c < 3; ++c)
    {
        if (QtProperty * prop = m_componentToProperty[c].value(property, nullptr))
        {
            Vec3Property p = m_values[prop];
            p.value[c] = value;
            q_ptr->setValue(prop, p);
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridVec3PropertyManagerPrivate::slotPropertyDestroyed(QtProperty * property)
{
    for (unsigned int c = 0; c < 3; ++c)
    {
        if (QtProperty * vecProp = m_componentToProperty[c].value(property, nullptr))
        {
            m_propertyToComponent[c][vecProp] = nullptr;
            m_componentToProperty[c].remove(property);
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

PropertyGridVec3PropertyManager::PropertyGridVec3PropertyManager(QObject * parent)
:   QtAbstractPropertyManager(parent)
,   d_ptr(new PropertyGridVec3PropertyManagerPrivate)
{
    d_ptr->q_ptr = this;

    d_ptr->m_doublePropertyManager = new QtDoublePropertyManager(this);
    connect(d_ptr->m_doublePropertyManager, SIGNAL(valueChanged(QtProperty *, double)),
            this, SLOT(slotDoubleChanged(QtProperty *, double)));
    connect(d_ptr->m_doublePropertyManager, SIGNAL(propertyDestroyed(QtProperty *)),
            this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

//----------------------------------------------------------------------------------------

PropertyGridVec3PropertyManager::~PropertyGridVec3PropertyManager()
{
    clear();
}

//----------------------------------------------------------------------------------------

QtDoublePropertyManager * PropertyGridVec3PropertyManager::getSubDoublePropertyManager() const
{
    return d_ptr->m_doublePropertyManager;
}

//----------------------------------------------------------------------------------------

Vec3Property PropertyGridVec3PropertyManager::value(const QtProperty * property) const
{
    return d_ptr->m_values.value(property, Vec3Property());
}

//----------------------------------------------------------------------------------------

QString PropertyGridVec3PropertyManager::valueText(const QtProperty * property) const
{
    const PropertyGridVec3PropertyManagerPrivate::PropertyValueMap::const_iterator it = d_ptr->m_values.constFind(property);
    if (it == d_ptr->m_values.constEnd())
    {
        return QString();
    }

    const Vec3Property v = it.value();
    
    return QString("%1/%2/%3").arg(QString::number(v.value[0]))
                              .arg(QString::number(v.value[1]))
                              .arg(QString::number(v.value[2]));
}

//----------------------------------------------------------------------------------------

void PropertyGridVec3PropertyManager::setValue(QtProperty * property, const Vec3Property & val)
{
    const PropertyGridVec3PropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
    {
        return;
    }

    if (it.value() == val)
    {
        return;
    }

    it.value() = val;

    for (unsigned int c = 0; c < 3; ++c)
    {
        d_ptr->m_doublePropertyManager->setValue(d_ptr->m_propertyToComponent[c][property], val.value[c]);
    }

    emit propertyChanged(property);
    emit valueChanged(property, val);
}

//----------------------------------------------------------------------------------------

void PropertyGridVec3PropertyManager::initializeProperty(QtProperty * property)
{
    d_ptr->m_values[property] = Vec3Property();

    static const char * const componentName[3] = { "X", "Y", "Z" };
    for (unsigned int c = 0; c < 3; ++c)
    {
        QtProperty * prop = d_ptr->m_doublePropertyManager->addProperty();
        prop->setPropertyName(componentName[c]);
        d_ptr->m_doublePropertyManager->setValue(prop, 0);
        d_ptr->m_propertyToComponent[c][property] = prop;
        d_ptr->m_componentToProperty[c][prop] = property;
        property->addSubProperty(prop);
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridVec3PropertyManager::uninitializeProperty(QtProperty * property)
{
    for (unsigned int c = 0; c < 3; ++c)
    {
        QtProperty * prop = d_ptr->m_propertyToComponent[c][property];
        if (prop != nullptr)
        {
            d_ptr->m_componentToProperty[c].remove(prop);
            delete prop;
        }
        d_ptr->m_propertyToComponent[c].remove(property);
    }

    d_ptr->m_values.remove(property);
}

//----------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------

void PropertyGridVec4PropertyManagerPrivate::slotDoubleChanged(QtProperty * property, double value)
{
    for (unsigned int c = 0; c < 4; ++c)
    {
        if (QtProperty * prop = m_componentToProperty[c].value(property, nullptr))
        {
            Vec4Property p = m_values[prop];
            p.value[c] = value;
            q_ptr->setValue(prop, p);
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridVec4PropertyManagerPrivate::slotPropertyDestroyed(QtProperty * property)
{
    for (unsigned int c = 0; c < 4; ++c)
    {
        if (QtProperty * vecProp = m_componentToProperty[c].value(property, nullptr))
        {
            m_propertyToComponent[c][vecProp] = nullptr;
            m_componentToProperty[c].remove(property);
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

PropertyGridVec4PropertyManager::PropertyGridVec4PropertyManager(QObject * parent)
:   QtAbstractPropertyManager(parent)
,   d_ptr(new PropertyGridVec4PropertyManagerPrivate)
{
    d_ptr->q_ptr = this;

    d_ptr->m_doublePropertyManager = new QtDoublePropertyManager(this);
    connect(d_ptr->m_doublePropertyManager, SIGNAL(valueChanged(QtProperty *, double)),
            this, SLOT(slotDoubleChanged(QtProperty *, double)));
    connect(d_ptr->m_doublePropertyManager, SIGNAL(propertyDestroyed(QtProperty *)),
            this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

//----------------------------------------------------------------------------------------

PropertyGridVec4PropertyManager::~PropertyGridVec4PropertyManager()
{
    clear();
}

//----------------------------------------------------------------------------------------

QtDoublePropertyManager * PropertyGridVec4PropertyManager::getSubDoublePropertyManager() const
{
    return d_ptr->m_doublePropertyManager;
}

//----------------------------------------------------------------------------------------

Vec4Property PropertyGridVec4PropertyManager::value(const QtProperty * property) const
{
    return d_ptr->m_values.value(property, Vec4Property());
}

//----------------------------------------------------------------------------------------

QString PropertyGridVec4PropertyManager::valueText(const QtProperty * property) const
{
    const PropertyGridVec4PropertyManagerPrivate::PropertyValueMap::const_iterator it = d_ptr->m_values.constFind(property);
    if (it == d_ptr->m_values.constEnd())
    {
        return QString();
    }

    const Vec4Property v = it.value();
    
    return QString("%1/%2/%3/%4").arg(QString::number(v.value[0]))
                                 .arg(QString::number(v.value[1]))
                                 .arg(QString::number(v.value[2]))
                                 .arg(QString::number(v.value[3]));
}

//----------------------------------------------------------------------------------------

void PropertyGridVec4PropertyManager::setValue(QtProperty * property, const Vec4Property & val)
{
    const PropertyGridVec4PropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
    {
        return;
    }

    if (it.value() == val)
    {
        return;
    }

    it.value() = val;

    for (unsigned int c = 0; c < 4; ++c)
    {
        d_ptr->m_doublePropertyManager->setValue(d_ptr->m_propertyToComponent[c][property], val.value[c]);
    }

    emit propertyChanged(property);
    emit valueChanged(property, val);
}

//----------------------------------------------------------------------------------------

void PropertyGridVec4PropertyManager::initializeProperty(QtProperty * property)
{
    d_ptr->m_values[property] = Vec4Property();

    static const char * const componentName[4] = { "X", "Y", "Z", "W" };
    for (unsigned int c = 0; c < 4; ++c)
    {
        QtProperty * prop = d_ptr->m_doublePropertyManager->addProperty();
        prop->setPropertyName(componentName[c]);
        d_ptr->m_doublePropertyManager->setValue(prop, 0);
        d_ptr->m_propertyToComponent[c][property] = prop;
        d_ptr->m_componentToProperty[c][prop] = property;
        property->addSubProperty(prop);
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridVec4PropertyManager::uninitializeProperty(QtProperty * property)
{
    for (unsigned int c = 0; c < 4; ++c)
    {
        QtProperty * prop = d_ptr->m_propertyToComponent[c][property];
        if (prop != nullptr)
        {
            d_ptr->m_componentToProperty[c].remove(prop);
            delete prop;
        }
        d_ptr->m_propertyToComponent[c].remove(property);
    }

    d_ptr->m_values.remove(property);
}

//----------------------------------------------------------------------------------------

// MOC included here because it needs access to the private implementation.
// The MOC files for debug and opt are excluded from compilation manually in the project
#include "moc_propertygridpropertymanagers.cpp"
