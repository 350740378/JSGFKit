#include "rulereference.h"

RuleReference::RuleReference(std::wstring ruleN)
{
    ruleName = ruleN;
}

RuleReference::~RuleReference()
{
    //dtor
}

Expansion * RuleReference::clone() {
    return new RuleReference(this->getRuleName());
}

std::wstring RuleReference::getText() const {
    return L"<" + ruleName + L">";
}

/** Access ruleName;
* \return The current value of ruleName;
*/
std::wstring RuleReference::getRuleName() const {
    return ruleName;
}

/** Set ruleName;
* \param val New value to set
*/
void RuleReference::setRuleName(std::wstring val) {
    ruleName = val;
}

bool RuleReference::hasChild() const {
    return false;
}

unsigned int RuleReference::childCount() const {
    return 0;
}

std::shared_ptr<Expansion> RuleReference::getChild(const unsigned int index) const {
    return nullptr;
}

void RuleReference::replaceChild(std::shared_ptr<Expansion> newChild, const unsigned long index) {
    // Do nothing cause there are no children
}

ExpansionType RuleReference::getType() const {
    return RULE_REFERENCE;
}
