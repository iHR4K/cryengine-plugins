#include <ISystem/ISystem.hpp>
#include "Editor.hpp"

#include <cassert>


int main()
{
	Editor editor{ };
	bool isSuccessfule = editor.Initialize();

	return 0;
}