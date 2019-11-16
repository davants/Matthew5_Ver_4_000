#include "Matthew5WindowsUserInput.h"

bool Matthew5::Windows::Matthew5WindowsUserHoldingKeys::CharHit(char c)
{
	if (c < 256)
	{
		return Key[c];
	}
	else
	{
		return false;
	}

}

int Matthew5::Windows::Matthew5WindowsUserHoldingKeys::GetMouseMoveX(void)
{
	// Keep this data around until user asks for it, then set it to zero.
	int returnValue = MouseMoveX - MouseCurrentX;
	MouseMoveX = MouseCurrentX;
	return returnValue;
}

int Matthew5::Windows::Matthew5WindowsUserHoldingKeys::GetMouseMoveY(void)
{
	// Keep this data around until user asks for it, then set it to zero.
	int returnValue = MouseMoveY - MouseCurrentY;
	MouseMoveY = MouseCurrentY;
	return returnValue;

}