﻿#ifndef MATCHINFO_H
#define MATCHINFO_H
#include "expansion.h"
#include <string>

class MatchInfo
{
     private:
        std::shared_ptr<Expansion> expansion; //!< Member variable "expansion"
        std::wstring matchingSection; //!< Member variable "matchingSection"

    public:
        /** Default constructor */
        MatchInfo(std::shared_ptr<Expansion> e, std::wstring matchingStringPart);

        /** Access expansion
         * \return The current value of expansion
         */
        std::shared_ptr<Expansion> getExpansion() { return expansion; }
        /** Set expansion
         * \param val New value to set
         */
        void setExpansion(Expansion * val) { expansion.reset(); expansion = std::shared_ptr<Expansion> (val); }

        /** Access matchingSection
         * \return The current value of matchingSection
         */
        std::wstring getMatchingSection() { return matchingSection; }
        /** Set matchingSection
         * \param val New value to set
         */
        void setMatchingSection(std::wstring val) { matchingSection = val; }
};

#endif // MATCHINFO_H
