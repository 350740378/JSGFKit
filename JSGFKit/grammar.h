#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <vector>
#include <string>
#include <cstring>
#include <regex>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <cwchar>
#include <fstream>
#include <sstream> 

#include "expansion.h"
#include "rule.h"
#include "unparsedsection.h"
#include "sequence.h"
#include "token.h"
#include "kleenestar.h"
#include "plusoperator.h"
#include "matchinfo.h"
#include "optionalgrouping.h"
#include "requiredgrouping.h"
#include "tag.h"
#include "rulereference.h"
#include "alternativeset.h"
#include "matchinfo.h"
#include "matchresult.h"

typedef std::vector<std::shared_ptr<MatchInfo>> Matchvector;

/**
  * \mainpage
  * JSGF Kit++ is a C++ port of the Java JSGF Kit library.
  * JSGF Kit++ can parse JSGF grammars to produce Grammar objects.
  * These Grammar objects can be manipulated by adding or removing Rule objects and manipulating the rule Expansion for each Rule.
  * Test/input strings can be tested/matched against Rules inside of grammars, and JSGF Kit++ provides functions to extract the tags that the test string matches.
  * Please see the examples folder for example programs for parsing, manipulating, and testing strings against Grammar objects using JSGF Kit Plus Plus.
  *
  */

class Grammar
{
    private:


    protected:
        // Member data
        std::wstring name;
        std::vector<std::shared_ptr<Rule>> rules;

        //Matching
        Matchvector getMatchingExpansions(std::shared_ptr<Expansion> e, std::wstring words[], unsigned int wordCount, unsigned int wordPosition) const;

        // Parsing functions
        static Expansion * parseAlternativeSets(std::vector<Expansion *> & exp);
        static void parseRuleReferences(const std::vector<Expansion *> & expansions, std::vector<Expansion *> & returnExpansions);
        static void parseRequiredGroupings(const std::vector<Expansion *> & expansions, std::vector<Expansion *> & returnExpansions);
        static void parseOptionalGroupings(const std::vector<Expansion *> & expansions, std::vector<Expansion *> & returnExpansions);
        static void parseUnaryOperators(const std::vector<Expansion *> & expansions, std::vector<Expansion *> & returnExpansions);
        static std::vector<Expansion *> parseTokensFromString(std::wstring part);
		static void trimUnparsedSections(std::vector<Expansion *> & expansions);
		static bool isEmptyUnparsedSection(Expansion * e);

    public:
        /** Default constructor */
        Grammar();
        /**
         * Constructor that specifies the grammar name
         * \param [in] string Name of the Grammar
         */
        Grammar(const std::wstring & grammarName);

        Grammar(std::wistream & inputStream);
        Grammar(std::wistream * inputStream);

        /** Default destructor */
        ~Grammar();

        bool writeGrammar(std::wofstream & outputStream) const;

        void addRule(std::shared_ptr<Rule> r);
        std::shared_ptr<Rule> getRule(const std::wstring & name) const;
        std::vector<std::shared_ptr<Rule>> getRules() const;
        bool removeRule(const std::wstring & ruleName);

        std::wstring getName() const;
        void setName(const std::wstring & s);

        std::wstring getText() const;

        // Parsing
        static Expansion * parseExpansionsFromString(const std::wstring & input);
        static void parseGrammarFromString(const std::wstring & s, Grammar & g);
        static void parseGrammar(wifstream f, Grammar & g);

        // Matching
        std::wstring getMatchingPublicRule(std::wstring test) const;
        MatchResult match(std::wstring test) const;
        Matchvector matchesRule(std::shared_ptr<Rule> rule, const std::wstring & test) const;
        Matchvector matchesRule(const std::wstring & ruleName, const std::wstring & test) const;
        std::vector<std::wstring> getMatchingTags(const std::wstring & test) const;
        static std::vector<std::wstring> getMatchingTags(std::vector<std::shared_ptr<MatchInfo>> matchInfo);

        // Utility
        void walkGrammar(void (* callback)(Expansion *));
        void walkExpansion(Expansion * e, void (* callback)(Expansion *));
        static std::wstring printExpansionType(Expansion * e);

        // Helper functions
		static int isspaceMy(int input);

        static bool isSpecialCharacter(wchar_t c);
        static std::wstring trimString(std::wstring input);
        static std::vector<std::wstring> splitString(const std::wstring & s, const std::wstring & rgx_str);
        static bool stringContains(const std::wstring & part, const std::wstring & search);
        static bool stringStartsWith(const std::wstring & s, const std::wstring & test);
        static bool stringEndsWith(const std::wstring & s, const std::wstring & test);
        static std::wstring replaceAll(const std::wstring & s, const std::wstring & re, const std::wstring & replacement);
        static std::wstring replaceFirst(const std::wstring & s, const std::wstring & re, const std::wstring & replacement);

        static std::wregex specialCharacterRegex;

        static inline std::wstring &ltrim(std::wstring &s);
        static inline std::wstring &rtrim(std::wstring &s);
};

#endif // GRAMMAR_H
