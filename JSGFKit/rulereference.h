#ifndef RULEREFERENCE_H
#define RULEREFERENCE_H

#include <expansion.h>


class RuleReference : public Expansion
{
    private:
        std::wstring ruleName; //!< Member variable "ruleName;"

    public:
        /** Default constructor */
        RuleReference(std::wstring ruleN);

        /** Default destructor */
        ~RuleReference();

        /** Access ruleName;
         * \return The current value of ruleName;
         */
        std::wstring getRuleName() const;

        /** Set ruleName;
         * \param val New value to set
         */
        void setRuleName(std::wstring val);

        bool hasChild() const;
        void replaceChild(std::shared_ptr<Expansion> newChild, const unsigned long index = 0);
        unsigned int childCount() const;
        Expansion * clone();
        std::shared_ptr<Expansion> getChild(const unsigned int index = 0) const;
        ExpansionType getType() const;
        std::wstring getText() const;
};

#endif // RULEREFERENCE_H
