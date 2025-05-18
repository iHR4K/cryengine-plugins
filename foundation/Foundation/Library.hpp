#pragma once

#include <windows.h>
#include <libloaderapi.h>
#include <cassert>

class Library
{
public:
	Library(const char* name)
	{
		m_Module = ::LoadLibraryA(name);

		// TODO: should be runtime check
		assert(m_Module != nullptr);
	}

	~Library()
	{
		if (m_Module != nullptr)
		{
			::FreeLibrary((HMODULE)(m_Module));
			m_Module = nullptr;
		}
	}

	template<typename TProcedure>
	TProcedure GetProcedureAddress(const char* procedureName)
	{
		return (TProcedure)::GetProcAddress((HMODULE)m_Module, procedureName);
	}

	void Release()
	{
		m_Module = nullptr;
	}

private:
	HMODULE m_Module{ nullptr };
};
