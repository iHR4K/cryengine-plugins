#include <ISystem/ISystem.hpp>
#include "Editor.hpp"

#include <cassert>


int main()
{
	Editor editor{ };
	bool isSuccessful = editor.Initialize();

	return 0;
}