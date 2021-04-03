#pragma once

#include <string>
#include <vector>
#include <memory>

namespace parser
{
	class CState;

	class CToken
	{
	public:
		CToken(const std::string &a_sToken);
		virtual ~CToken() = default;

		const std::string &getToken() const;		
		// return true if the token was recognized
		bool handle(CState &a_state) const;		

	private:
		virtual void handleToken(CState &a_state) const = 0;
		virtual bool isToken(const CState &a_state) const;
		
	private:
		std::string m_sToken;
	};

	using TOKENS = std::vector<std::unique_ptr<CToken>>;

	TOKENS getTokens();

}