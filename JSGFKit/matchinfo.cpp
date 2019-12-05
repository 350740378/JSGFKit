#include "matchinfo.h"

MatchInfo::MatchInfo(std::shared_ptr<Expansion> e, std::wstring matchingStringPart)
{
    expansion = e;
    matchingSection = matchingStringPart;
}
