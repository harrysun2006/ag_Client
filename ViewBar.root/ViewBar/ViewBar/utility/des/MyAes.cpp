#include "StdAfx.h"
#include "MyAes.h"
#include "../../MyUtility.h"

namespace utility
{

	CMyCrypt::CMyCrypt(void)
	{
	}

	CMyCrypt::~CMyCrypt(void)
	{
	}


	// Function to convert unsigned char to string of length 2
	void CMyCrypt::Char2Hex(unsigned char ch, char* szHex)
	{
		unsigned char byte[2];
		byte[0] = ch/16;
		byte[1] = ch%16;
		for(int i=0; i<2; i++)
		{
			if(byte[i] >= 0 && byte[i] <= 9)
				szHex[i] = '0' + byte[i];
			else
				szHex[i] = 'A' + byte[i] - 10;
		}
		szHex[2] = 0;
	}

	// Function to convert string of length 2 to unsigned char
	void CMyCrypt::Hex2Char(char const* szHex, unsigned char& rch)
	{
		rch = 0;
		for(int i=0; i<2; i++)
		{
			if(*(szHex + i) >='0' && *(szHex + i) <= '9')
				rch = (rch << 4) + (*(szHex + i) - '0');
			else if(*(szHex + i) >='A' && *(szHex + i) <= 'F')
				rch = (rch << 4) + (*(szHex + i) - 'A' + 10);
			else
				break;
		}
	}    

	// Function to convert string of unsigned chars to string of chars
	void CMyCrypt::CharStr2HexStr(unsigned char const* pucCharStr, char* pszHexStr, int iSize)
	{
		int i;
		char szHex[3];
		pszHexStr[0] = 0;
		for(i=0; i<iSize; i++)
		{
			Char2Hex(pucCharStr[i], szHex);
			strcat(pszHexStr, szHex);
		}
	}

	// Function to convert string of chars to string of unsigned chars
	void CMyCrypt::HexStr2CharStr(char const* pszHexStr, unsigned char* pucCharStr, int iSize)
	{
		int i;
		unsigned char ch;
		for(i=0; i<iSize; i++)
		{
			Hex2Char(pszHexStr+2*i, ch);
			pucCharStr[i] = ch;
		}
	}

	// Encrypt a CString with a key
	BOOL CMyCrypt::Encrypt(const CString &key, const CString &in, CString &out)
	{

		char szHex[17] = {0};

		const int LEN = 8;
		CStringA strA = CMyUtility::GetCStringA(in);
		int iLenStr		= strA.GetLength();
		int iBei		= iLenStr / LEN;
		int iRemainder	= iLenStr % LEN;

		CString newIn = in;
		bool bHasTail = false;
		if (iRemainder != 0)
		{
			for (int i=0; i<LEN - iRemainder; ++i)
			{
				newIn += "0";
			}

			bHasTail = true;

			++iBei;
		}

		char *pKey = CMyUtility::GetAnsiString(key);
		CBlowFish oBlowFish((unsigned char*)pKey, LEN);
		delete []pKey;

		CString strBuffer;
		for (int i=0; i<iBei; ++i)
		{
			char szDataIn[LEN + 1];
			memset(szDataIn, 0, sizeof(szDataIn));
			strBuffer = newIn.Mid(i*LEN, LEN);
			char *pIn = CMyUtility::GetAnsiString(strBuffer);
			strncpy(szDataIn, pIn, LEN);
			delete [] pIn;
			pIn = NULL;
			char szDataOut[LEN+1] = "\0\0\0\0\0\0\0\0";
			oBlowFish.Encrypt((unsigned char*)szDataIn, (unsigned char*)szDataOut, LEN);
			CharStr2HexStr((unsigned char*)szDataOut, szHex, LEN);

			out += szHex;
		}

		CString tmp;
		if(bHasTail)
			tmp.Format(_T("%s{[]}%d"), out, LEN - iRemainder);
		else
			tmp.Format(_T("%s{[]}%d"), out, 0);
		out = tmp;

		return TRUE;
	}


	// Decrypt a CString with a key
	BOOL CMyCrypt::Decrypt(const CString &key, const CString &in, CString &out)
	{
		char szHex[17] = {0};

		int iPos = in.Find(_T("{[]}"));
		CString newIn = in.Left(iPos);
		CString strTail = in.Right(in.GetLength() - iPos - 4);
		int iTailLen = _ttoi(strTail);

		const int LEN = 8;
		int iLenStr = newIn.GetLength();
		int iBei	= iLenStr / (LEN*2);

		char *pKey = CMyUtility::GetAnsiString(key);
		CBlowFish oBlowFish((unsigned char*)pKey, LEN);
		delete [] pKey;

		CString strBuffer;
		for (int i=0; i<iBei; ++i)
		{
			char szDataIn[LEN + 1];
			memset(szDataIn, 0, sizeof(szDataIn));
			strBuffer = newIn.Mid(i*LEN*2, LEN*2);
			char *pIn = CMyUtility::GetAnsiString(strBuffer);
			strncpy(szHex, pIn, LEN*2);

			delete [] pIn;
			pIn = NULL;
			HexStr2CharStr(szHex, (unsigned char*)szDataIn, LEN);

			char szDataOut[LEN+1] = "\0\0\0\0\0\0\0\0";
			oBlowFish.Decrypt((unsigned char*)szDataIn, (unsigned char*)szDataOut, LEN);

			out += szDataOut;
		}

		if (iTailLen > 0)
		{
			out = out.Left(out.GetLength() - iTailLen);
		}

		return TRUE;
	}
}