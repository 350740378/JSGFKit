#include "token.h"

Token::Token()
{
    //ctor
}

Token::Token(std::wstring s) {
    text = s;
}

Token::~Token()
{

}

Expansion * Token::clone() {
    return new Token(this->getText());
}

std::wstring Token::getText() const {
    return text;
}

bool Token::hasChild() const {
    return false;
}

std::shared_ptr<Expansion> Token::getChild(const unsigned int index) const {
    return nullptr;
}

/** Set text
 * \param val New value to set
 */
void Token::setText(std::wstring val) {
    text = val;
}

unsigned int Token::childCount() const {
    return 0;
}

void Token::replaceChild(std::shared_ptr<Expansion> newChild, const unsigned long index) {
    // Do nothing because Token's don't have children
}

ExpansionType Token::getType() const {
    return TOKEN;
}
