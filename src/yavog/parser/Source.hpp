#pragma once
#include <string>
#include <vector>

struct Source
{
	std::string origin;
	std::vector<size_t> lineStart;
public:
	std::string content;

	Source(std::string origin,std::string content);
	std::string getOrigin()const;

	struct Position{
		size_t line;
		size_t column;

		size_t lineStartIndex;
	};

	Position getline(size_t i);
	
	void error  (size_t i,size_t length,std::string msg);
	void warning(size_t i,size_t length,std::string msg);
	void hint   (size_t i,size_t length,std::string msg);
	void ref    (size_t i,size_t length,std::string msg);

	//TODO: have multiline codeblocks
	//TODO: show multiple messages. interreferencing each other.
	void printCodeSection(size_t i,size_t length,std::string msg);
};


