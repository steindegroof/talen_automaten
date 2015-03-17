#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "automata.h"
#include <sstream>
#include <assert.h>

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
std::multimap<std::pair<std::string, char>, std::string> Automaton::getTransitionFunction(){
	return this->transitionFunction;
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
// generates a new state name that doesn't exist yet in the NFA 
std::string NFA::generateStateName() {
    int iname = 0;
    std::string name = std::to_string(iname);
    do {
	std::string name = std::to_string(iname);
        iname++;
    } while (this->hasState(name));
    return name;
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

//Christophe:
//REGEX -> DFA (via State Elimination)
void printVector(std::vector<std::string> vector){
	std::vector<std::string>::iterator it;
	std::cout << "This vector contains:" << std::endl;
	for( it = vector.begin(); it!=vector.end(); it++){
		std::cout << *it << std::endl;
	}
	std::cout << std::endl;
}

std::multimap<std::pair<std::string, std::string>, std::string> convertTransitionFunction(std::multimap<std::pair<std::string, char>, std::string> transitionFunction){
	std::multimap<std::pair<std::string, std::string>, std::string> newTransitionFunction;
	std::multimap<std::pair<std::string, char>, std::string>::iterator it;
	for( it = transitionFunction.begin(); it!=transitionFunction.end(); it++){
		std::pair<std::string,char> x = it->first;
		std::string p1 = std::get<0>(x);
		char p2 = std::get<1>(x);
		std::string p3;
		std::stringstream ss;
		ss << p2;
		ss >> p3;
		//transitionfunction.insert(std::pair<std::pair<std::string, char>, std::string>(arrow, state2));
		std::pair<std::string,std::string> paartje (p1,p3);
		newTransitionFunction.insert(std::pair<std::pair<std::string,std::string>,std::string>(paartje,it->second));
	}
	return newTransitionFunction;
}

void printTransitionFunction(std::multimap<std::pair<std::string, std::string>, std::string> t){
	std::multimap<std::pair<std::string, std::string>, std::string>::iterator it;
	for(it = t.begin(); it!= t.end(); it++){
		std::pair<std::string,std::string> paar = it->first;
		std::string eindstaat = it->second;
		std::string beginstaat = std::get<0>(paar);
		std::string regex = std::get<1>(paar);
		std::cout << "beginstaat:" << beginstaat <<" ,regex:" << regex << " ,eindstaat:" << eindstaat << std::endl;
	}
}

struct Pos{
	int beginhaak;
	int lengte;
	Pos(int begin,int l){
		beginhaak = begin;
		lengte = l;
	}
};

std::string simplify_base_case(std::string s){ //alleen maar haakjes binnenkrijgen
	//std::cout << "base case is:" << s << std::endl;
	int length = 0;
	std::string::iterator it;
	for( it = s.begin(); it!= s.end(); it++){
		length++;
	}
	if(length == 3){ // + moet " " worden
		if(s.substr(1,1) != "+"){
			return s.substr(1,1);
		}
		else{
			return " ";
		}
	}
	else{ //langer dan (x)
		//std::size_t found = s.find(" ");
		//std::cout << "gevonden op: " << found << std::endl;
		while(true){
			int position = 0;
			bool stop = true;
			for( it = s.begin(); it!= s.end(); it++){
				if(*it == ' '){
					std::string::iterator it2;
					int l = 0;
					int haakjesrechts = 0;
					for(it2 = it; it2!=s.end(); it2++){ //naar rechtse einde zoeken.
						if(*it2 == ')' ){
							if(haakjesrechts == 0){
								break;
							}
							else{
								haakjesrechts--;
							}
						}
						else if(*it2 == '(' ){
							haakjesrechts++;
						}
						else if(*it2 == '+' && haakjesrechts == 0){
							l++; //want dan nemen we de + mee weg.
							break;
						}
						l++;
					}
					//als we breaken met position en lengte verder werken.
					int lengte = l;
					//std::cout << "position = " << position << "en lengte = " << l << std::endl;
					//std::cout << "we hebben nu " << s.substr(begin,lengte) << std::endl;
					l = 0;
					int haakjeslinks = 0;
					for(it2 = it; it2 != s.begin(); it2--){ //naar linkse einde zoeken.
						if(*it2 == '(' ){
							if(haakjeslinks == 0){
								break;
							}
							else{
								haakjeslinks--;
							}
						}
						else if(*it2 == ')' ){
							haakjeslinks++;
						}
						else if(*it2 == '+' && haakjeslinks == 0){
							l++; //want dan nemen we de + mee weg.
							break;
						}
						l++;
					}
					int negatieve_lengte = l;
					//std::cout << "we select: " << s.substr(position-negatieve_lengte+1,negatieve_lengte+lengte-1) << std::endl;
					//we gaan het nu verwijderen:
					s.replace(position-negatieve_lengte+1,negatieve_lengte+lengte-1,"");
					stop = false;
					break;
				}
				position++;
			}
			if(stop){
				break; //break dor de while(true);
			}
		}
	}
	return s; //niets doen
}

std::vector<Pos> simplify_2(std::string s){
	std::vector<Pos> pairs;
	std::vector<int> stack; //hier slaan we posities '(' in op
	std::string::iterator s_it;
	int pos = 0;
	for(s_it = s.begin(); s_it != s.end(); s_it++){
		if(*s_it == '('){
			stack.push_back(pos);
		}
		if(*s_it == ')'){
			int eerstehaak = stack.back();
			Pos paar = Pos(eerstehaak,pos-eerstehaak+1);
			pairs.push_back(paar);
			stack.pop_back();
		}
		pos++;
	}
	//de paren zijn nu opgebouwd
	/*
	std::vector<Pos>::iterator pos_it;
	for(pos_it = pairs.begin(); pos_it!=pairs.end();pos_it++){
		Pos i = *pos_it;
		std::string input = s.substr(i.beginhaak,i.lengte);
		std::cout << input << std::endl;
	}*/
	return pairs;
}

std::string simplify_parentheses(std::string s){
	while(true){
		//std::cout << "ik krijg binnen:" << s << std::endl;
		int length = 0;
		std::string::iterator it;
		for( it = s.begin(); it!= s.end(); it++){
			length++;
		}
		bool done = true;
		std::vector<Pos> pairs = simplify_2(s);
		std::vector<Pos>::iterator p_it;
		for(p_it = pairs.begin(); p_it != pairs.end(); p_it++){
			std::string left;
			std::string right;
			Pos i = *p_it;
			if(i.beginhaak == 0){
				left = "";
			}
			else{
				left = s.substr(i.beginhaak-1,1); //links van haak
				//std::cout << "left: " << left << std::endl;
			}
			if(i.beginhaak+i.lengte == length){
				right = "";
			}
			else{
				right = s.substr(i.beginhaak+i.lengte,1); //rechts van haak
				//std::cout << "right: " << right << std::endl;
			}
			if( (left == ")") || (right == "(" || right == "*")){
				//dan mag het niet
			}
			else{
				s.replace(i.beginhaak,i.lengte,s.substr(i.beginhaak+1,i.lengte-2));
				done = false;
				break;
			}
		}
		if(done){
			break;
		}
	}
	return s;
}

std::string simplify(std::string s){
	while(true){
		std::string::iterator start;
		start = s.begin();
		if(*start == '+'){
			s.replace(0,1,"");
		}
		std::string toReplace1 = "( )*";
		std::size_t found = s.find(toReplace1);
		while(found != std::string::npos){
			s.replace(found,toReplace1.length(),"");
			found = s.find(toReplace1);
		}
		std::string toReplace2 = "()*";
		std::size_t found2 = s.find(toReplace2);
		while(found2 != std::string::npos){
			s.replace(found2,toReplace2.length(),"");
			found2 = s.find(toReplace2);
		}
		//einde zoeken en elimineren ( )*
		std::vector<Pos> pairs = simplify_2(s); //we hebben nu de pairs
		std::vector<Pos>::iterator p_it;
		std::string output;
		bool done_something = false;
		for(p_it = pairs.begin(); p_it != pairs.end(); p_it++){
			Pos i = *p_it;
			std::string input = s.substr(i.beginhaak,i.lengte);
			output = simplify_base_case(input);
			if(input != output){
				s.replace(i.beginhaak,i.lengte,output);
				done_something = true;
				break;
			}
		}
		if(!done_something){
			break;
		}
	}
	//nu hebben we enkel nog overtollige haakjes
	return s;
}

void eliminateState(std::string s,std::multimap<std::pair<std::string, std::string>, std::string>& regexTransitionFunction){
//eliminate this state.
	//bereken alle q's, states die een pijl naar de te elimineren staat hebben.
	std::vector<std::pair<std::string,std::string>> Q; //bevat vertrekstaat met regex
	std::vector<std::pair<std::string,std::string>> P; //(staat,regex)
	std::vector<std::string> S; //(regex) staat is toch altijd naar zichzelf (s)
	std::multimap<std::pair<std::string, std::string>, std::string>::iterator it2;
	for(it2 = regexTransitionFunction.begin(); it2!=regexTransitionFunction.end(); it2++){
		std::pair<std::string,std::string> pair = it2->first;
		std::string vertrekstaat = std::get<0>(pair);
		std::string regex = std::get<1>(pair);
		std::string eindstaat = it2->second;
		if(eindstaat == s && vertrekstaat!=s){ //s mag geen q zijn!
			Q.push_back(std::pair<std::string,std::string>(vertrekstaat,regex));
		}
		else if(vertrekstaat == s){
			if(eindstaat == s){ //lus naar zichzelf, s
				S.push_back(regex);
			}
			else{ //vanuit s naar ergens anders (p dus)
				P.push_back(std::pair<std::string,std::string>(eindstaat,regex)); //met een regex ga je naar een eindstaat (volgorde!!)
			}
		}
	}
	//nu hebben we normaal alle pijlen van en naar s
	std::vector<std::pair<std::string,std::string>>::iterator Qit;
	for(Qit = Q.begin(); Qit != Q.end(); Qit++){
		std::vector<std::pair<std::string,std::string>>::iterator Pit;
		for(Pit = P.begin(); Pit != P.end(); Pit++){
			//now get the regex between Q and P if it exists.
			std::string QtoP = ""; //standaard lege taal Rij(boek) lees r,i,j
			std::multimap<std::pair<std::string, std::string>, std::string>::iterator it2; //onnodige lijn?
			std::vector<std::pair<std::string,std::string>> toDelete;
			for(it2 = regexTransitionFunction.begin(); it2!=regexTransitionFunction.end(); it2++){
				std::pair<std::string,std::string> pair = it2->first;
				std::string vertrekstaat = std::get<0>(pair);
				std::string regex = std::get<1>(pair);
				std::string eindstaat = it2->second;
				if(vertrekstaat == std::get<0>(*Qit) && eindstaat == std::get<0>(*Pit)){
					QtoP = regex; //stel de regex gelijk aan de pijl
					toDelete.push_back(pair); //en verwijder deze pijl want we gaan hem verbeteren met regex
				}
			}
			std::vector<std::pair<std::string,std::string>>::iterator crzy;
			for(crzy = toDelete.begin(); crzy != toDelete.end(); crzy++){
				regexTransitionFunction.erase(*crzy);
			}
			toDelete.clear();
			//nu kan het zijn dat we R,i,j gevonden hebben (QtoP)
			// we gaan deze pijlen toeveogen aan transitiefunctie
			std::string newregex = "";
			if(QtoP != ""){
				newregex = "("+QtoP+")"+"+";
			}
			newregex += std::get<1>(*Qit);
			std::vector<std::string>::iterator Sit;
			std::string Sregex = "";
			for(Sit = S.begin(); Sit!=S.end();Sit++){
				if(Sit != S.begin()){
					Sregex += "+";
				}
				Sregex += *Sit;
			}
			if(S.size() == 0){ //loop was vruchteloos, we hebben de lege taal
				Sregex = " ";
			}
			newregex += "("+Sregex+")*"; // pas op! kan leeg worden
			newregex += std::get<1>(*Pit);
			//regex is compleet, maar nu moeten we hem mss nog vereenvoudigen.
			newregex = simplify(newregex);
			std::pair<std::string,std::string> start_en_regex (std::get<0>(*Qit),newregex);
			regexTransitionFunction.insert(std::pair<std::pair<std::string,std::string>,std::string>(start_en_regex,std::get<0>(*Pit)));
		}
	}
	//nu hebben we alle nieuwe pijlen gezet. Nu kunnen we S verwijderen samen met al de pijlen van en naar s.
	std::vector<std::pair<std::string,std::string>> toDelete;
	std::multimap<std::pair<std::string, std::string>, std::string>::iterator st;
	for(st = regexTransitionFunction.begin(); st!=regexTransitionFunction.end(); st++){
		std::pair<std::string,std::string> pair = st->first;
		std::string vertrekstaat = std::get<0>(pair);
		std::string regex = std::get<1>(pair);
		std::string eindstaat = st->second;
		if(vertrekstaat == s || eindstaat == s){ //verwijder dit element
			toDelete.push_back(pair);
		}/*
		else{
			std::cout<<"ik verwijder niet want:" << vertrekstaat << " en " << eindstaat << std::endl;
		}*/
	}
	std::vector<std::pair<std::string,std::string>>::iterator crzy;
	for(crzy = toDelete.begin(); crzy != toDelete.end(); crzy++){
		regexTransitionFunction.erase(*crzy);
	}
	toDelete.clear();
	//alles in transitietabel van s is nu weg.
}


std::string convertToRegex(Automaton a){ //the automaton can be a DFA
	std::string regex = "";
	std::vector<char> symbols = a.getSymbols();
	std::vector<std::string> states = a.getStates();
	std::string startState = a.getStartState();
	std::vector<std::string> acceptStates = a.getAcceptStates();
	//We gaan bereken welke states geen begin of eindstaat zijn. Deze gaan we straks eerst elimineren
	std::vector<std::string> intermediateStates;
	std::vector<std::string>::iterator it;
	for(it = states.begin(); it != states.end(); ++it){
		std::vector<std::string>::iterator it2;
		bool found_in_acceptstates = false;
		for(it2 = acceptStates.begin(); it2!=acceptStates.end(); ++it2){
			if(*it == *it2){
				found_in_acceptstates = true;
			}
		}
		if(*it != startState && !found_in_acceptstates){
			intermediateStates.push_back(*it);
		}		
	}
	//printVector(intermediateStates);

	std::multimap<std::pair<std::string, char>, std::string> transitionFunction = a.getTransitionFunction();
	std::multimap<std::pair<std::string, std::string>, std::string> regexTransitionFunction = convertTransitionFunction(transitionFunction);//nieuwe transitiefunctie.

	//printTransitionFunction(regexTransitionFunction);
	for(it = intermediateStates.begin(); it!=intermediateStates.end(); it++){
		//eliminate this state.
		std::string s = *it;
		eliminateState(s,regexTransitionFunction);
	}
	//printTransitionFunction(regexTransitionFunction);
	//nu moeten we elke acceptoestand als enige overhouden met de begintoestand.
	//we slaan even de transitiefunctie op zoals ze nu is.
	std::multimap<std::pair<std::string, std::string>, std::string> backup_regexTransitionFunction = regexTransitionFunction;
	std::vector<std::string> acceptStates_w_this;
	for(it = acceptStates.begin(); it != acceptStates.end(); it++){
		std::vector<std::string>::iterator fill_it;
		for(fill_it = acceptStates.begin(); fill_it != acceptStates.end(); fill_it++){
			if(*fill_it != *it){
				acceptStates_w_this.push_back(*fill_it); //moet gecleared worden na loop!!!
			}
		}
		//printVector(acceptStates_w_this);
		//acceptStates_w_this bevat nu alle acceptstates buiten de huidige acceptstate waarvoor we willen kijken.
		for(fill_it = acceptStates_w_this.begin(); fill_it != acceptStates_w_this.end(); fill_it++){
			eliminateState(*fill_it,regexTransitionFunction);
		}
		//nu zijn ze allemaal weg buiten de acceptstate en mss nog de beginstate.
		std::multimap<std::pair<std::string, std::string>, std::string>::iterator map_it;
		if(*it == startState){ //speciaal geval R*
			assert(regexTransitionFunction.size() == 1);
			for(map_it = regexTransitionFunction.begin(); map_it != regexTransitionFunction.end(); map_it++){
				std::pair<std::string,std::string> pair = map_it->first;
				std::string Rregex = std::get<1>(pair);
				regex += "+("+Rregex+")*"; //R*
			}
		}
		else{ //two state automaton
			std::string R = " "; //allemaal de lege taal by default
			std::string S = " ";
			std::string T = " ";
			std::string U = " ";
			for(map_it = regexTransitionFunction.begin(); map_it != regexTransitionFunction.end(); map_it++){
				std::pair<std::string,std::string> pair = map_it->first;
				std::string vertrekstaat = std::get<0>(pair);
				std::string Rregex = std::get<1>(pair);
				std::string eindstaat = map_it->second;
				if(vertrekstaat == startState && eindstaat == startState){
					R = Rregex;
				}
				else if(vertrekstaat == startState && eindstaat == *it){
					S = Rregex;
				}
				else if(vertrekstaat == *it && eindstaat == *it){
					U = Rregex;
				}
				else if(vertrekstaat == *it && eindstaat == startState){
					T = Rregex;
				}
			}
			//we hebben nu R,S,T en U. Nu gaan we regex bouwen
			regex += "+((("+R+")+("+S+")("+U+")*("+T+"))*("+S+")("+U+")*"+")";
			regex = simplify(regex);
		}
		//alles klaar voor deze optie, nu alles herstellen
		acceptStates_w_this.clear();
		regexTransitionFunction = backup_regexTransitionFunction;
	}

	//std::cout << simplify("a+b+( )*+b+( )*");
	regex = simplify_parentheses(regex);
	return regex;
}
