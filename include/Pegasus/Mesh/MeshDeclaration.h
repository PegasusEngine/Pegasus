/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MehsDeclaration.h
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	General mesh header to be included by every mesh node header

#ifndef PEGASUS_MESH_MESHDECLARATION_H
#define PEGASUS_MESH_MESHDECLARATION_H


//! Macro to use just after the braces when declaring a mesh generator node class.
//! It declares the constructors, destructor and the functions for the mesh manager
//! \param className Name of the class of the declared node
#define DECLARE_MESH_GENERATOR_NODE(className)                                       \
    DECLARE_MESH_NODE(className, Pegasus::Mesh::MeshGenerator)                 \

//! Macro to use just after the braces when declaring a mesh operator node class.
//! It declares the constructors, destructor and the functions for the mesh manager
//! \param className Name of the class of the declared node
#define DECLARE_MESH_OPERATOR_NODE(className)                                        \
    DECLARE_MESH_NODE(className, Pegasus::Mesh::MeshOperator)                  \

//----------------------------------------------------------------------------------------

//! Macro to declares the constructors, destructor and the functions for the mesh manager
//! of a mesh node class
//! \warning Do not use directly, it is used only for the macros above and the main mesh class
//! \param className Name of the class of the declared node
//! \param baseClassName Name of the base class of the declared node
#define DECLARE_MESH_NODE(className, baseClassName)                                      \
    public:                                                                                 \
        inline static const char * GetClassName() { return #className; }                    \
                                                                                            \
        inline static Pegasus::Graph::NodeReturn CreateNode(                                \
                                    Pegasus::Alloc::IAllocator* nodeAllocator,            \
                                    Pegasus::Alloc::IAllocator* nodeDataAllocator)        \
            {   return PG_NEW(nodeAllocator, -1, "Mesh::" #className, Alloc::PG_MEM_PERM)   \
                                className(nodeAllocator, nodeDataAllocator); }              \
                                                                                            \
                                                                                            \
                                                                                            \
    private:                                                                                \
        PG_DISABLE_COPY(className)                                                          \


#endif  // PEGASUS_MESH_MESHDECLARATION_H
