#include "rule.h"
#include "grammar.h"

Rule::Rule()
{
    //ctor
}

Rule::Rule(std::wstring n, bool visible, std::shared_ptr<Expansion> e)
{
    name = n;
    isVisible = visible;
    ruleExpansion = e;
}

Rule::~Rule()
{
    ruleExpansion.reset();
}

std::wstring Rule::getRuleString() const {
    if(isVisible) {
		std::wstring stmp = ruleExpansion->getText();
        return Grammar::trimString(L"public <" + name + L"> = " + stmp) + L";";
    }
    else {
        return Grammar::trimString(L"<" + name + L"> = " + ruleExpansion->getText()) + L";";
    }

}
