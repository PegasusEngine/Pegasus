/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ExpressionEngine.h
//! \author Kleber Garcia
//! \date   11th October 2014
//! \brief  Pegasus Blockscript expression engine

#ifndef PEGASUS_EXPRESSION_ENGINE_H
#define PEGASUS_EXPRESSION_ENGINE_H

#include "Pegasus/BlockScript/IVisitor.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/bs.parser.hpp"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Core/Assertion.h"

extern int* GetIddMem(Pegasus::BlockScript::Ast::Idd* idd, Pegasus::BlockScript::BsVmState& state);

namespace Pegasus
{

namespace BlockScript
{

//! class that interprets an expression tree
template<class IntrinsicType>
class ExpressionEngine : public IVisitor
{
public:
    explicit ExpressionEngine() : mState(nullptr) {}
    virtual ~ExpressionEngine(){}
    IntrinsicType& Eval(Ast::Exp* exp, BsVmState& state);

    
#define BS_PROCESS(N) virtual void Visit(Ast::N* n);
#include "Pegasus/BlockScript/Ast.inl"
#undef BS_PROCESS

private:
    IntrinsicType* GetArrayReference(Ast::Exp* lhs, Ast::Exp* rhs);
    BsVmState* mState;
    IntrinsicType mResult;
};


typedef ExpressionEngine<int>   ExpressionEngine_Int;
typedef ExpressionEngine<float> ExpressionEngine_Float;
typedef ExpressionEngine<Pegasus::Math::Vec4> ExpressionEngine_Float4;
typedef ExpressionEngine<Pegasus::Math::Vec3> ExpressionEngine_Float3;
typedef ExpressionEngine<Pegasus::Math::Vec2> ExpressionEngine_Float2;

//Singleton expression engines. 
ExpressionEngine_Int    gIntExpEngine;
ExpressionEngine_Float  gFloatExpEngine;
ExpressionEngine_Float2 gFloat2ExpEngine;
ExpressionEngine_Float3 gFloat3ExpEngine;
ExpressionEngine_Float4 gFloat4ExpEngine;


}
}

#include "../Source/Pegasus/BlockScript/ExpressionEngine.inl"



#endif
