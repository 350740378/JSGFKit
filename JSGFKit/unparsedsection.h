﻿#ifndef UNPARSEDSECTION_H
#define UNPARSEDSECTION_H

#include <expansion.h>
#include <string>

using namespace std;

/// An internal class used by the parser to generate the Expansions
class UnparsedSection : public Expansion
{
    private:
        wstring section;

    public:
        /** Default constructor */
        UnparsedSection();
        UnparsedSection(wstring s);

        /** Default destructor */
        ~UnparsedSection();
        Expansion * clone(); // Really should never be used but should still be implemented

        ExpansionType getType() const;

        ///Will always return true
        bool hasChild() const;
        void replaceChild(std::shared_ptr<Expansion> newChild, const unsigned long index = 0);

        unsigned int childCount() const;

        /// Will always return nullptr
        shared_ptr<Expansion> getChild(const unsigned int index = 0) const;

        wstring getSection() const;
        void setSection(wstring s);
        wstring getText() const;
};

#endif // UNPARSEDSECTION_H
