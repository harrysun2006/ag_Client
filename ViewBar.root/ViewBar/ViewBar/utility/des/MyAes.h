// ***************************************************************
//  CMyDes   version:  1.0  date: 03/30/2007
//  -------------------------------------------------------------
//  A wrapper class for BlowFish
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

//#include "Rijndael.h"
#include "Blowfish.h"

namespace utility
{

	class CMyCrypt
	{
	public:
		CMyCrypt(void);
	public:
		~CMyCrypt(void);

		BOOL Encrypt(const CString&key, const CString &in, CString &out);
		BOOL Decrypt(const CString&key, const CString &in, CString &out);

	private:

		//Function to convert unsigned char to string of length 2
		void Char2Hex(unsigned char ch, char* szHex);

		//Function to convert string of length 2 to unsigned char
		void Hex2Char(char const* szHex, unsigned char& rch);

		//Function to convert string of unsigned chars to string of chars
		void CharStr2HexStr(unsigned char const* pucCharStr, char* pszHexStr, int iSize);
		void HexStr2CharStr(char const* pszHexStr, unsigned char* pucCharStr, int iSize);
	};
}
