#include "tag.h"
#include <iostream>

std::wstring Tag::getText() const {
    std::wstring s = childExpansion->getText();
    std::vector<std::wstring>::const_iterator it;
    for(it = strings.begin(); it != strings.end(); it++){
        s.append(L" {" + *it + L"}");
    }
    return s;
}

bool Tag::hasChild() const {
    return true;
}

void Tag::addTag(std::wstring t) {
    strings.push_back(t);
}

void Tag::removeTag(std::wstring t) {
    strings.erase(std::find(strings.begin(), strings.end(), t));
}

int Tag::getTagCount() const
{
    return tagCount;
}

Expansion * Tag::clone() {
    Tag * t = new Tag(std::shared_ptr<Expansion>(this->getChild()->clone()));
    std::vector<std::wstring> tags = strings;
    std::vector<std::wstring>::iterator it;
    for(it = tags.begin(); it != tags.end(); it++){
        t->addTag(*it);
    }
    return t;
}

Tag::~Tag() {
    childExpansion.reset();
}

Tag::Tag(std::shared_ptr<Expansion> e)
{
    childExpansion = e;
    tagCount = 0;
}

Tag::Tag(std::shared_ptr<Expansion> e, std::wstring tag) {
    childExpansion = e;
    strings.push_back(tag);
    tagCount++;
}

Tag::Tag(std::shared_ptr<Expansion> e, unsigned short numberOfTags, std::wstring tags[]) {
    childExpansion = e;
    for(unsigned short i = 0; i < numberOfTags; i++) {
        strings.push_back(tags[i]);
    }
    tagCount += numberOfTags;
}

/** Access childExpansion
 * \return The current value of childExpansion
 */
std::shared_ptr<Expansion> Tag::getChild(const unsigned int index) const {
    return childExpansion;
}

void Tag::replaceChild(std::shared_ptr<Expansion> newChild, const unsigned long index) {
    childExpansion = newChild;
}

/**
  * Returns a vector of strings of the tags the child Expansion is tagged with. (All of the strings between the { } )
  */
 std::vector<std::wstring> Tag::getTags() const {
    std::vector<std::wstring> v;
    std::vector<std::wstring>::const_iterator it;
    for(it = strings.begin(); it != strings.end(); it++){
        v.push_back(*it);
    }
    return v;
}

/** Set childExpansion
 * \param val Disassociates with the current child and tags the new specified child Expansion.
 */
void Tag::setChildExpansion(std::shared_ptr<Expansion> val) {
    childExpansion = val;
}

ExpansionType Tag::getType() const {
    return TAG;
}

unsigned int Tag::childCount() const {
    return hasChild() ? 1 : 0;
}
