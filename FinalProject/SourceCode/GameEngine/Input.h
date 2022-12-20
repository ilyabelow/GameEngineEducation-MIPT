#pragma once

enum EInputCommand : size_t
{
	eIC_GoLeft = 0,
	eIC_GoRight,
	eIC_GoForward,
	eIC_GoBackward,
	eIC_Jump,
	eIC_Shoot,
	eIC_BecomeServer,
	eIC_BecomeClient,

	eIC_Max
};
