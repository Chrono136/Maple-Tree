#include "stdafx.h"
#include "TMD.h"


TMD::TMD()
{
	signature = new char[256];
	padding = new char[60];
	issuer = new char[64];
	reserved = new char[58];
}


TMD::~TMD()
{
}
