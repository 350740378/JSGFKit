﻿#include "matchresult.h"
#include "grammar.h"

MatchResult::MatchResult() : matches(false) { }

MatchResult::MatchResult(std::shared_ptr<Rule> rule, Matchvector ml) : matches(true)
{
    matchingRule = rule;
    matchvector = ml;
}

std::vector<std::wstring> MatchResult::getMatchingTags() {
    return Grammar::getMatchingTags(matchvector);
}

const std::shared_ptr<Rule> MatchResult::getMatchingRule() {
    return matchingRule;
}
