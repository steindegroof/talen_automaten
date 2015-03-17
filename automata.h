/* A simple parser for automaton files
 * This parser does minimal error handling. It will always try to return what it can and will
 * accept various weird and invalid inputs. Some syntax errors will generate an error message,
 * but don't count on these to catch all mistakes.
 * verification whether an automaton is valid should happen on the level of the automaton itself.
**/
#ifndef AUTOMATA_H_
#define AUTOMATA_H_

#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

// CONSTANTS
const std::string deadstatename = "DEAD";
const char separator = '_';
const char padding = '+';
const char epsilon = 'E';


// HELPER FUNCTIONS

// append vectors
void mergeVector(std::vector<std::string>&, const std::vector<std::string>&);

// class representing na abstract automaton
class Automaton {
    protected:
        std::vector<std::string> states;
	std::vector<char> symbols;
	std::multimap<std::pair<std::string, char>, std::string> transitionFunction;
	std::string startState;
        std::vector<std::string> acceptStates;
        // using ostream for export to dot format
        friend std::ostream& operator<<(std::ostream&, const Automaton&);
    public:
        // default constructor
        Automaton();
        // add a symbol to the automaton
        virtual void addSymbol(char);
        // add a state to the automaton
	void addState(std::string);
        // add an accept state to the automaton
        void addAcceptState(std::string);
        // returns whether a given symbol exists in the automaton
        bool hasSymbol(const char&) const;
        // returns wether a given state exists
        bool hasState(std::string) const;
        // returns wether a given state is an accept state
        bool hasAcceptState(std::string) const;
        // returns whether a given transition exists in the automaton
        virtual bool hasTransition(const std::pair<std::string, char>&, const std::string&) const;
        // add a transition to the automaton
	void addTransition(std::pair<std::string, char>, std::string);
        // return a vector with the symbols of the automaton
        std::vector<char> getSymbols() const;
        // return the states reached by inputting a given symbol from the given state
        virtual std::vector<std::string> delta(std::string, char) const;
        // return the states reached by inputting a given symbol from the any of the given states
        std::vector<std::string> delta(std::vector<std::string> states, char symbol) const;
        std::vector<std::string> delta(std::string, std::string) const;
        // return the start state
        std::string getStartState() const;
        // return a vector with all the states in the automaton
        std::vector<std::string> getStates() const;
        // return a vector with all the accept states in the automaton
        std::vector<std::string> getAcceptStates() const;
		//return the multimap from the transition function
		std::multimap<std::pair<std::string, char>, std::string> getTransitionFunction();
        void setStates(std::vector<std::string>);
        void setSymbols(std::vector<char>);
        void setTransitionFunction(std::multimap<std::pair<std::string, char>, std::string>);
        void setStartState(std::string);
        void setAcceptStates(std::vector<std::string>);
        virtual void convertToDFA(Automaton&);
};

class DFA: public Automaton {
    public:
        bool hasTransition(std::pair<std::string, char>, std::string);
};


class NFA: public Automaton {
    protected:
        // generates a new unique state name for a vector of state names
        std::string generateStateName(std::vector<std::string>);
        // generates a new state name that doesn't exist yet (simple integers going up)
        std::string generateStateName();
        // generate a name for a dead state, based on the const deadstatename, 
        // but avoiding existing state nams
        std::string generateDeadStateName();
        // check if a group of states contains at least one accept state
        bool containsAcceptState(std::vector<std::string>);
        // recursively adds states to the DFA by looping over the alphabet,
        // adding states according to SSC
        void deltaOverSigma(std::vector<std::string>&, DFA&);
    public:
        // returns an equivalent DFA
        void convertToDFA(DFA&);
};


class ENFA: public NFA {
    private:
        // return the states reached by inputting a given symbol from the given state, without accounting for epsilon
        std::vector<std::string> unclosed_delta(std::string, char) const;
    public:
        // return the states reached by inputting a given symbol from the given state
        std::vector<std::string> delta(std::string, char) const;
        // add a symbol to the alphabet, allowing for epsilon
        void addSymbol(char);
        // return a vector with all the states that form the closure of the given state
        std::vector<std::string> getClosure(std::string) const;
        // returns an equivalent DFA
        void convertToDFA(DFA&);
};


class AutomataParser {
    private:
        AutomataParser(const AutomataParser&);
        AutomataParser operator=(const AutomataParser&);
        std::string filename;
        std::ifstream handle;
        // goes to the next '<' character in the file. returns false if EOF reached
        bool goNextElement();
        // returns the name of the current element
        std::string getElementName();
        void closeElement(std::string);
        std::string readString();
        bool seekTag(std::string);
    public:
        // default constructor
        AutomataParser();
        AutomataParser(std::string);
        ~AutomataParser();
        void loadFile(std::string);
        std::string getType();
        std::vector<std::string> getStates();
	std::vector<char> getSymbols();
	std::multimap<std::pair<std::string, char>, std::string> getTransitionFunction();
	std::string getStartState();
        std::vector<std::string> getAcceptStates();
        Automaton makeAutomaton();
       
};

void printVector(std::vector<std::string>);
std::string convertToRegex(Automaton);

#endif
