#pragma once

#include <filesystem>
#include "token.hpp"
#include "program.hpp"

#include <cstdio>

class Compiler {
public:
	Compiler(void) {
	}

	Program build(const std::filesystem::path &entryPointPath) {
		auto sourceFile = File(entryPointPath);
		auto tokens = TokenParser::readTokens(sourceFile);
		for (auto &token : tokens) {
			if (token.getClass() == TokenClass::StringLiteral)
				std::printf("\"%s\"\n", token.getString().c_str());
			else
				std::printf("%s\n", token.getString().c_str());
		}

		return Program();
	}
};