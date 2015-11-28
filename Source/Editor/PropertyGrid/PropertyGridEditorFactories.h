/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridEditorFactories.h
//! \author	Karolyn Boulanger
//! \date	August 6th, 2015
//! \brief	List of editor factories, one per property editor type

#ifndef EDITOR_PROPERTYGRIDEDITORFACTORIES_H
#define EDITOR_PROPERTYGRIDEDITORFACTORIES_H

#include "PropertyGrid/qtpropertybrowser/qteditorfactory.h"


typedef QtCheckBoxFactory           PropertyGridBoolEditorFactory;
typedef QtSpinBoxFactory            PropertyGridUIntEditorFactory;
typedef QtSpinBoxFactory            PropertyGridIntEditorFactory;
typedef QtDoubleSpinBoxFactory      PropertyGridFloatEditorFactory;
typedef QtColorEditorFactory        PropertyGridColor8RGBEditorFactory;
typedef QtColorEditorFactory        PropertyGridColor8RGBAEditorFactory;
typedef QtLineEditFactory           PropertyGridString64EditorFactory;
typedef QtEnumEditorFactory         PropertyGridEnumEditorFactory;


#endif // EDITOR_PROPERTYGRIDEDITORFACTORIES_H
