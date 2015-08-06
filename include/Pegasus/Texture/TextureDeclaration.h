/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureDeclaration.h
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	General texture header to be included by every texture node header

#ifndef PEGASUS_TEXTURE_TEXTUREDECLARATION_H
#define PEGASUS_TEXTURE_TEXTUREDECLARATION_H


//! Macro to use just after the braces when declaring a texture generator node class.
//! It declares the constructors, destructor and the functions for the texture manager
//! \param className Name of the class of the declared node
#define DECLARE_TEXTURE_GENERATOR_NODE(className)                                               \
    DECLARE_TEXTURE_NODE(className, Pegasus::Texture::TextureGenerator)                         \

//! Macro to use just after the braces when declaring a texture operator node class.
//! It declares the constructors, destructor and the functions for the texture manager
//! \param className Name of the class of the declared node
#define DECLARE_TEXTURE_OPERATOR_NODE(className)                                                \
    DECLARE_TEXTURE_NODE(className, Pegasus::Texture::TextureOperator)                          \

//----------------------------------------------------------------------------------------

//! Macro to declares the constructors, destructor and the functions for the texture manager
//! of a texture node class
//! \warning Do not use directly, it is used only for the macros above and the main texture class
//! \param className Name of the class of the declared node
//! \param baseClassName Name of the base class of the declared node
#define DECLARE_TEXTURE_NODE(className, baseClassName)                                          \
    public:                                                                                     \
        inline static const char * GetClassName() { return #className; }                        \
                                                                                                \
        inline static Pegasus::Graph::NodeReturn CreateNode(                                    \
                                    Pegasus::Alloc::IAllocator* nodeAllocator,                  \
                                    Pegasus::Alloc::IAllocator* nodeDataAllocator)              \
            {   return PG_NEW(nodeAllocator, -1, "Texture::" #className, Alloc::PG_MEM_PERM)    \
                                className(nodeAllocator, nodeDataAllocator); }                  \
                                                                                                \
        className(Pegasus::Alloc::IAllocator* nodeAllocator,                                    \
                  Pegasus::Alloc::IAllocator* nodeDataAllocator)                                \
        :   baseClassName(nodeAllocator, nodeDataAllocator) { }                                 \
                                                                                                \
        className(const Pegasus::Texture::TextureConfiguration & configuration,                 \
                  Pegasus::Alloc::IAllocator* nodeAllocator,                                    \
                  Pegasus::Alloc::IAllocator* nodeDataAllocator)                                \
        :   baseClassName(configuration, nodeAllocator, nodeDataAllocator) { }                  \
                                                                                                \
    protected:                                                                                  \
        virtual ~className() { }                                                                \
                                                                                                \
    private:                                                                                    \
        PG_DISABLE_COPY(className)                                                              \


#endif  // PEGASUS_TEXTURE_TEXTUREDECLARATION_H
