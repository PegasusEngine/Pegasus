/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ExpressionEngine.inl
//! \author Kleber Garcia
//! \date   15th October 2014
//! \brief  Pegasus Blockscript expression engine internal implementation 


namespace Pegasus
{
namespace BlockScript
{

template<class IntrinsicType> IntrinsicType& ExpressionEngine<IntrinsicType>::Eval(Ast::Exp* exp, BsVmState& state)
{
    mState = &state;
    exp->Access(this);
    return mResult;
}

template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::Program* n)          {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::ExpList* n)          {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::Stmt* n)             {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::StmtList* n)         {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::ArgDec* n)           {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::ArgList* n)          {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::FunCall* n)          {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::Imm* n)              {PG_FAILSTR("this function must be specialized via templates.");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::StrImm* n)           {PG_FAILSTR("this function must be specialized via templates.");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::StmtExp* n)          {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::StmtFunDec* n)       {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::StmtIfElse* n)       {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::StmtWhile* n)        {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::StmtReturn* n)       {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::StmtStructDef* n)    {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::StmtEnumTypeDef* n)  {PG_FAILSTR("function not supported");}
template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::ArrayConstructor* n) {PG_FAILSTR("function not supported");}

template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::Exp* n)              { n->Access(this); }

template<class IntrinsicType> IntrinsicType* ExpressionEngine<IntrinsicType>::GetArrayReference(Ast::Exp* lhs, Ast::Exp* rhs)
{
    PG_ASSERT(lhs->GetExpType() == Ast::Idd::sType);
    PG_ASSERT(lhs->GetTypeDesc()->GetModifier() == TypeDesc::M_ARRAY);

    Ast::Idd* lhsIdd = static_cast<Ast::Idd*>(lhs);
    int rhsOffset = gIntExpEngine.Eval(rhs, *mState);

    char* memLoc = reinterpret_cast<char*>(GetIddMem(lhsIdd, *mState)) + rhsOffset; 

    return reinterpret_cast<IntrinsicType*>(memLoc);
}

template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::Binop* n)
{
    if (n->GetOp() == O_ACCESS)
    {
        mResult = *GetArrayReference(n->GetLhs(),n->GetRhs());
        return;
    }

    n->GetLhs()->Access(this);
    IntrinsicType r1 = mResult;

    n->GetRhs()->Access(this);
    IntrinsicType r2 = mResult; 

    switch(n->GetOp())
    {
    case O_MUL:
        mResult = r1 * r2;        
        break;
    case O_PLUS: 
        mResult = r1 + r2;
        break;
    case O_MINUS: 
        mResult = r1 - r2;
        break;
    case O_DIV: 
        mResult = r1 / r2;
        break;
    default:
        PG_FAILSTR("Unsupported expression!");
    }
}

void ExpressionEngine<int>::Visit(Ast::Binop* n)
{
    if (n->GetOp() == O_ACCESS)
    {
        mResult = *GetArrayReference(n->GetLhs(),n->GetRhs());
        return;
    }

    n->GetLhs()->Access(this);
    IntrinsicType r1 = mResult;

    n->GetRhs()->Access(this);
    IntrinsicType r2 = mResult; 

    switch(n->GetOp())
    {
    case O_MUL:
        mResult = r1 * r2;        
        break;
    case O_PLUS: 
        mResult = r1 + r2;
        break;
    case O_MINUS: 
        mResult = r1 - r2;
        break;
    case O_DIV: 
        mResult = r1 / r2;
        break;
    case O_MOD: 
        mResult = r1 % r2;
        break;
    case O_EQ: 
        mResult = r1 == r2;
        break;
    case O_NEQ: 
        mResult = r1 != r2;
        break;
    case O_GT: 
        mResult = r1 > r2;
        break;
    case O_LT: 
        mResult = r1 < r2;
        break;
    case O_GTE: 
        mResult = r1 >= r2;
        break;
    case O_LTE: 
        mResult = r1 <= r2;
        break;
    case O_LAND: 
        mResult = r1 && r2;
        break;
    case O_LOR: 
        mResult = r1 || r2;
        break;
    default:
        PG_FAILSTR("Unsupported expression!");
    }
}

void ExpressionEngine<float>::Visit(Ast::Binop* n)
{
    if (n->GetOp() == O_ACCESS)
    {
        mResult = *GetArrayReference(n->GetLhs(),n->GetRhs());
        return;
    }

    n->GetLhs()->Access(this);
    IntrinsicType r1 = mResult;

    n->GetRhs()->Access(this);
    IntrinsicType r2 = mResult; 

    switch(n->GetOp())
    {
    case O_MUL:
        mResult = r1 * r2;        
        break;
    case O_PLUS: 
        mResult = r1 + r2;
        break;
    case O_MINUS: 
        mResult = r1 - r2;
        break;
    case O_DIV: 
        mResult = r1 / r2;
        break;
    case O_EQ: 
        mResult = r1 == r2;
        break;
    case O_NEQ: 
        mResult = r1 != r2;
        break;
    case O_GT: 
        mResult = r1 > r2;
        break;
    case O_LT: 
        mResult = r1 < r2;
        break;
    case O_GTE: 
        mResult = r1 >= r2;
        break;
    case O_LTE: 
        mResult = r1 <= r2;
        break;
    case O_LAND: 
        mResult = r1 && r2;
        break;
    case O_LOR: 
        mResult = r1 || r2;
        break;
    default:
        PG_FAILSTR("Unsupported expression!");
    }
}

template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::Unop* unop)
{
    unop->GetExp()->Access(this);
    IntrinsicType r = mResult;

    switch(unop->GetOp())
    {
    case O_MINUS:
        mResult = -r;
        break;
    default:
        PG_FAILSTR("Unsupported unary operator.");
    }
}

template<class IntrinsicType> void ExpressionEngine<IntrinsicType>::Visit(Ast::Idd* n)
{
    mResult = *reinterpret_cast<IntrinsicType*>(GetIddMem(n, *mState)); 
}

void ExpressionEngine_Float::Visit(Ast::Imm* n)
{
    mResult = n->GetVariant().f[0];
}

void ExpressionEngine_Int::Visit(Ast::Imm* n)
{
    mResult = n->GetVariant().i[0];
}

void ExpressionEngine_Float4::Visit(Ast::Imm* n)
{
    mResult.x = n->GetVariant().f[0];
    mResult.y = n->GetVariant().f[1];
    mResult.z = n->GetVariant().f[2];
    mResult.w = n->GetVariant().f[3];
}

void ExpressionEngine_Float3::Visit(Ast::Imm* n)
{
    mResult.x = n->GetVariant().f[0];
    mResult.y = n->GetVariant().f[1];
    mResult.z = n->GetVariant().f[2];
}

void ExpressionEngine_Float2::Visit(Ast::Imm* n)
{
    mResult.x = n->GetVariant().f[0];
    mResult.y = n->GetVariant().f[1];
}

} //namespace BlockScript
} //namespace ExpressioEngine
