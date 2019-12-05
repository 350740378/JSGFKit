#include "optionalgrouping.h"

OptionalGrouping::OptionalGrouping()
{
    //ctor
}

OptionalGrouping::~OptionalGrouping()
{
    childExpansion.reset();
}

Expansion * OptionalGrouping::clone() {
    return new OptionalGrouping(std::shared_ptr<Expansion>(this->getChild()->clone()));
}

OptionalGrouping::OptionalGrouping(std::shared_ptr<Expansion> e) {
    childExpansion = e;
}

std::wstring OptionalGrouping::getText() const {
    std::wstring s = L"[";
    s.append(childExpansion->getText());
    s.append(L"]");
    return s;
}

ExpansionType OptionalGrouping::getType() const {
    return OPTIONAL_GROUPING;
}

unsigned int OptionalGrouping::childCount() const {
    return hasChild() ? 1 : 0;
}

void OptionalGrouping::replaceChild(std::shared_ptr<Expansion> newChild, const unsigned long index) {
    childExpansion = newChild;
}

bool OptionalGrouping::hasChild() const {
    return childExpansion != nullptr;
}

std::shared_ptr<Expansion> OptionalGrouping::getChild(const unsigned int index) const {
    return childExpansion;
}

void OptionalGrouping::setChild(std::shared_ptr<Expansion> e) {
    childExpansion = e;
}
