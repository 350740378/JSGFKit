﻿#include "kleenestar.h"

KleeneStar::KleeneStar()
{
    //ctor
}

KleeneStar::~KleeneStar()
{
    childExpansion.reset();
}

Expansion * KleeneStar::clone() {
    return new KleeneStar(std::shared_ptr<Expansion>(this->getChild()->clone()));
}

KleeneStar::KleeneStar(std::shared_ptr<Expansion> e) {
    childExpansion = e;
}

std::wstring KleeneStar::getText() const {
    std::wstring s = childExpansion->getText();
    s.append(L"*");
    return s;
}

ExpansionType KleeneStar::getType() const {
    return KLEENE_STAR;
}

unsigned int KleeneStar::childCount() const {
    return hasChild() ? 1 : 0;
}

bool KleeneStar::hasChild() const {
    return childExpansion != nullptr;
}

void KleeneStar::replaceChild(std::shared_ptr<Expansion> newChild, const unsigned long index) {
    childExpansion = newChild;
}

std::shared_ptr<Expansion> KleeneStar::getChild(const unsigned int index) const {
    return childExpansion;
}

void KleeneStar::setChild(std::shared_ptr<Expansion> e) {
    childExpansion.reset();
    childExpansion = e;
}
