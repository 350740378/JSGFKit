#include "grammar.h"
using namespace std;

std::wregex Grammar::specialCharacterRegex  = std::wregex(L"[;=<>*+\\[\\]()|{} ]"); /// Regex that matches for JSGF special characters that cannot be unescaped in non-quoted Tokens

Grammar::Grammar() /// Default constructor. Creates an empty Grammar object with the grammar name set to "default"
{
    name = L"default";
}

/**
  * Constructor with name.
  * \param string grammarName - name of the Grammar to be created
  */
Grammar::Grammar(const wstring & grammarName)
{
    name = grammarName;
}

Grammar::Grammar(std::wistream * inputStream) {
    std::wstring statement = L"";
    while(getline(*inputStream, statement, L';')) {
        statement = Grammar::trimString(statement);
        //Remove extra whitespace between characters
        statement = Grammar::replaceAll(statement, L" {2,}", L" ");

        if (Grammar::stringStartsWith(statement, L"#"))
        {
            continue; // Line is a comment, skip it.
        }
        else if (Grammar::stringStartsWith(statement, L"grammar "))
        {
            vector<wstring> parts = Grammar::splitString(statement, L" ");
            setName(parts[1]);
        }
        else if (Grammar::stringStartsWith(statement, L"public <"))
        {
            statement = Grammar::replaceFirst(statement, L"public ", L"");
            vector<wstring> parts = Grammar::splitString(statement, L"=");
            wstring ruleName = Grammar::replaceAll(parts[0], L"<|>", L"");
            ruleName = Grammar::trimString(ruleName);
            Expansion * exp = Grammar::parseExpansionsFromString(parts[1]);
            addRule(shared_ptr<Rule>(new Rule(ruleName, true, shared_ptr<Expansion>(exp))));
        }
        else if (Grammar::stringStartsWith(statement, L"<"))
        {
            vector<wstring> parts = Grammar::splitString(statement, L"=");
            wstring ruleName = Grammar::replaceAll(parts[0], L"<|>", L"");
            ruleName = Grammar::trimString(ruleName);
            Expansion * exp = Grammar::parseExpansionsFromString(parts[1]);
            addRule(shared_ptr<Rule>(new Rule(ruleName, false, shared_ptr<Expansion>(exp))));
        }
    }
}

Grammar::Grammar(wistream & inputStream) {
    std::wstring statement = L"";
    while(getline(inputStream, statement, L';')) {
        statement = Grammar::trimString(statement);
        //Remove extra whitespace between characters
        statement = Grammar::replaceAll(statement, L" {2,}", L" ");

        if (Grammar::stringStartsWith(statement, L"#"))
        {
            continue; // Line is a comment, skip it.
        }
        else if (Grammar::stringStartsWith(statement, L"grammar "))
        {
            vector<wstring> parts = Grammar::splitString(statement, L" ");
            setName(parts[1]);
        }
        else if (Grammar::stringStartsWith(statement, L"public <"))
        {
            statement = Grammar::replaceFirst(statement, L"public ", L"");
            vector<wstring> parts = Grammar::splitString(statement, L"=");
            wstring ruleName = Grammar::replaceAll(parts[0], L"<|>", L"");
            ruleName = Grammar::trimString(ruleName);
            Expansion * exp = Grammar::parseExpansionsFromString(parts[1]);
            addRule(shared_ptr<Rule>(new Rule(ruleName, true, shared_ptr<Expansion>(exp))));
        }
        else if (Grammar::stringStartsWith(statement, L"<"))
        {
            vector<wstring> parts = Grammar::splitString(statement, L"=");
            wstring ruleName = Grammar::replaceAll(parts[0], L"<|>", L"");
            ruleName = Grammar::trimString(ruleName);
            Expansion * exp = Grammar::parseExpansionsFromString(parts[1]);
            addRule(shared_ptr<Rule>(new Rule(ruleName, false, shared_ptr<Expansion>(exp))));
        }
    }
}

Grammar::~Grammar() /// Default destructor, removes its ownership of Rule objects
{
    for(shared_ptr<Rule> & r : rules)
    {
        r.reset(); ///TODO: Double check this, should we really be resetting this?
    }
}

std::wstring Grammar::getName() const {
    return name;
}

void Grammar::setName(const std::wstring & s) {
    name = s;
}

bool Grammar::writeGrammar(std::wofstream & outputStream) const {
    outputStream << getText();
    ///TODO: Make this return value mean something
    return true;
}

/**
  * Adds the specified Rule to the Grammar.
  * \param [in] rule
  */
void Grammar::addRule(shared_ptr<Rule> r)
{
    rules.push_back(r);
}

/**
  * Generates the JSGF grammar and returns it as a string.
  * \return string
  */
wstring Grammar::getText() const
{
    wstring s = L"grammar " + name + L";\n";

    for(shared_ptr<Rule> r : rules)
    {
		std::wstring stmp = r->getRuleString();
        s += stmp + L"\n";
    }

    return s;
}

/**
  * Returns the rule name of the public rule that matches the test string. If no matching public rule is found, an empty string is returned.
  * \param string String to test against public rules.
  * \return string
  */
std::wstring Grammar::getMatchingPublicRule(std::wstring test) const {
    test = Grammar::trimString(test);
    test = Grammar::replaceAll(test, L" {2,}", L" ");
    for(shared_ptr<Rule> r : rules) {
        if(r->isPublic()) {
        Matchvector m = matchesRule(r, test);
            if(m.size() != 0) {
                return r->getRuleName();
            }
        }
    }
    return L"";
}

/** Returns a MatchResult object that contains info about the rule that the test string matches. An empty MatchResult object is returned if no match is found.
  * \param string String to test against the Grammar's rules
  * \return MatchResult
  */
MatchResult Grammar::match(std::wstring test) const {
    test = Grammar::trimString(test);
    test = Grammar::replaceAll(test, L" {2,}", L" ");
    for(shared_ptr<Rule> r : rules) {
        Matchvector m = matchesRule(r, test);
        if(m.size() != 0) {
            return MatchResult(r, m);
        }
    }
    return MatchResult();
}

void Grammar::parseGrammar(wifstream f, Grammar & g) {
    std::wstring statement = L"";
    while(getline(f, statement, L';')) {
        statement = Grammar::trimString(statement);
        //Remove extra whitespace between characters
        statement = Grammar::replaceAll(statement, L" {2,}", L" ");

        if (Grammar::stringStartsWith(statement, L"#"))
        {
            continue; // Line is a comment, skip it.
        }
        else if (Grammar::stringStartsWith(statement, L"grammar "))
        {
            vector<wstring> parts = Grammar::splitString(statement, L" ");
            g.setName(parts[1]);
        }
        else if (Grammar::stringStartsWith(statement, L"public <"))
        {
            statement = Grammar::replaceFirst(statement, L"public ", L"");
            vector<wstring> parts = Grammar::splitString(statement, L"=");
            wstring ruleName = Grammar::replaceAll(parts[0], L"<|>", L"");
            ruleName = Grammar::trimString(ruleName);
            Expansion * exp = Grammar::parseExpansionsFromString(parts[1]);
            g.addRule(shared_ptr<Rule>(new Rule(ruleName, true, shared_ptr<Expansion>(exp))));
        }
        else if (Grammar::stringStartsWith(statement, L"<"))
        {
            vector<wstring> parts = Grammar::splitString(statement, L"=");
            wstring ruleName = Grammar::replaceAll(parts[0], L"<|>", L"");
            ruleName = Grammar::trimString(ruleName);
            Expansion * exp = Grammar::parseExpansionsFromString(parts[1]);
            g.addRule(shared_ptr<Rule>(new Rule(ruleName, false, shared_ptr<Expansion>(exp))));
        }
    }
}

/**
  * Parses a new Grammar object from string s
  * \param [in] string s
  * \return Grammar *
  */
void Grammar::parseGrammarFromString(const wstring & s, Grammar & grammar)
{
    //string noComments = Grammar::replaceAll(s, "(\\#+.*[\\n|\\r|\\v])|([//]+.*[\\n|\\r|\\v])", ""); // Remove all commented out lines
    //vector<string> statements = Grammar::splitString(s, ";"); // Split into statements with each semicolon, NOTE: semicolons within quotes are not protected! Lookbehinds are not supported in C++11 https://stackoverflow.com/questions/14538687/using-regex-lookbehinds-in-c11#14539500
    wstring statement;
	wstringstream ss(s);
	//ss.str(s); //(s, ios_base::in | ios_base::out);
	//ss<<s;

    ///https://www.reddit.com/r/cpp_questions/comments/931zq5/any_way_to_simulate_multi_line_regex_with_c11/
    ///Why can't C++11 just have multiline regex? Ugh.

    while(getline(ss, statement, L';')) { /// Note: Semicolons are not protected!
        statement = Grammar::trimString(statement);
        //Remove extra whitespace between characters
        statement = Grammar::replaceAll(statement, L" {2,}", L" ");

        if (Grammar::stringStartsWith(statement, L"#"))
        {
            continue; // Line is a comment, skip it.
        }
        else if (Grammar::stringStartsWith(statement, L"grammar "))
        {
            vector<wstring> parts = Grammar::splitString(statement, L" ");
            grammar.setName(parts[1]);
        }
        else if (Grammar::stringStartsWith(statement, L"public <"))
        {
            statement = Grammar::replaceFirst(statement, L"public ", L"");
			vector<wstring> parts = Grammar::splitString(statement, L"=");
            wstring ruleName = Grammar::replaceAll(parts[0], L"<|>", L"");
            ruleName = Grammar::trimString(ruleName);
            Expansion * exp = Grammar::parseExpansionsFromString(parts[1]);
            grammar.addRule(std::make_shared<Rule>(ruleName, true, shared_ptr<Expansion>(exp)));
        }
        else if (Grammar::stringStartsWith(statement, L"<"))
        {
            vector<wstring> parts = Grammar::splitString(statement, L"=");
            wstring ruleName = Grammar::replaceAll(parts[0], L"<|>", L"");
            ruleName = Grammar::trimString(ruleName);
            Expansion * exp = Grammar::parseExpansionsFromString(parts[1]);
            grammar.addRule(std::make_shared<Rule>(ruleName, false, shared_ptr<Expansion>(exp)));
        }
    }
}

Expansion * Grammar::parseExpansionsFromString(const wstring & input)
{
    vector<Expansion *> inVector = parseTokensFromString(input);
    vector<Expansion *> outVector;
    parseRuleReferences(inVector, outVector);
    inVector.clear();
    inVector.swap(outVector);
    parseRequiredGroupings(inVector, outVector);
    inVector.clear();
    inVector.swap(outVector);
    parseOptionalGroupings(inVector, outVector);
 	inVector.clear();
    inVector.swap(outVector);
    parseUnaryOperators(inVector, outVector); // Don't swap and clear again, end of scope will destroy the inVector anyways
    return parseAlternativeSets(outVector);
}
//* + {}
void Grammar::parseUnaryOperators(const vector<Expansion *> & expansions, vector<Expansion *> & output)
{
    vector<Expansion *> tempExp;
    //Parse Plus and Unary operators
    //Note: Each symbol cannot be nested
    bool expansionFound = false; //We found an expansion that a Unary operator can be applied to, check to see if the next char is a unary operator
    Expansion * selectedExpansion= nullptr;
    vector<Expansion *>::const_iterator expansionIterator = expansions.begin();

    while(expansionIterator != expansions.end())
    {
        if ((*expansionIterator)->getType() == UNPARSED_SECTION)
        {
            if (expansionFound)
            {
                UnparsedSection * up = (UnparsedSection *) *expansionIterator;
                if (Grammar::stringStartsWith(up->getSection(), L"*"))   // Kleene star operator
                {
                    KleeneStar *ks = new KleeneStar(shared_ptr<Expansion>(selectedExpansion));
                    tempExp.push_back(ks);
                    wstring newUnprocessedText = up->getSection();
                    newUnprocessedText = newUnprocessedText.replace(up->getSection().find(L"*"), 1, L""); // Remove the ) token from the  UnparsedSection
                    UnparsedSection * u = new UnparsedSection(Grammar::trimString(newUnprocessedText));
                    tempExp.push_back(u);
                    expansionFound = false;
                }
                else if (Grammar::stringStartsWith(up->getSection(), L"+"))     // Plus operator
                {
                    PlusOperator *ps = new PlusOperator(shared_ptr<Expansion>(selectedExpansion));
                    tempExp.push_back(ps);
                    wstring newUnprocessedText = up->getSection();
                    newUnprocessedText = newUnprocessedText.replace(up->getSection().find(L"+"), 1, L""); // Remove the ) token from the  UnparsedSection
                    UnparsedSection * u = new UnparsedSection(Grammar::trimString(newUnprocessedText));
                    tempExp.push_back(u);
                    expansionFound = false;
                }
                else
                {
                    tempExp.push_back(selectedExpansion);
                    tempExp.push_back(*expansionIterator);
                    expansionFound = false;
                }
            }
            else
            {
                tempExp.push_back(*expansionIterator);
            }
        }
        else
        {
            if (expansionFound)   // If we already had found an expansion before this and we didnt find a unary operator after it, add it to the vector of processed expansions
            {
                tempExp.push_back(selectedExpansion);
            }
            expansionFound = true;
            selectedExpansion = *expansionIterator;
        }
        expansionIterator++;
    }

    if (expansionFound)   // If we reached the end of the loop with a taggable expansion selected, but didnt add it to the vector of expressions
    {
        tempExp.push_back(selectedExpansion);
    }

    vector<Expansion *> exp = tempExp;

    tempExp.clear();
    bool foundLegalExpansion = false;
    bool foundStart = false;
    bool tagStarted = false;
    wstring currentTag = L"";
    Tag * tagExpansion = nullptr;
    selectedExpansion = nullptr; // Reset the selected Expansion

    expansionIterator = exp.begin();
    //NOTE: A single expansion is allowed to have multiple tags!
    while(expansionIterator != exp.end())
    {
        if (foundLegalExpansion)
        {
            if (foundStart)
            {
                if ((*expansionIterator)->getType() == UNPARSED_SECTION)   // Could contain the ending }
                {
                    UnparsedSection * up = (UnparsedSection *) *expansionIterator;
                    if (Grammar::stringStartsWith(up->getSection(), L"}"))   // Found the end of the tag!
                    {

                        if(!tagStarted)
                        {
                            tagExpansion = new Tag(shared_ptr<Expansion>(selectedExpansion));
                            tagStarted = true;
                        }

                        tagExpansion->addTag(currentTag);
                        currentTag = L""; // Reset the tag string contents
                        wstring upText = up->getSection().replace(up->getSection().find(L"}"), 1, L""); // Remove the } token from the  UnparsedSection
                        upText = Grammar::trimString(upText);

                        if (Grammar::stringEndsWith(upText, L"{"))   // Test to see if another tag follows this one
                        {
                            foundStart = true;
                            upText = upText.substr(0, upText.length() - 1); // Remove the last char, which should be the { symbol
                        }
                        else
                        {
                            tagStarted = false;
                            foundStart = false;
                            tempExp.push_back(tagExpansion);
                            foundLegalExpansion = false;
                        }
                        UnparsedSection * u = new UnparsedSection(Grammar::trimString(upText));
                        tempExp.push_back(u);
                    }
                    else
                    {
                        currentTag += up->getSection();
                        delete up;
                    }
                }
                else
                {
                    currentTag += (*expansionIterator)->getText(); // Add the expansions text contents to the tag string
                    delete *expansionIterator;
                }
            }
            else
            {
                // Looking for a starting bracket {
                if ((*expansionIterator)->getType() == UNPARSED_SECTION)   // May contain the { we're looking for
                {
                    UnparsedSection * up = (UnparsedSection *) *expansionIterator;
                    if (Grammar::stringEndsWith(up->getSection(), L"{"))   // Found the start of the tag!
                    {
                        foundStart = true;
                        wstring newUnprocessedText = up->getSection().substr(0, up->getSection().length() - 1); // Remove the last char, which should be the { symbol
                        UnparsedSection * u = new UnparsedSection(Grammar::trimString(newUnprocessedText));
                        tempExp.push_back(u); // Add the updated UnprocessedText section to the vector
                    }
                    else
                    {
                        tempExp.push_back(selectedExpansion);
                        tempExp.push_back(*expansionIterator);
                        foundLegalExpansion = false;
                    }
                }
                else     // No tag possible for the selected expansion, but check to see if this expansion can be tagged
                {
                    tempExp.push_back(selectedExpansion);
                    if (!((*expansionIterator)->getType() == PLUS_OPERATOR || (*expansionIterator)->getType() == KLEENE_STAR))
                    {
                        selectedExpansion = *expansionIterator;
                        tagStarted = false;
                    }
                    else     // current expansion cannot be tagged, begin search for taggable expansion over again
                    {
                        foundLegalExpansion = false;
                        tempExp.push_back(*expansionIterator);
                    }
                }
            }
        }
        else     // Looking for a expansion that is taggable
        {
            if (!((*expansionIterator)->getType() == PLUS_OPERATOR || (*expansionIterator)->getType() == KLEENE_STAR || (*expansionIterator)->getType() == UNPARSED_SECTION))
            {
                foundLegalExpansion = true; // Found a taggable expansion, select it and start searching for tags
                selectedExpansion = *expansionIterator;
                tagStarted = false;
            }
            else     // Unary operators and UnparsedSections cannot be tagged, pass over them
            {
                tempExp.push_back(*expansionIterator);
            }
        }
        expansionIterator++;
    }

    if (foundLegalExpansion)   // Reached end of loop and had selected a taggable expansion, but no tags found
    {
        tempExp.push_back(selectedExpansion);
    }

    output = tempExp;
}
//[]
void Grammar::parseOptionalGroupings(const vector<Expansion *> & expansions, vector<Expansion *> & returnExpansions)
{
    vector<Expansion *> tempExp;
    vector<Expansion *> exp = expansions;
    vector<Expansion *> children;
    unsigned short nestCount = 0;
    wchar_t startChar = '[';
    wchar_t endChar = ']';

    vector<Expansion *>::iterator expansionIterator = exp.begin();
    while(expansionIterator != exp.end())
    {
        if ((*expansionIterator)->getType() == UNPARSED_SECTION)
        {
            UnparsedSection * up = (UnparsedSection *) *expansionIterator;

            wstring childString;
            wstring outsideString;

			wstring stmp0 = up->getSection();
            unsigned int stringSize = stmp0.length();

			wchar_t *cstr = new wchar_t[stringSize + 1];
			wmemset(cstr, 0, stringSize + 1);
			wmemcpy(cstr, stmp0.c_str(), stringSize);

            for (unsigned int i =0; i < stringSize; i++)
            {
                wchar_t c = cstr[i];
                if (c == startChar)
                {
                    nestCount++;
                    if (nestCount == 1)
                    {
                        if (outsideString.size() > 0)
                        {
                            UnparsedSection * u = new UnparsedSection(outsideString);
                            tempExp.push_back(u);
                        }
                        outsideString = L"";
                        continue;
                    }
                }
                else if (c == endChar)
                {
                    nestCount--;
                    if (nestCount == 0)   // Transition to the end of the string
                    {
                        UnparsedSection * u = new UnparsedSection(childString);
                        children.push_back(u);
                        vector<Expansion *> parsedChildren;
                        parseOptionalGroupings(children, parsedChildren);
                        children.clear(); // Using children as next return value
                        parseUnaryOperators(parsedChildren, children);
                        ///TODO: Fix this
                        shared_ptr<Expansion> child = shared_ptr<Expansion>(parseAlternativeSets(children));
                        OptionalGrouping *og = new OptionalGrouping(child);
                        tempExp.push_back(og);
                        childString = L"";
                        children.clear();
                    }
                }

                if (nestCount >= 1)
                {
                    childString += c;
                }
                else if (c != endChar)
                {
                    outsideString += c;
                }
            }

            if (outsideString.size() > 0)
            {
                UnparsedSection * u = new UnparsedSection(outsideString);
                tempExp.push_back(u);
            }

            if (childString.size() > 0)
            {
                if (nestCount > 0)
                {
                    UnparsedSection * u = new UnparsedSection(childString);
                    children.push_back(u);
                }
                else
                {
                    UnparsedSection * u = new UnparsedSection(childString);
                    tempExp.push_back(u);
                }
            }
            delete *expansionIterator;
			delete[]cstr;
        }
        else
        {
            if (nestCount >= 1)   // Element is part of this grouping's children
            {
                children.push_back(*expansionIterator);
            }
            else
            {
                tempExp.push_back(*expansionIterator);
            }
        }
        expansionIterator++;
    }

    returnExpansions = tempExp;
}
//()
void Grammar::parseRequiredGroupings(const vector<Expansion *> & expansions, vector<Expansion *> & returnExpansions)
{
    vector<Expansion *> tempExp;
    vector<Expansion *> exp = expansions;
    vector<Expansion *> children;
    unsigned short nestCount = 0;
	wchar_t startChar = '(';
	wchar_t endChar = ')';

    vector<Expansion *>::iterator expansionIterator = exp.begin();
    while(expansionIterator != exp.end())
    {
        if ((*expansionIterator)->getType() == UNPARSED_SECTION)
        {
            UnparsedSection * up = (UnparsedSection *) *expansionIterator;
            wstring childString;
            wstring outsideString;
			wstring stmp0 = up->getSection();
            unsigned int stringSize = stmp0.length();

			wchar_t *cstr=new wchar_t[stringSize+1];
			wmemset(cstr, 0, stringSize + 1);
			wmemcpy(cstr, stmp0.c_str(), stringSize);

            for (unsigned int i =0; i < stringSize; i++)
            {
				wchar_t c = cstr[i];
                if (c == startChar)
                {
                    nestCount++;
                    if (nestCount == 1)
                    {
                        if (outsideString.size() > 0)
                        {
                            UnparsedSection * u = new UnparsedSection(outsideString);
                            tempExp.push_back(u);
                        }
                        outsideString = L"";
                        continue;
                    }
                }
                else if (c == endChar)
                {
                    nestCount--;
                    if (nestCount == 0)   // Transition to the end of the string
                    {
                        UnparsedSection * u = new UnparsedSection(childString);
                        children.push_back(u);
                        vector<Expansion *> parsedChildren;
                        parseRequiredGroupings(children, parsedChildren);
                        children.clear(); // Using children as next return value
                        parseOptionalGroupings(parsedChildren, children);
                        parsedChildren.clear(); // Using parsedChildren as next return value
                        parseUnaryOperators(children, parsedChildren);
                        children.clear(); // Using children as next return value
                        ///TODO: Fix this, compiles but ends up destroying all child expansions because new input/output idiom returns void and relies upon references.
                        ///Find other parsing methods that also use the incorrect form.
                        shared_ptr<Expansion> child = shared_ptr<Expansion>(parseAlternativeSets(parsedChildren));
                        RequiredGrouping *rg = new RequiredGrouping(child);
                        tempExp.push_back(rg);
                        childString = L"";
                        children.clear();
                    }
                }

                if (nestCount >= 1)
                {
                    childString += c;
                }
                else if (c != endChar)
                {
                    outsideString += c;
                }
            }

            if (outsideString.size() > 0)
            {
                UnparsedSection * u = new UnparsedSection(outsideString);
                tempExp.push_back(u);
            }

            if (childString.size() > 0)
            {
                if (nestCount > 0)
                {
                    UnparsedSection * u = new UnparsedSection(childString);
                    children.push_back(u);
                }
                else
                {
                    UnparsedSection * u = new UnparsedSection(childString);
                    tempExp.push_back(u);
                }
            }

			delete []cstr;
            delete *expansionIterator;
        }
        else
        {
            if (nestCount >= 1)   // Element is part of this grouping's children
            {
                children.push_back(*expansionIterator);
            }
            else
            {
                tempExp.push_back(*expansionIterator);
            }
        }
        expansionIterator++;
    }

    returnExpansions = tempExp;
}

void Grammar::parseRuleReferences(const vector<Expansion *> & expansions, vector<Expansion *> & returnExpansions)
{
    //Parse Rule References because they have next highest precedence
    //Pattern: "<"+TOKEN+">"
    vector<Expansion *> tempExp; // Temporary vector that will be copied into exp
    vector<Expansion *> exp = expansions;
    bool startSearch; // True = looking for a <
    bool endSearch; // True = looking for a >
    bool tokenSearch; // True = hoping that the next Expansion is a Token object containing the name of the rule that is being referenced
    Token * selectedToken; // Only set to null to avoid compiler warnings.
	unsigned short iterationCount = 0;

    bool iterationNeeded = true;
    while (iterationNeeded)
    {
        vector<Expansion *>::iterator expansionIterator = exp.begin();
        iterationNeeded = false;
        tempExp.clear();
        startSearch = true;
        endSearch = false;
        tokenSearch = false;
        selectedToken = nullptr;

        while(expansionIterator != exp.end())
        {
            if (startSearch)
            {
                if ((*expansionIterator)->getType() == UNPARSED_SECTION)
                {
                    UnparsedSection * up = (UnparsedSection *) *expansionIterator;
                    if (Grammar::stringEndsWith(up->getSection(), L"<"))
                    {
                        startSearch = false;
                        tokenSearch = true;
                        //Found the < that starts the rule reference, so we need to remove it from the old UnparsedSection and add the new  UnparsedSection to the vector
                        wstring newUnprocessedText = up->getSection().substr(0, up->getSection().length() - 1); // Remove the last char, which should be the < symbol
                        UnparsedSection * u = new UnparsedSection(Grammar::trimString(newUnprocessedText));
                        tempExp.push_back(u); // Add the updated UnprocessedText section to the vector
                    }
                    else
                    {
                        tempExp.push_back(up); // UnparsedSection does not end in a < so it must not contain the start to a rule reference
                    }
                }
                else
                {
                    tempExp.push_back(*expansionIterator); // Not an UnparsedSection with text, continue on
                }
            }
            else if (endSearch)
            {
                if ((*expansionIterator)->getType() == UNPARSED_SECTION)
                {
                    UnparsedSection * up = (UnparsedSection *) *expansionIterator;
                    if (Grammar::stringStartsWith(up->getSection(), L">"))
                    {
                        endSearch = false;
                        startSearch = true;
                        RuleReference * rr = new RuleReference(selectedToken->getText());
                        tempExp.push_back(rr);
                        //Found the > that ends the rule reference, so we need to remove it from the old  UnparsedSection and add the new  UnparsedSection to the vector
                        wstring newUnprocessedText = up->getSection().replace(up->getSection().find(L">"), 1, L""); // Remove the > token from the  UnparsedSection
                        UnparsedSection * u = new UnparsedSection(Grammar::trimString(newUnprocessedText));
                        tempExp.push_back(u);
                        iterationNeeded = true;
                    }
                    else
                    {
                        throw L"Found start of rule reference, but did not find > immediately afterwards! found: " + (*expansionIterator)->getText();
                    }
                }
                else
                {
                    throw L"Found start of rule reference, but did not find rule name immediately afterwards! found: " + (*expansionIterator)->getText();
                }
            }
            else if (tokenSearch)
            {
                if ((*expansionIterator)->getType() == TOKEN)
                {
                    endSearch = true;
                    tokenSearch = false;
                    selectedToken = (Token *) *expansionIterator;
                }
                else
                {
                    throw L"Found < character denoting rule reference, but did not find rule name immediately afterwards! found: " + (*expansionIterator)->getText();
                }
            }
            else
            {
                tempExp.push_back(*expansionIterator);
            }

            expansionIterator++;
        }
        exp.swap(tempExp);
        tempExp.clear();
        iterationCount++;
    }

    returnExpansions = exp;
}

vector<Expansion *> Grammar::parseTokensFromString(std::wstring part)
{
    vector<Expansion *> exp;
    //Parse Tokens because they have the highest precedence
    wstring passed; // All characters that are not part of a token
    unsigned int position = 0;
    bool escapedMode; // If turned true, the next character will be added to the currentToken string without being evaluated
    bool quotedMode; // If turned true, all characters between quotes are added to the currentToken string
	wchar_t quoteType; // Which quote character to seek for to signify end of quote
    bool tokenMode = false; // If True, test and add the following characters to the currentToken string
    wstring currentToken;

    unsigned int stringLength = part.length();

    wchar_t * charArray = new wchar_t[stringLength + 10];
	wmemset(charArray, 0, stringLength + 10);
	std::wcscpy(charArray, part.c_str());
    //strcpy(charArray, part.c_str());

	wchar_t a; // This holds the current character that is being scanned
    while (position < stringLength)
    {
        escapedMode = false;
        quotedMode = false;
        quoteType = L'\'';
        tokenMode = false;
        currentToken = L""; // This holds the string of characters that are being scanned into one Token
        a = charArray[position];

        while (!tokenMode && position < stringLength)
        {
            a = charArray[position];
            if(!Grammar::isSpecialCharacter(a))
            {
                UnparsedSection * up = new UnparsedSection();
                up->setSection(Grammar::trimString(passed));
                exp.push_back(up);
                passed = L"";
                tokenMode = true;
                //DO NOT INCREMENT THE POSITION COUNTER, WE ARE LETTING THE NEXT LOOP EVALUATE THIS CHARACTER
            }
            else
            {
                passed += a;
                position++;
            }
        }

        if (!tokenMode)   // We reached the end of the string without finding a token, so add what we passed over
        {
            UnparsedSection * up = new UnparsedSection();
            up->setSection(Grammar::trimString(passed));
            exp.push_back(up);
            passed = L"";
        }

        while (tokenMode && position < stringLength)
        {
            a = charArray[position]; // Retrieve the current char we are using
            if (escapedMode)   // escape mode ensures that the next character is not processed literally
            {
                escapedMode = false;
                currentToken += a;
                position++;
            }
            else
            {
                if (a == L'\\')   // Test to see if char triggers escape mode
                {
                    escapedMode = true;
                    position++;
                }
                else if (quotedMode)     // Token is either like: "blarg" or: 'blarg'
                {
                    if (a == quoteType)
                    {
                        quotedMode = false;
                        tokenMode = false;
                        currentToken += quoteType; // The last character is a part of the token (Either ' or " )
                        position++; // Last character was added to the token, so it was processed, increment the position counter
                        //Entire token has now been scanned into currentToken
                        Token * t = new Token(currentToken);
                        exp.push_back(t);
                        currentToken = L"";
                    }
                    else     // Characters within the quotes
                    {
                        currentToken += a;
                        position++;
                    }
                }
                else if (a == L'\"')
                {
                    quotedMode = true;
                    quoteType = L'\"';
                    currentToken += a;
                    position++;
                }
                else if (a == L'\'')
                {
                    quotedMode = true;
                    quoteType = '\'';
                    currentToken += a;
                    position++;
                }
                else if (Grammar::isSpecialCharacter(a)) {// Check to see if char matches special characters
                    tokenMode = false;
                    //Entire token has now been scanned into currentToken
                    Token * t = new Token(currentToken);
                    exp.push_back(t);
                    currentToken = L"";
                    passed = a;
                    position++;
                }
                else
                {
                    currentToken += a;
                    position++;
                }
            }
        }
        if (tokenMode)   // Reached end of string before end of token
        {
            Token * t = new Token(currentToken);
            exp.push_back(t);
            currentToken = L"";
        }
    }
    delete[] charArray;

    if (!tokenMode)   // We reached the end of the string without finding a token, so add what we passed over
    {
        UnparsedSection * up = new UnparsedSection();
        up->setSection(Grammar::trimString(passed));
        exp.push_back(up);
    }

    Grammar::trimUnparsedSections(exp);

    return exp;
}

bool Grammar::isEmptyUnparsedSection(Expansion * e) {
	if(e->getType() == UNPARSED_SECTION) {
		UnparsedSection * u = (UnparsedSection *) e;
		std::wstring s = u->getSection();
		if(s.size() == 0) {
			delete e;
			return true;
		}
		if(s == L" ") {
			delete e;
			return true;
		}
		if(s == L"  ") {
			delete e;
			return true;
		}
		if(s == L"   ") {
			delete e;
			return true;
		}
		u->setSection(trimString(s));
		return false;
	}
	else {
		return false;
	}
}

void Grammar::trimUnparsedSections(std::vector<Expansion *> & exp) {
	exp.erase(std::remove_if(exp.begin(), exp.end(), Grammar::isEmptyUnparsedSection), exp.end());
}

Expansion * Grammar::parseAlternativeSets(vector<Expansion *> & exp) {
    //vector<Expansion *> tempExp;
    //Remove all leftover UnparsedSections
    vector<Expansion *>::iterator expansionIterator;

    trimUnparsedSections(exp);

    Sequence * currentSequence = new Sequence(); // CREATED ON THE FREE STORE!
    AlternativeSet * aset = new AlternativeSet(); // CREATED ON THE FREE STORE!
    expansionIterator = exp.begin();

    while (expansionIterator != exp.end())
    {
        if ((*expansionIterator)->getType() == UNPARSED_SECTION)
        {
            UnparsedSection * up = (UnparsedSection *) *expansionIterator;
            if (Grammar::stringContains(up->getSection(), L"|"))
            {
                shared_ptr<Expansion> a;
                if(currentSequence->childCount() == 1)
                {
                    a = currentSequence->getChild();
                    delete currentSequence;
                }
                else {
					a.reset(currentSequence);
                }

                aset->addChild(a);
                currentSequence = new Sequence();
            }
            else
            {
                currentSequence->addChild(std::shared_ptr<Expansion> (up));
            }
        }
        else
        {
            currentSequence->addChild(std::shared_ptr<Expansion>(*expansionIterator));
        }
        expansionIterator++;
    }

    Expansion * output;
    if (aset->childCount() > 0)
    {
        std::shared_ptr<Expansion> a;
        if(currentSequence->childCount() == 1)
        {
            a = currentSequence->getChild();
            delete currentSequence;
        }
        else {
			a.reset(currentSequence);
        }

        aset->addChild(a);
        output = aset;
    }
    else
    {
        delete aset; // aset has no children yet, and we are not returning it. so it must be deleted, and deleting it won't result in any dangling pointers

        if(currentSequence->childCount() == 1)
        {
            Expansion * e = currentSequence->getChild()->clone();
            delete currentSequence;
            output = e;
        }
        else
        {
            output = currentSequence;
        }
    }

    return output;
}

/**
  * Returns a pointer to a Rule object that has the specified name. Returns nullptr if not rule can be found!
  * \param [in] ruleName
  * \return shared_ptr<Rule>
  * \return nullptr if no rule can be found
  */
shared_ptr<Rule> Grammar::getRule(const wstring & ruleName) const
{
    for(shared_ptr<Rule> r : rules)
    {
        if(r->getRuleName() == ruleName)
        {
            return r;
        }
    }
    return nullptr;
}

/** Returns a vector of shared ptrs to Rules of all the Rules in this Grammar
  *
  */
vector<shared_ptr<Rule>> Grammar::getRules() const {
    return rules;
}

/** Attempts to remove the specified Rule from the list of rules.
  * Returns true on success, returns false if the rule with the specified name could not be found.
  * \param ruleName A string holding the name of the rule that should be removed
  *
  */
bool Grammar::removeRule(const wstring & ruleName) {
    for(vector<shared_ptr<Rule>>::iterator r = rules.begin(); r != rules.end(); r++) {
        if((*r)->getRuleName() == ruleName) {
            rules.erase(r);
            return true;
        }
    }
    return false;
}

/** Walks through all of the expansions in every rule. Calls the callback function for each expansion it walks across.
  * \param callback A void returning function that expects a pointer to an Expansion for its args. The expansion pointer is the expansion that was encountered
  *
  *
  */
void Grammar::walkGrammar(void (* callback)(Expansion *)) {
    for(shared_ptr<Rule> r : rules) {
        walkExpansion(r->getRuleExpansion().get(), callback);
    }
}

/// Walks through the given expansion and all of its children. Calls the callback function for each expansion it walks across.
void Grammar::walkExpansion(Expansion * e, void (* callback)(Expansion *)) {
    callback(e);
    if(e->hasChild()) {
        for(unsigned int i = 0; i < e->childCount(); i++) {
            Grammar::walkExpansion(e->getChild(i).get(), callback);
        }
    }
}

/**
  * Matches the array of word strings against the specified Expansion. Mainly used internally. Use Grammar::matchedRule() for testing for matches.
  * \param [in] e The Expansion being tested against
  * \param [in] words[] array of strings that are being matched against the Expansion
  * \param [in] wordCount Number of words that are being matched (usually is the the length of words[])
  * \param [in] wordPosition Index of the word to start matching at
  * \return vector<shared_ptr<MatchInfo>> - std::vector of MatchInfo objects
  */
vector<shared_ptr<MatchInfo>> Grammar::getMatchingExpansions(shared_ptr<Expansion> e, wstring words[], unsigned int wordCount, unsigned int wordPosition) const
{
    vector<shared_ptr<MatchInfo>> matchvector;

    if ((e.get())->getType() == TOKEN)
    {
        Token * t = (Token *) e.get();
        if (t->getText() == (words[wordPosition]))
        {
            wstring matchedPart = words[wordPosition];
            matchvector.push_back(shared_ptr<MatchInfo>(new MatchInfo(shared_ptr<Expansion>(e), words[wordPosition])));
        }
        else
        {
            // No match
        }
    }
    else if ((e.get())->getType() == RULE_REFERENCE)
    {
        RuleReference * ref = (RuleReference *) e.get();

        Rule * rule = getRule(ref->getRuleName()).get(); // Try to get the rule
        if(rule)
        {
            vector<shared_ptr<MatchInfo>> m1 = (getMatchingExpansions(rule->getRuleExpansion(), words, wordCount, wordPosition));
            if (m1.size() != 0)
            {
                matchvector.push_back(std::make_shared<MatchInfo>(e, L"")); // Need to mark that the rule was matched!
                matchvector.insert(matchvector.end(), m1.begin(), m1.end());
            }
        }
    }
    else if ((e.get())->getType() == OPTIONAL_GROUPING)
    {
        OptionalGrouping * og = (OptionalGrouping *) e.get();
        vector<shared_ptr<MatchInfo>> m1 = getMatchingExpansions(og->getChild(), words, wordCount, wordPosition);
        if (m1.size() == 0)
        {
            // Optional, so it can match. Used for sequences
        }
        else
        {
            //Matches
            matchvector.push_back(shared_ptr<MatchInfo>(new MatchInfo(shared_ptr<Expansion>(e), L"")));
            vector<shared_ptr<MatchInfo>> moreMatches = getMatchingExpansions(og->getChild(), words, wordCount, wordPosition);
            matchvector.insert(matchvector.end(), moreMatches.begin(), moreMatches.end());
        }
    }
    else if ((e.get())->getType() == REQUIRED_GROUPING)
    {
        RequiredGrouping * rg = (RequiredGrouping *) e.get();
        vector<shared_ptr<MatchInfo>> m1 = getMatchingExpansions(rg->getChild(), words, wordCount, wordPosition);

        if (m1.size() != 0)
        {
            matchvector.push_back(shared_ptr<MatchInfo>(new MatchInfo(shared_ptr<Expansion>(e), L"")));
            matchvector.insert(matchvector.end(), m1.begin(), m1.end());
        }
    }
    else if (e.get()->getType() == TAG)
    {
        Tag * t = (Tag *) e.get();
        vector<shared_ptr<MatchInfo>> m1 = getMatchingExpansions(t->getChild(), words, wordCount, wordPosition);

        if (m1.size() != 0)
        {
            matchvector.push_back(shared_ptr<MatchInfo>(new MatchInfo(shared_ptr<Expansion>(e), L"")));
            matchvector.insert(matchvector.end(), m1.begin(), m1.end());
        }
    }
    else if ((e.get())->getType() == ALTERNATE_SET)
    {
        AlternativeSet * as = (AlternativeSet *) e.get();
        for (shared_ptr<Expansion> x : as->getChildren())
        {
            vector<shared_ptr<MatchInfo>> m1 = getMatchingExpansions(x, words, wordCount, wordPosition);

            if (m1.size() == 0 && (x->getType() == KLEENE_STAR || x->getType() == OPTIONAL_GROUPING))   // Stupid OptionalGrouping
            {
                continue;
            }

            if (m1.size() != 0)
            {
				matchvector.push_back(shared_ptr<MatchInfo>(new MatchInfo(shared_ptr<Expansion>(e), L"")));
                matchvector.insert(matchvector.end(), m1.begin(), m1.end()); // Found a match! Add it to the vector
                break;
            }
        }
    }
    else if ((e.get())->getType() == SEQUENCE)
    {
        Sequence * seq = (Sequence *) e.get();
        vector<shared_ptr<MatchInfo>> localMatchvector;
        vector<shared_ptr<Expansion>> expansions = seq->getChildren();
        unsigned int matchedCount = 0;

        for (shared_ptr<Expansion> x : expansions)
        {
            vector<shared_ptr<MatchInfo>> m1 = getMatchingExpansions(x, words, wordCount, wordPosition);
            if (m1.size() == 0 && (x->getType() == KLEENE_STAR || x->getType() == OPTIONAL_GROUPING))   // Stupid OptionalGrouping
            {
                matchedCount++; // Still counts a match
                continue;
            }

            if (m1.size() != 0)
            {
                matchedCount++;
                for (shared_ptr<MatchInfo> localMatch : m1)
                {
                    if(localMatch->getMatchingSection() != L"")
                    {
                        wordPosition += Grammar::splitString(localMatch->getMatchingSection(), L" ").size();
                    }
                }
                localMatchvector.insert(localMatchvector.end(), m1.begin(), m1.end()); // Found a match! Add it to the vector
            }
            else   // Doesn't match! Sequence aborted.
            {
                localMatchvector.clear();
                break;
            }

            if (wordPosition > wordCount - 1)   // Sequence is longer than provided words! Abort!
            {
                break;
            }
        }

		//bool bisError = (matchedCount != seq->childCount());

		int isOptioalCnt = seq->LastChildIsOptioalCnt();

		if (localMatchvector.size() != 0 || matchedCount == (seq->childCount() - isOptioalCnt))
		{
			matchvector.push_back(shared_ptr<MatchInfo>(new MatchInfo(shared_ptr<Expansion>(e), L"")));
			matchvector.insert(matchvector.end(), localMatchvector.begin(), localMatchvector.end());
		}

		else if (matchedCount != seq->childCount())   // Not all of the required matches were met! bisLastChildIsOptioal()
		{
			localMatchvector.clear();
		}
		else
		{
			localMatchvector.clear();
		}
    }
    else if ((e.get()->getType()) == KLEENE_STAR)
    {
        KleeneStar * ks = (KleeneStar *) e.get();
        bool done = false;
        vector<shared_ptr<MatchInfo>> m1;
        matchvector.push_back(shared_ptr<MatchInfo>(new MatchInfo(shared_ptr<Expansion>(e), L"")));
        while(!done)
        {
            if (wordPosition > wordCount - 1)
            {
                break;
            }
            m1 = getMatchingExpansions(ks->getChild(), words, wordCount, wordPosition);
            if (m1.size() == 0)
            {
                // No matches
                done = true;
            }
            else
            {
                //Matches
                for (shared_ptr<MatchInfo> mi2 : m1)
                {
                    if(mi2->getMatchingSection() != L"")
                    {
                        wordPosition += Grammar::splitString(mi2->getMatchingSection(), L" ").size();
                    }
                }
                matchvector.insert(matchvector.end(), m1.begin(), m1.end());
                matchvector.push_back(shared_ptr<MatchInfo>(new MatchInfo(shared_ptr<Expansion>(e), L"")));
            }
        }
    }
    else if ((e.get())->getType() == PLUS_OPERATOR)
    {
        PlusOperator * po = (PlusOperator *) e.get();
        bool done = false;
        vector<shared_ptr<MatchInfo>> m1;
        while(!done)
        {
            if (wordPosition > wordCount-1)
            {
                break;
            }
            m1 = getMatchingExpansions(po->getChild(), words, wordCount, wordPosition);
            if (m1.size() == 0)
            {
                // No matches
                done = true;
            }
            else
            {
                //Matches
                matchvector.push_back(shared_ptr<MatchInfo>(new MatchInfo(shared_ptr<Expansion>(e), L"")));
                for (shared_ptr<MatchInfo> mi2 : m1)
                {
                    if(mi2->getMatchingSection() != L"")
                    {
                        wordPosition += Grammar::splitString(mi2->getMatchingSection(),L" ").size();
                    }
                }
                matchvector.insert(matchvector.end(), m1.begin(), m1.end());
            }
        }
    }

    return matchvector;
}


/**
  * Matches a string of words (test) against the specified Rule.
  * If the test string does not match the Rule, will return a vector with a length/size of 0.
  * \param [in] rule Rule being matched against
  * \param [in] test std::string being matched
  * \return vector<shared_ptr<MatchInfo>> - vector of MatchInfo objects. Can have a size of 0 if test string does not match.
  */
vector<shared_ptr<MatchInfo>> Grammar::matchesRule(const shared_ptr<Rule> rule, const wstring & test) const {
    vector<wstring> words = Grammar::splitString(test, L" ");
    wstring* wordArray = &words[0];
    vector<shared_ptr<MatchInfo>> m1 = getMatchingExpansions(rule->getRuleExpansion(), wordArray, words.size(), 0);
    unsigned int matchCount = 0;
    for (shared_ptr<MatchInfo> mi2 : m1) {
        if (mi2->getMatchingSection() != L"") {
            matchCount++;
        }
    }
    if(matchCount != words.size()) { // Must match all the words!
        m1.clear();
    }

    return m1;
}

/**
  * Overloading convenience function for vector<shared_ptr<MatchInfo>> Grammar::matchesRule(shared_ptr<Rule> rule, string test) that accepts a string for the rule name.
  * \param [in] ruleName Name of the Rule to be matched against
  * \param [in] test string that will be matched against the Rule
  * \return vector<shared_ptr<MatchInfo>> - see vector<shared_ptr<MatchInfo>> Grammar::matchesRule(shared_ptr<Rule> rule, string test)
  */
vector<shared_ptr<MatchInfo>> Grammar::matchesRule(const wstring & ruleName, const wstring & test) const {
    shared_ptr<Rule> r = getRule(ruleName);
    if(!r) { // Check to make sure rule exists
        vector<shared_ptr<MatchInfo>> m;
        return m;
    }
    return matchesRule(r, test);
}

vector<wstring> Grammar::getMatchingTags(vector<shared_ptr<MatchInfo>> matchInfo) {
    vector<wstring> matchedTags;
    for(shared_ptr<MatchInfo> m : matchInfo) {
        if(m->getExpansion().get()->getType() == TAG) {
            Tag * t = (Tag *) m->getExpansion().get();
            vector<wstring> s = t->getTags();
            matchedTags.insert(matchedTags.end(), s.begin(), s.end());
        }
    }
    return matchedTags;
}

std::vector<std::wstring> Grammar::getMatchingTags(const std::wstring & test) const {
    MatchResult m = match(test);
    if(m.matches) {
        return m.getMatchingTags();
    }
    std::vector<std::wstring> v;
    return v;
}

/**
  * Static helper function that replaces the first occurrence of regex re in string s with the replacement string
  * \param [in] s string that is being modified
  * \param [in] re A string representing a C regex that will be matches against string s
  * \param [in] replacement A string that will replace the first match of re against string s
  * \return string The modified string with the first occurrence of re replaced with replacement
  */
wstring Grammar::replaceFirst(const wstring & s, const wstring & re, const wstring & replacement)
{
    wregex reg (re);
    return regex_replace(s, reg, replacement, regex_constants::format_first_only);
}

/**
  * Static helper function that replaces all occurrences of regex re in string s with the replacement string
  * \param [in] s string that is being modified
  * \param [in] re A string representing a C regex that will be matches against string s
  * \param [in] replacement A string that will replace the first match of re against string s
  * \return string The modified string with all occurrences of re replaced with replacement
  */
wstring Grammar::replaceAll(const wstring & s, const wstring & re, const wstring & replacement)
{
    wregex reg (re);
    return regex_replace(s, reg, replacement);
}

///Compliments to: https://stackoverflow.com/questions/16749069/c-split-string-by-regex for the below code
vector<wstring> Grammar::splitString(const wstring & s, const wstring & rgx_str)
{
    vector<wstring> elems;

    wregex rgx (rgx_str);

	wsregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
	wsregex_token_iterator end;

    while (iter != end)
    {
        elems.push_back(*iter);
        ++iter;
    }

    return elems;
}

bool Grammar::stringContains(const wstring & part, const wstring & search)
{
    size_t p = part.find(search);
    return p != wstring::npos;
}

bool Grammar::stringStartsWith(const wstring & s, const wstring & test)
{
    return s.find(Grammar::trimString(test)) == 0;
}

bool Grammar::stringEndsWith(const wstring & s, const wstring & test)
{
    return (s.find(Grammar::trimString(test)) == s.length() - test.length()) && s.find(test) != wstring::npos;
}

///Helper function that returns true if char c is one of: ";=<>*+[]()|{}"
bool Grammar::isSpecialCharacter(wchar_t c) {
	return c==L';' || c== L'=' || c== L'<' || c== L'>' ||\
			c== L'*' || c== L'+' || c== L'[' || c== L']' ||\
			c== L'(' || c== L')' || c== L'|' || c== L'{' ||\
			c== L'}' || c== L' ';
}

///Helper function that returns a string representation of the type of Expansion
std::wstring Grammar::printExpansionType(Expansion * e) {
	switch(e->getType()) {
		case UNPARSED_SECTION:
			return L"UNPARSED_SECTION";
			break;
		case KLEENE_STAR:
			return L"KLEENE_STAR";
			break;
		case PLUS_OPERATOR:
			return L"PLUS_OPERATOR";
			break;
		case REQUIRED_GROUPING:
			return L"REQUIRED_GROUPING";
			break;
		case OPTIONAL_GROUPING:
			return L"OPTIONAL_GROUPING";
			break;
		case SEQUENCE:
			return L"SEQUENCE";
			break;
		case TOKEN:
			return L"TOKEN";
			break;
		case ALTERNATE_SET:
			return L"ALTERNATIVE_SET";
			break;
		case TAG:
			return L"TAG";
			break;
		default:
			return L"UNKNOWN TYPE";
			break;
	}
	return L"ERROR";
}

///Thanks for this trimming function: https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

// trim from start
inline std::wstring &Grammar::ltrim(std::wstring &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(Grammar::isspaceMy))));
    return s;
}

// trim from end
inline std::wstring &Grammar::rtrim(std::wstring &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(Grammar::isspaceMy))).base(), s.end());
    return s;
}

int Grammar::isspaceMy(int input)
{
	if (input == 0x20 || input == 0x0c || input == 0x0a || input == 0x0d || input == 0x09 || input == 0x0b)
		return 100;
	return 0;
}

inline wstring Grammar::trimString(wstring input)
{
    return ltrim(rtrim(input));
}
