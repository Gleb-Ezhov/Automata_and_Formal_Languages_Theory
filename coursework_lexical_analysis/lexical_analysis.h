#pragma once
#include "program_analysis.h"
#include "auxiliary_algorithms.h"

struct CodePair
{
	CodePair(int tableNum, int indexNum) : tableNum(tableNum), indexNum(indexNum) {};

	int tableNum;
	int indexNum;

	bool operator == (const CodePair& p)
	{
		if (this->tableNum == p.tableNum && this->indexNum == p.indexNum) return true;
		return false;
	}

	bool operator != (const CodePair& p)
	{
		if (this->tableNum != p.tableNum || this->indexNum != p.indexNum) return true;
		return false;
	}
};

class LexicalAnalysis
{
public:
	LexicalAnalysis(ProgramAnalysis* analysis);
	std::vector<CodePair>& getLexicalOutput() { return lexicalOutput; };

private:
	void lexicalAnalysis();

	char LexicalAnalysis::getCharacterFromInput();
	void LexicalAnalysis::addCodePairToOutput(int tableNum, int indexNum);

	void LexicalAnalysis::IdentifierState(std::string& buffer, char& next);
	void LexicalAnalysis::BinaryState(std::string& buffer, char& next);
	void LexicalAnalysis::OctalState(std::string& buffer, char& next);
	void LexicalAnalysis::DecimalState(std::string& buffer, char& next);
	void LexicalAnalysis::RealState(std::string& buffer, char& next);
	void LexicalAnalysis::ExponentialFormState(std::string& buffer, char& next);
	void LexicalAnalysis::ExponentialFormExtraState(std::string& buffer, char& next);
	void LexicalAnalysis::HexadecimalState(std::string& buffer, char& next);

	ProgramAnalysis* analysis;
	std::string input;
	std::vector<CodePair> lexicalOutput;
};