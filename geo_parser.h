#ifndef __GEO_PARSER__
#define __GEO_PARSER__

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <memory>
#include <sstream>
#include <stack>
#include <set>
#include <string>

enum ParserError
{
	PE_NONE,
	PE_INVALID_VALUE,
	PE_UNCLOSED_QUOTES,
	PE_UNCLOSED_BRACKETS,
	PE_SYNTAX_ERROR
};

enum ParserType
{
	PT_ARRAY,
	PT_STRING,
	PT_PRIMITIVE
};

struct ParserToken
{
	ParserToken* parent = nullptr;
	ParserType type;
	std::string label;
	bool isDouble = false;
	std::vector<std::unique_ptr<ParserToken>> childs;
};

ParserToken* createToken(ParserType a_type, const std::string& a_label)
{
	ParserToken* result = new ParserToken();
	result->type = a_type;
	result->label = a_label;
	if (a_type == PT_PRIMITIVE)
	{
		if (std::find(a_label.begin(), a_label.end(), '.') != a_label.end())
		{
			result->isDouble = true;
		}
	}
	return result;
}

void updateToken(ParserToken& a_token, ParserType a_type, const std::string& a_label)
{
	a_token.type = a_type;
	a_token.label = a_label;
}

bool skip(const char& a_char)
{
	static const std::vector<char> skipChars =
	{
		' ',
		'\t',
		'\n',
		'\r'
	};
	return (std::find(skipChars.begin(), skipChars.end(), a_char) != skipChars.end());
}

ParserError processAlpha(std::string& a_alpha,
						 std::string::const_iterator& a_it,
						 const std::string::const_iterator& a_end)
{
	ParserError result = PE_NONE;
	std::stringstream ss;
	for (;a_it != a_end; ++a_it)
	{
		char c = *a_it;
		if (isalnum(c))
		{
			ss << c;
		}
		else if (c == '[')
		{
			a_alpha = ss.str();
			break;
		}
		else if (skip(c) == true)
		{
			continue;
		}
		else
		{
			result = PE_SYNTAX_ERROR;
		}
	}
	return result;
}

ParserError processDigit(std::string& a_digit,
						 std::string::const_iterator& a_it,
                         const std::string::const_iterator& a_end)
{
	ParserError result = PE_NONE;
	std::stringstream ss;
	for (; a_it != a_end; ++a_it)
	{
		char c = *a_it;
		if (isdigit(c) || c == '.' || c == '+' || c == '-')
		{
			ss << c;
		}
		else if (c == ',' || c == ']')
		{
			a_digit = ss.str();
			--a_it;
			break;
		}
		else if (skip(c) == true)
		{
			continue;
		}
		else
		{
			result = PE_SYNTAX_ERROR;
		}
	}
	return result;
}

ParserError processString(std::string& a_string,
						  std::string::const_iterator& a_it,
						  const std::string::const_iterator& a_end)
{
	ParserError result = PE_NONE;
	std::stringstream ss;
	for (; a_it != a_end; ++a_it)
	{
		char c = *a_it;
		if (c == '\"')
		{
			break;
		}
		else
		{
			ss << c;
		}
	}
	if (a_it == a_end)
	{
		result = PE_UNCLOSED_QUOTES;
	}
	else
	{
		a_string = ss.str();
	}
	return result;
}

void popAndPushToken(std::stack<ParserToken*>& a_stack)
{
	ParserToken* child = a_stack.top();
	a_stack.pop();
	ParserToken* parent = a_stack.top();
	child->parent = parent;
	parent->childs.push_back(std::unique_ptr<ParserToken>(child));
}

ParserError geo_parser(const std::string a_input, ParserToken& a_root)
{
	ParserError result = PE_NONE;
	std::stack<ParserToken*> stack_;
	for (std::string::const_iterator it = a_input.begin();
		 it != a_input.end();
		 ++it)
	{
		char c = *it;
		if (isalpha(c))
		{
			std::string alpha = "";
			result = processAlpha(alpha, it, a_input.end());
			if (result == PE_NONE)
			{
				if (stack_.empty())
				{
					updateToken(a_root, PT_ARRAY, alpha);
					stack_.push(&a_root);
				}
				else
				{
					ParserToken* token = createToken(PT_ARRAY, alpha);
					stack_.push(token);
				}
			}
		}
		else if (isdigit(c) || c == '+' || c == '-')
		{
			std::string digit = "";
            result = processDigit(digit, it, a_input.end());
			if (result == PE_NONE)
			{
				ParserToken* token = createToken(PT_PRIMITIVE, digit);
				stack_.push(token);
			}
			else
			{
				break;
			}
		}
		else if (c == '\"')
		{
			++it;
			std::string str = "";
			result = processString(str, it, a_input.end());
			if (result == PE_NONE)
			{
				ParserToken* token = createToken(PT_STRING, str);
				stack_.push(token);
			}
			else
			{
				break;
			}
		}
		else if (c == ',')
		{
			popAndPushToken(stack_);
		}
		else if (c == ']')
		{
			popAndPushToken(stack_);
		}
		else if (skip(c) == true)
		{
			continue;
		}
		else
		{
			result = PE_SYNTAX_ERROR;
			break;
		}
		if (result != PE_NONE)
		{
			break;
		}
	}
    return result;
}

ParserToken* find(ParserToken* a_node, const std::string& a_key)
{
    if (a_node->label == a_key)
    {
        return a_node;
    }
    else
    {
        for (auto it = a_node->childs.begin(); it != a_node->childs.end(); ++it)
        {
			if ((*it)->type == PT_ARRAY)
			{
				if ((*it)->label == a_key)
				{
					return (*it).get();
				}
				else
				{
					ParserToken* ptrToken = find((*it).get(), a_key);
					if (ptrToken != nullptr)
					{
						return ptrToken;
					}
				}
			}
        }
    }
    return nullptr;
}

#endif /** __GEO_PARSER__ **/
