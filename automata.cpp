#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "automata.h"

// HELPER FUNCTIONS //////////////////////////////////////////////////////////////

// joins the second vector to the first, discarding duplicates
void mergeVector(std::vector<std::string>& vector1, const std::vector<std::string>& vector2) {
    std::vector<std::string>::const_iterator it;
    for (it = vector2.begin(); it != vector2.end(); it++) {
        if (find(vector1.begin(), vector1.end(), *it) == vector1.end()) {
            vector1.push_back(*it);
        }
    }
}

// AUTOMATON CLASS ///////////////////////////////////////////////////////////////

// default constructor
Automaton::Automaton() {
}
// overloading ostream<< for outputting in .dot format
std::ostream& operator<<(std::ostream& os, const Automaton& fa) {
    os << "digraph finite_state_automaton {" << std::endl;
    os << "rankdir=LR;" << std::endl;

    std::stringstream ss;
    std::vector<std::string>::const_iterator it = fa.acceptStates.begin();
    while (true) {
        ss << *it++;
        if (it != fa.acceptStates.end()) {
            ss << " ";
        }
        else {
            break;
        }
    }
    os << "node [shape = doublecircle]; " << ss.str() << std::endl;
    os << "node [shape = point]; emptystartnode" << std::endl;
    os << "node [shape = circle];" << std::endl;
    // set up start arrow
    os << "emptystartnode -> " << fa.getStartState() <<  " [ label = \"start\" ];" << std::endl;
    // loop over states; generate all the arrows for each state
    std::vector<std::string>::const_iterator state;
    for (state = fa.states.begin(); state != fa.states.end(); state++) {
        std::map<std::string, std::vector<char> > arrows;
        std::vector<char>::const_iterator symbol;
        for (symbol = fa.symbols.begin(); symbol != fa.symbols.end(); symbol++) {
            std::vector<std::string> delta = fa.delta(*state, *symbol);
            std::vector<std::string>::iterator resultstate;
            for (resultstate = delta.begin(); resultstate != delta.end(); resultstate++) {
                std::map<std::string, std::vector<char> >::iterator arrow = arrows.find(*resultstate);
                if (arrow == arrows.end()) {
                    std::vector<char> arrowlabel;
                    arrowlabel.push_back(*symbol);
                    arrows.insert(std::make_pair(*resultstate, arrowlabel));
 
                }
                else {
                    arrow->second.push_back(*symbol);
                }
            }
        } 
        // Now write a line for each arrow we collected:
        std::map<std::string, std::vector<char> >::const_iterator arrow;
        for (arrow = arrows.begin(); arrow != arrows.end(); arrow++) {
            std::vector<char>::const_iterator ch;
            std::string label = "";
            for (ch = arrow->second.begin(); ch != arrow->second.end(); ch++) {
                if (label != "") {
                   label += ", ";
                }
                label += *ch;
            }
            os << *state << " -> " << arrow->first << " [ label = \"" << label << "\" ];" << std::endl; 
        }
    }
    os << "}";
    return os;
}
// adds the given states to the automaton, discarding duplicates
void Automaton::setStates(std::vector<std::string> states) {
    std::vector<std::string>::iterator i;
    for (i = states.begin(); i < states.end(); i++) {
        this->addState(*i);
    }
}
// adds the given symbols to the automaton, discarding duplicates
void Automaton::setSymbols(std::vector<char> symbols) {
    std::vector<char>::iterator i;
    for (i = symbols.begin(); i < symbols.end(); i++) {
        this->addSymbol(*i);
    }
}
void Automaton::setTransitionFunction(std::multimap<std::pair<std::string, char>, std::string> transitionfunction) {
    std::multimap<std::pair<std::string, char>, std::string>::iterator it;
    for (it = transitionfunction.begin(); it != transitionfunction.end(); it++) {
        this->addTransition(it->first, it->second);
    }
}
// verifies if the given state is an existing state in the automaton and sets it to start state
void Automaton::setStartState(std::string state) {
    std::vector<std::string>::iterator it;
    it = std::find(this->states.begin(), this->states.end(), state);
    if (it == this->states.end()) {
        std::cerr << "Start state not set: state " << state << " unknown in automaton." << std::endl;
    }
    else {
        this->startState = state;
    }
}
// adds the given accepts states to the automaton if they are present in the states, discarding duplicates
void Automaton::setAcceptStates(std::vector<std::string> states) {
    std::vector<std::string>::iterator i;
    for (i = states.begin(); i < states.end(); i++) {
        this->addAcceptState(*i);
    }
}
// add a symbol to the automaton
void Automaton::addSymbol(char symbol) {
    // disallow epsilon by default
    if (symbol == epsilon or symbol == '\0') {
        std::cerr << "Symbol epsilon disallowed. not added" << std::endl;
    }
    else if (this->hasSymbol(symbol)) {
        std::cerr << "Symbol " << symbol << " already known in automaton, skipping" << std::endl;
    }
    else {
        this->symbols.push_back(symbol);
    }
}

// add a state to the automaton
void Automaton::addState(std::string state) {
    if (this->hasState(state)) {
        std::cerr << "State " << state << " already known in automaton, skipping" << std::endl;
    }
    else {
        this->states.push_back(state);
    }
}
// add an accept state to the automaton
void Automaton::addAcceptState(std::string state) {
    if (this->hasAcceptState(state)) {
        std::cerr << "Accept state " << state << " already known in automaton, skipping" << std::endl;
    }
    else if (this->hasState(state)) {
        this->acceptStates.push_back(state);
    }
    else {
        std::cerr << "Acceptstate '" << state << "' is not a known state in automaton, skipping." << std::endl;
    }
}
// returns whether a given symbol exists in the automaton
bool Automaton::hasSymbol(const char& symbol) const {
    std::vector<char>::const_iterator it;
    it = std::find(this->symbols.begin(), this->symbols.end(), symbol);
    if (it == this->symbols.end()) {
        return false;
    }
    else {
        return true;
    }
}
// returns wether a given state exists
bool Automaton::hasState(std::string state) const {
    std::vector<std::string>::const_iterator i;
    i = std::find(this->states.begin(), this->states.end(), state);
    if (i == this->states.end()) {
        return false;
    }
    else {
        return true;
    }
}
// returns wether a given state is an accept state
bool Automaton::hasAcceptState(std::string state) const {
    std::vector<std::string>::const_iterator i;
    i = std::find(this->acceptStates.begin(), this->acceptStates.end(), state);
    if (i == this->acceptStates.end()) {
        return false;
    }
    else {
        return true;
    }
}
// returns whether a given transition exists in the automaton
bool Automaton::hasTransition(const std::pair<std::string, char>& arrow, const std::string& result) const {
    std::pair<std::multimap<std::pair<std::string, char>, std::string>::const_iterator, std::multimap<std::pair<std::string, char>, std::string>::const_iterator> rangeit = this->transitionFunction.equal_range(arrow);
    std::multimap<std::pair<std::string, char>, std::string>::const_iterator it;
    for (it = rangeit.first; it != rangeit.second; it++) {
        if (it->second == result) {
            return true;
        }
    }
    return false;
}

// add a transition to the automaton
void Automaton::addTransition(std::pair<std::string, char> arrow, std::string result) {
    if (this->hasState(arrow.first) and this->hasSymbol(arrow.second) and this->hasState(result)) {
        if (this->hasTransition(arrow, result)) {
            std::cerr << "The transition (" << arrow.first << "," << arrow.second << ',' << result << 
                         ") exists already, skipping." << std::endl;
        }
        else {
            this->transitionFunction.insert(std::make_pair(arrow, result));
        }
    }
    else {
        std::cerr << "Transition (" << arrow.first << "," << arrow.second << ',' << result <<
                     ") not added because it contains a state or symbol that is unknown in the automaton." << std::endl;
    }
}
// return a vector with the symbols of the automaton
std::vector<char> Automaton::getSymbols() const {
    return this->symbols;
}
// return a vector with the states of the automaton
std::vector<std::string> Automaton::getStates() const {
    return this->states;
}
// return a vector with the accept states of the automaton
std::vector<std::string> Automaton::getAcceptStates() const {
    return this->acceptStates;
}
// return the states reached by inputting a given symbol from the given state
std::vector<std::string> Automaton::delta(std::string state, char symbol) const {
    std::vector<std::string> resultstates;
    if (!this->hasState(state) or !this->hasSymbol(symbol)) {
        std::cerr << "The given state '" << state << "' or symbol '" << symbol << "' doesn't exist." << std::endl;
    }
    else {
        std::pair<std::multimap<std::pair<std::string, char>, std::string>::const_iterator, std::multimap<std::pair<std::string, char>, std::string>::const_iterator> itrange;
        itrange = this->transitionFunction.equal_range(std::make_pair(state, symbol));
        std::multimap<std::pair<std::string, char>, std::string>::const_iterator it;
        for (it = itrange.first; it != itrange.second; it++) {
            resultstates.push_back(it->second);
        }
    }
    return resultstates;
}
// return the states reached by inputting a given symbol from the any of the given states
std::vector<std::string> Automaton::delta(std::vector<std::string> states, char symbol) const {
    std::vector<std::string> resultstates;
    std::vector<std::string>::iterator it;
    for (it = states.begin(); it != states.end(); it++) {
        std::vector<std::string> currentdelta =  this->delta(*it, symbol);
        mergeVector(resultstates, currentdelta);
    }
    return resultstates;
}
// return the states reached by inputting a given string from the given state
std::vector<std::string> Automaton::delta(std::string state, std::string symbols) const {
    std::vector<std::string> resultstates;
    std::string::iterator i;
    for (i = symbols.begin(); i != symbols.end(); i++) {
        std::vector<std::string> subresults = this->delta(state, *i);
        // INCOMPLETE
        // merge with resultstates, remove duplicates
    }
    return resultstates;
}
// return the start state
std::string Automaton::getStartState() const {
    return startState;
}

void Automaton::convertToDFA(Automaton&) {}//empty

//////////////////////////////////////////////////////////////////////////////////
// DFA CLASS /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// returns whether a given transition exists in the DFA 
// this only compares the initial state and symbol, ignoring the result (since 
// only one arrow with a certain symbol is allowed in a DFA)
bool DFA::hasTransition(std::pair<std::string, char> arrow, std::string result) {
    if (this->transitionFunction.count(arrow) == 0) {
        return false;
    }
    return true;
}

// NFA CLASS /////////////////////////////////////////////////////////////////////
// generates a new unique state name for a vector of state names
std::string NFA::generateStateName(std::vector<std::string> states) {
    if (states.size() > 1) {
        std::stringstream ss;
        std::vector<std::string>::iterator it = states.begin();
        while (true) {
            ss << *it++;
            if (it != states.end()) {
                ss << separator;
            }
            else {
                break;
            }
        }
        std::string name = ss.str();
        while (this->hasState(name)) {
            name += padding;
        }
        return name;
    }
    else if (states.empty()) {
        return "";
    }
    else {
        return states[0];
    }
}
// generate a name for a dead state, based on the const deadstatename, 
// but avoiding existing state nams
std::string NFA::generateDeadStateName() {
    std::string deadstate = deadstatename;
    while (this->hasState(deadstate)) {
        deadstate += padding;
    }
    return deadstate;
}

// check if a group of states contains at least one accept state
bool NFA::containsAcceptState(std::vector<std::string> states) {
    std::vector<std::string>::iterator it;
    for (it = states.begin(); it != states.end(); it++) {
        if (this->hasAcceptState(*it)) {
            return true;
        }
    }
    return false;
}
// recursively adds states to the DFA by looping over the alphabet,
// adding states according to SSC 
void NFA::deltaOverSigma(std::vector<std::string>& states, DFA& dfa) {
    std::string state = this->generateStateName(states);
    // only add states if the new state is not yet known (recursion base case)
    if (!dfa.hasState(state)) {
        dfa.addState(state);
        std::vector<char>::iterator it;
        for (it = this->symbols.begin(); it != this->symbols.end(); it++) {
            std::vector<std::string> targetstates = this->delta(states, *it);
            if (targetstates.empty()) {
                // transition to dead state
                targetstates.push_back(this->generateDeadStateName());
                this->deltaOverSigma(targetstates, dfa);
                dfa.addTransition(std::make_pair(state, *it), this->generateDeadStateName());
            }
            else {
                // transition to new state and recurse over new state  
                this->deltaOverSigma(targetstates, dfa);
                std::string newstate = this->generateStateName(targetstates);
                dfa.addTransition(std::make_pair(state, *it), newstate);
                if (this->containsAcceptState(targetstates)) {
                    if (!dfa.hasAcceptState(newstate)) {
                        dfa.addAcceptState(newstate);
                    }
                }
            }
        }
    }
}
// constructs an equivalent DFA
void NFA::convertToDFA(DFA& dfa) {
    dfa.setSymbols(this->symbols);
    std::vector<std::string> startvector;
    startvector.push_back(this->getStartState());
    this->deltaOverSigma(startvector, dfa);
    dfa.setStartState(this->getStartState());
}

//////////////////////////////////////////////////////////////////////////////////
/// EPSILON NFA CLASS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// return the states reached by inputting a given symbol from the given state, without accounting for epsilon
std::vector<std::string> ENFA::unclosed_delta(std::string state, char symbol) const {
    std::vector<std::string> resultstates;
    if (!this->hasState(state) or !this->hasSymbol(symbol)) {
        std::cerr << "The given state '" << state << "' or symbol '" << symbol << "' doesn't exist." << std::endl;
    }
    else {
        std::pair<std::multimap<std::pair<std::string, char>, std::string>::const_iterator, std::multimap<std::pair<std::string, char>, std::string>::const_iterator> itrange;
        itrange = this->transitionFunction.equal_range(std::make_pair(state, symbol));
        std::multimap<std::pair<std::string, char>, std::string>::const_iterator it;
        for (it = itrange.first; it != itrange.second; it++) {
            resultstates.push_back(it->second);
        }
    }
    return resultstates;
}

// return the states reached by inputting a given symbol from the given state
std::vector<std::string> ENFA::delta(std::string state, char symbol) const {
    std::vector<std::string> resultstates;
    std::vector<std::string>::iterator it;
    std::vector<std::string> states = this->getClosure(state);
    for (it = states.begin(); it != states.end(); it++) {
        std::vector<std::string> uncloseddelta =  this->unclosed_delta(*it, symbol);
            std::vector<std::string>::iterator i;
            for (i = uncloseddelta.begin(); i != uncloseddelta.end(); i++) {
                mergeVector(resultstates, getClosure(*i));
            }

        //mergeVector(resultstates, currentdelta);
    }
    return resultstates;
}

// add a symbol to the ENFA
void ENFA::addSymbol(char symbol) {
    // convert null char to epsilon
    if (symbol == '\0') {
        symbol = epsilon;
    }
    if (this->hasSymbol(symbol)) {
        std::cerr << "Symbol " << symbol << " already known in automaton, skipping" << std::endl;
    }
    else {
        this->symbols.push_back(symbol);
    }
}

// return a vector with all the states from the closure of a given state
std::vector<std::string> ENFA::getClosure(std::string state) const {
    std::vector<std::string> closure;
    if (!this->hasState(state)) {
        std::cerr << "State '" << state << "' unknown in automaton while attempting to calculate closure." << std::endl;
        return closure;
    }
    //if (!this->hasTransition(std::make_pair(state, epsilon), state)) {
        closure.push_back(state);
    //}
    std::vector<std::string> nextlevel = this->unclosed_delta(state, epsilon);
    std::vector<std::string>::iterator it;
    for (it = nextlevel.begin(); it != nextlevel.end(); it++) {
        // prevent loops in recursion:
        if (find(closure.begin(), closure.end(), *it) == closure.end()) {
            mergeVector(closure, this->getClosure(*it));
        }
    }
    return closure;
}
// returns an equivalent DFA
void ENFA::convertToDFA(DFA& dfa) {
    dfa.setSymbols(this->symbols);
    std::vector<std::string> startvector = this->getClosure(this->getStartState());
    this->deltaOverSigma(startvector, dfa);
    dfa.setStartState(this->generateStateName(startvector));

}

//////////////////////////////////////////////////////////////////////////////////
/// AUTOMATAPARSER CLASS /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

AutomataParser::AutomataParser() {
}
AutomataParser::AutomataParser(std::string filename) {
    this->filename = filename;
    this->handle.open(filename.c_str());
}
AutomataParser::~AutomataParser() {
    this->handle.close();
}
// position at the next element by finding the next occurence of '<'
// leaves the handle right after the '<'
bool AutomataParser::goNextElement() {
    char ch = '\0';
    while (ch != '<') {
        handle.get(ch);
        if (handle.eof()) {
            return false;
        }
    }
    handle.unget();
    return true;
}
// returns the name of the element at handle position
// leaves the handle right after the closing '>'
std::string AutomataParser::getElementName() {
    std::string retstr = "";
    char ch;
    handle.get(ch);
    if (ch != '<') {
        std::cerr << "Not positioned at '<' character when trying to read element name." << std::endl;
        return "";
    }
    handle.get(ch);

    while (ch != '>' and ch != '\n') {
        retstr += ch;
        handle.get(ch);
        if (ch == '\\') {
            handle.get(ch);
            if (ch == '>') {
                retstr += ch;
                handle.get(ch);
            }
            else {
                retstr += '\\';
            }
        }
    }
    if (ch == '\n') {
        std::cerr << "No closing '>' found for element " << retstr << ". " << std::endl;
    }
    return retstr;
}
// read the closing tag </string>
// leaves the handle right after the closing '>'
void AutomataParser::closeElement(std::string name) {
    char ch;
    handle.get(ch);
    if (ch != '<') {
        std::cerr << "Not positioned at closing tag position when verifying closing tag for " << name << std::endl;
    }
    handle.get(ch);
    if (ch != '/') {
        std::cerr << "Closing tag for " << name << " has no '/' character." << std::endl;
    }

    std::string::iterator i;
    for(i = name.begin() ; i < name.end(); i++){
        handle.get(ch);
        if (ch != *i) {
            std::cerr << "Closing tag name doesn't match " << name << std::endl; 
        }
    }
    handle.get(ch);
    if (ch != '>') {
        std::cerr << "Closing tag doesn't have > after name " << name << std::endl;
        // continue until we find '>' or end of line
        while (ch != '>' and ch != '\n') {
            handle.get(ch);
        }
    }

}
// read a string terminated by ',' or '<' or '\n'.
// leaves the handle at the position right before the terminator
std::string AutomataParser::readString() {
    std::string retstr = "";
    char ch;
    handle.get(ch);
    while (ch != ',' and ch != '<' and ch != '\n') {
        retstr += ch;
        handle.get(ch);
        /*if (ch == '\\') {
            handle.get(ch);
            if (ch == ',' or ch == '<') {
                retstr += ch;
                handle.get(ch);
            }
            else if (ch == '0') {
                retstr += epsilon;
            }
            else {
                retstr += '\\';
                handle.unget();
            }
        }*/
        if (ch == '\0') {
            retstr += epsilon;
        }
    }
    handle.unget();
    return retstr;
}
// find the first occurence of a tag with the given name (starting from whichever position we are at)
// leaves the handle position right after the tag
bool AutomataParser::seekTag(std::string name) {
    std::string cname = "";
    while (cname != name) {
        if (!goNextElement()) {
            std::cerr << "EOF reached without finding " << name << " tag" << std::endl;
            return false;
        }
        cname = getElementName();
    }
    return true;
}
std::string AutomataParser::getType() {
    handle.seekg(0);
    std::string name = "TYPE";
    if (!seekTag(name)) {
        return "";
    }
    std::string type = readString();
    closeElement(name);
    return type;
}
std::vector<std::string> AutomataParser::getStates() {
    std::vector<std::string> states;
    char ch = ',';
    handle.seekg(0);
    std::string name = "STATES";
    if (!seekTag(name)) {
        return states;
    }
    while (ch == ',') {
        std::string state = readString();
        states.push_back(state);
        handle.get(ch);
    }
    handle.unget();
    closeElement(name);
    return states;
}
std::vector<char> AutomataParser::getSymbols() {
    std::vector<char> symbols;
    handle.seekg(0);
    std::string name = "SYMBOLS";
    if (!seekTag(name)) {
        return symbols;
    }
    char ch = handle.get();
    while (ch != '<' and ch != '\n') {
        if (ch == ',') {
            //do nothing
        }
        else if (ch == '\\') {
            handle.get(ch);
            if (ch == '\\' or ch == '<') {
                symbols.push_back(ch);
            }
            else if (ch == '0') {
                symbols.push_back(epsilon);
            }
            else {
                symbols.push_back('\\');
                symbols.push_back(ch);
            }
        }
        else if (ch == '\0') {
            symbols.push_back(epsilon);
        }
        else {
            symbols.push_back(ch);
        }
        handle.get(ch);
    }
    handle.unget();
    closeElement(name);
    return symbols;
}
std::multimap<std::pair<std::string, char>, std::string> AutomataParser::getTransitionFunction() {
    std::multimap<std::pair<std::string, char>, std::string> transitionfunction;
    handle.seekg(0);
    std::string name = "TRANSITIONFUNCTION";
    std::string tname = "T"; // transition tag name
    std::string ename; // element name
    char ch;
    if (!seekTag(name)) {
        return transitionfunction;
    }
    do {
        if (!goNextElement()) {
            std::cerr << "No more transitions or transitionfunction close tag found." << std::endl;
            return transitionfunction;
        }
        ename = getElementName();
        if (ename != "T") {
            if (ename == "/TRANSITIONFUNCTION") {
                // hackish way to detect closing tag
                break;
            }
            std::cerr << "Current element name = " << ename << ", expecting T for transition." << std::endl;
        }
        else {
            std::string state1 = readString();
            handle.get(ch);
            if (ch != ',') {
                std::cerr << "No comma found in transition, skipping and trying to find next <T> tag..." << std::endl;
                handle.unget();
                continue;
            }
            std::string symbol = readString();
            handle.get(ch);
            if (ch != ',') {
                std::cerr << "No second comma found in transition, skipping and trying to find next <T> tag..." << std::endl;
                handle.unget();
                continue;
            }
            std::string state2 = readString();
            closeElement("T");
            if (symbol.length() > 1) {
                std::cerr << "found more than one character for symbol in transition: " << 
                             state1 << ", " << symbol << ", " << state2 << ". Using first char." << std::endl;
            }
            std::pair<std::string, char> arrow(state1, symbol[0]);
            transitionfunction.insert(std::pair<std::pair<std::string, char>, std::string>(arrow, state2));
        }
    } while (!handle.eof());
 
    return transitionfunction;
}
std::string AutomataParser::getStartState() {
    std::string startstate;
    handle.seekg(0);
    std::string name = "STARTSTATE";
    if (!seekTag(name)) {
        return "";
    }
    startstate = readString();
    closeElement(name);
    return startstate;
}
std::vector<std::string> AutomataParser::getAcceptStates() {
    std::vector<std::string> acceptstates;
    char ch = ',';
    handle.seekg(0);
    std::string name = "ACCEPTSTATES";
    if (!seekTag(name)) {
        return acceptstates;
    }
    while (ch == ',') {
        std::string state = readString();
        acceptstates.push_back(state);
        handle.get(ch);
    }
    handle.unget();
    closeElement(name);
    return acceptstates;
}

Automaton AutomataParser::makeAutomaton() {
    Automaton* automaton = new Automaton();
    if (this->getType() == "dfa") {
        DFA* dfa = new DFA();
        dfa->setStates(this->getStates());
        dfa->setSymbols(this->getSymbols());
        dfa->setStartState(this->getStartState());
        dfa->setAcceptStates(this->getAcceptStates());
        dfa->setTransitionFunction(this->getTransitionFunction());
        return *dfa;
    }
    else if (this->getType() == "nfa") {
        NFA* nfa = new NFA();
        nfa->setStates(this->getStates());
        nfa->setSymbols(this->getSymbols());
        nfa->setStartState(this->getStartState());
        nfa->setAcceptStates(this->getAcceptStates());
        nfa->setTransitionFunction(this->getTransitionFunction());
        return *nfa;
    }
    else if (this->getType() == "enfa") {
        ENFA* enfa = new ENFA();
        enfa->setStates(this->getStates());
        enfa->setSymbols(this->getSymbols());
        enfa->setStartState(this->getStartState());
        enfa->setAcceptStates(this->getAcceptStates());
        enfa->setTransitionFunction(this->getTransitionFunction());
        return *enfa;
    }
    std::cerr << "Unknown type of automaton; returning empty object." << std::endl;
    return *automaton;
}
