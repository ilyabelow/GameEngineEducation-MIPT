#include "ConsoleReader.h"

#include <iostream>

ConsoleReader::ConsoleReader()
{
	m_runner = std::thread([this]() 
		{
			while (!m_join) {
				UserInput input;
				std::cin >> input;
				{
					std::lock_guard guard(m_lock);
					m_queue.push(input);
				}
			}
		}
	);
}

UserInput ConsoleReader::GetInput()
{
	std::lock_guard guard(m_lock);
	UserInput top(std::move(m_queue.front()));
	m_queue.pop();
	return top;
}

bool ConsoleReader::HasInput()
{
	std::lock_guard guard(m_lock);
	return !m_queue.empty();
}

ConsoleReader::~ConsoleReader()
{
	m_join = true;
	m_runner.join();
}
