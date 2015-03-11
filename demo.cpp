#include <cstdlib>
#include <string>
#include "automata.h"


int main(int argc, char *argv[]) {

    AutomataParser parser("test.fa");
    std::string type = parser.getType();
    std::vector<std::string> states = parser.getStates();
    std::multimap<std::pair<std::string, char>, std::string> transitions = parser.getTransitionFunction();
    std::vector<char> symbols = parser.getSymbols();
    std::string startState = parser.getStartState();
    std::vector<std::string> acceptStates = parser.getAcceptStates();
    std::vector<std::string>::iterator i;

    NFA automaton;
    automaton.setStates(states);
    automaton.setSymbols(symbols);
    automaton.setStartState(startState);
    automaton.setAcceptStates(acceptStates);
    automaton.setTransitionFunction(transitions);

//    Automaton automaton = parser.makeAutomaton();


    DFA convertedtest;
    automaton.convertToDFA(convertedtest); 
    std::ofstream outputfile("dfa_from_testnfa.gv");
    outputfile << convertedtest;
    outputfile.close();
    std::ofstream outputfile2("testnfa.gv");
    outputfile2 << automaton;
    outputfile2.close();


    AutomataParser parser2("enfa.fa");
    type = parser2.getType();
    states = parser2.getStates();
    transitions = parser2.getTransitionFunction();
    symbols = parser2.getSymbols();
    startState = parser2.getStartState();
    acceptStates = parser2.getAcceptStates();

    ENFA enfa;
    enfa.setStates(states);
    enfa.setSymbols(symbols);
    enfa.setStartState(startState);
    enfa.setAcceptStates(acceptStates);
    enfa.setTransitionFunction(transitions);

//    Automaton enfa = parser.makeAutomaton();


    DFA convertedenfa;
    enfa.convertToDFA(convertedenfa); 
    std::ofstream outputfile3("dfa_from_enfa.gv");
    outputfile3 << convertedenfa;
    outputfile3.close();
    std::ofstream outputfile4("enfa.gv");
    outputfile4 << enfa;
    outputfile4.close();


    AutomataParser parser3("nfa.fa");
    type = parser3.getType();
    states = parser3.getStates();
    transitions = parser3.getTransitionFunction();
    symbols = parser3.getSymbols();
    startState = parser3.getStartState();
    acceptStates = parser3.getAcceptStates();

    NFA nfa;
    nfa.setStates(states);
    nfa.setSymbols(symbols);
    nfa.setStartState(startState);
    nfa.setAcceptStates(acceptStates);
    nfa.setTransitionFunction(transitions);

    DFA convertednfa;
    nfa.convertToDFA(convertednfa); 
    std::ofstream outputfile5("dfa_from_nfa.gv");
    outputfile5 << convertednfa;
    outputfile5.close();
    std::ofstream outputfile6("nfa.gv");
    outputfile6 << nfa;
    outputfile6.close();


    return 0;
}
