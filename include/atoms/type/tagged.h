#pragma once

// This file is part of 'Atoms' library - https://github.com/yaqwsx/atoms
// Author: Jan 'yaqwsx' Mrázek

#include <tuple>
#include <cassert>
#include <iostream>

namespace atoms {

enum class Operator {
    ADD, SUBTRACT, MULTIPLY, DIVIDE, EQUAL
    // ToDo: Add other operators
};

struct NoTagMerge {
    static std::tuple<> merge(Operator, const std::tuple<>&, const std::tuple<>&) {
        return {};
    }

    static bool equal(const std::tuple<>&, const std::tuple<>&) {
        return true;
    }
};

template <class Type>
struct OperatorMerge {
    static Type merge(Operator op, const Type& a, const Type& b) {
        switch(op) {
            case Operator::ADD:
                return a + b;
            case Operator::SUBTRACT:
                return a - b;
            case Operator::MULTIPLY:
                return a * b;
            case Operator::DIVIDE:
                return a / b;
            default:
                assert(false && "Unsupported operation");
        }
    }

    static bool equal(const Type& a, const Type& b) {
        return a == b;
    }
};

// Adds a tag to an arbitrary class or structure, keeping operator definitions
// and providing tag merge procedure
template <class Base, class TagType = std::tuple<>, class TagMerge = NoTagMerge>
struct Tagged : public Base {
    using Base::Base;

    Tagged& operator=(const Base& b) { Base::oeprator=(b); }
    Tagged(const Base& b) : Base(b) {}

    template <class... Args>
    static Tagged make_tagged(const TagType& t, Args... args) {
        Tagged res{args...};
        res.tag = t;
        return res;
    }

    TagType tag;
};

#define GENERATE_TAGGED_OPERATOR(op, opcode) \
template <class Base1, class Base2, class TagType, class TagMerge> \
auto operator op (const Tagged<Base1, TagType, TagMerge>& a, const Tagged<Base2, TagType, TagMerge>& b) \
    -> Tagged<decltype(std::declval<Base1>() op std::declval<Base2>()), TagType, TagMerge> \
{ \
    using  ResType = Tagged<decltype(std::declval<Base1>() op std::declval<Base2>()), TagType, TagMerge>; \
    ResType res = (*static_cast<const Base1*>(&a)) op (*static_cast<const Base2*>(&b)); \
    res.tag = TagMerge::merge(opcode, a.tag, b.tag); \
    return res; \
}

#define GENERATE_TAGGED_SHORT_OPERATOR(op, opcode) \
template <class Base1, class Base2, class TagType, class TagMerge> \
auto operator op (Tagged<Base1, TagType, TagMerge>& a, const Tagged<Base2, TagType, TagMerge>& b) \
    -> Tagged<Base1, TagType, TagMerge>& \
{ \
    (*static_cast<Base1*>(&a)) op (*static_cast<const Base2*>(&b)); \
    a.tag = TagMerge::merge(opcode, a.tag, b.tag); \
    return a; \
}

GENERATE_TAGGED_OPERATOR(-, Operator::SUBTRACT);
GENERATE_TAGGED_SHORT_OPERATOR(-=, Operator::SUBTRACT);
GENERATE_TAGGED_OPERATOR(+, Operator::ADD);
GENERATE_TAGGED_SHORT_OPERATOR(+=, Operator::ADD);
GENERATE_TAGGED_OPERATOR(*, Operator::MULTIPLY);
GENERATE_TAGGED_SHORT_OPERATOR(*=, Operator::MULTIPLY);
GENERATE_TAGGED_OPERATOR(/, Operator::DIVIDE);
GENERATE_TAGGED_SHORT_OPERATOR(/=, Operator::DIVIDE);
// ToDo: Other operators

} // namespace atoms