#pragma once
#include "lexical_analysis.h"
#include "semantic_analysis.h"

class SyntacticAnalysis
{
public:
	SyntacticAnalysis(ProgramAnalysis* analysis, SemanticAnalysis* semantic, std::vector<CodePair> lexemes);

private:
	void syntacticAnalysis();
	bool variablesDescription();
	void operatorRule();
	void compoundOperatorRule();
	void assignmentOperator();
	void expression();
	void factor();
	void term();
	void operand();
	void conditionalOperator();
	void fixedLoopOperator();
	void conditionalLoopOperator();
	void inputOperator();
	void outputOperator();

	ProgramAnalysis* analysis;
	SemanticAnalysis* semantic;
	std::vector<CodePair> lexemes;
};