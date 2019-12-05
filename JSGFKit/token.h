#ifndef TOKEN_H
#define TOKEN_H
#include "expansion.h"
#include <iostream>

using namespace std;

class Token : public Expansion
{
    private:
        wstring text; //!< Member variable "text"

    public:
        /** Default constructor */
        Token();
        Token(wstring s);
        /** Default destructor */
        ~Token();

        Expansion * clone();

        /** Access text
         * \return The current value of text
         */
        wstring getText() const;
        /** Set text
         * \param val New value to set
         */
        void setText(wstring val);

        ExpansionType getType() const;
        void replaceChild(std::shared_ptr<Expansion> newChild, const unsigned long index = 0);

        unsigned int childCount() const;
        bool hasChild() const;
        shared_ptr<Expansion> getChild(const unsigned int index = 0) const;
};

#endif // TOKEN_H
