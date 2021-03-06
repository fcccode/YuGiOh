#include "stdafx.h"
#include "Card.h"
#include <MyTools/Character.h>
#include <MyTools/Log.h>
#include "GameEnviroment.h"

#define _SELF L"Card.cpp"
CCard::CCard(_In_ UINT_PTR ulCardId) : _dwCardId(ulCardId)
{
	_dwBase = NULL;
}

CCard::CCard() : _dwCardId(NULL)
{
	_dwBase = NULL;
}

CCard::CCard(_In_ UINT_PTR ulCardId, _In_ UINT_PTR dwAddr) : _dwCardId(ulCardId),_dwBase(dwAddr)
{

}

DWORD CCard::GetPower() CONST _NOEXCEPT
{
	auto RAX = static_cast<DWORD>(MyTools::CCharacter::ReadMemValue(CARD_ATTRIBUTE_BASE + ((GetCardId() * 3) << 4)) + 0x4);
	if (RAX == 0x1FF)
		return 0;

	return RAX * 5 * 2;
}

DWORD CCard::GetDefence() CONST _NOEXCEPT
{
	auto RAX = static_cast<DWORD>(MyTools::CCharacter::ReadMemValue(CARD_ATTRIBUTE_BASE + ((GetCardId() * 3) << 4)) + 0x8);
	if (RAX == 0x1FF)
		return 0;

	return RAX * 5 * 2;
}

DWORD CCard::GetStarCount() CONST _NOEXCEPT
{
	auto RCX = GetCardId() * 3;
	RCX <<= 0x4;

	auto EAX = MyTools::CCharacter::ReadMemValue(CARD_ATTRIBUTE_BASE + RCX + 0xC) & 0xFFFFFFFF;
	return static_cast<DWORD>(MyTools::CCharacter::ReadMemValue((CARD_ATTRIBUTE_BASE + RCX + 0x18) & 0xFFFFFFFF));
}

CONST std::wstring& CCard::GetCardName()
{
	if (_wsCardName.empty())
	{
		DWORD64 r8 = MyTools::CCharacter::ReadMemValue(MyTools::CCharacter::ReadMemValue(CARD_NAME_BASE) + 0x18);
		DWORD64 rdx = MyTools::CCharacter::ReadMemValue(MyTools::CCharacter::ReadMemValue(CARD_NAME_BASE) + 0x28);
		if (r8 == NULL || rdx == NULL)
			return _wsCardName;

		if (!IsCard())
			return _wsCardName;

		auto ulCardID = GetCardId() - 0xF3C;
		DWORD64 ulNamePtr = MyTools::CCharacter::ReadMemValue((MyTools::CCharacter::ReadMemValue(CARD_NAME_INDEX_BASE + ulCardID * 2) & 0xFFFF) * 8 + rdx) + r8;
		if (ulNamePtr == NULL)
			return _wsCardName;

		WCHAR wszCardName[512] = { 0 };
		MyTools::CCharacter::wstrcpy_my(wszCardName, reinterpret_cast<WCHAR*>(ulNamePtr & 0xFFFFFFFF), 64);
		_wsCardName = wszCardName;
	}
	return _wsCardName;
}

CONST std::wstring& CCard::GetCardContent()
{
	if (_wsCardContent.empty())
	{
		DWORD64 r8 = MyTools::CCharacter::ReadMemValue(MyTools::CCharacter::ReadMemValue(CARD_NAME_BASE) + 0x38);
		DWORD64 rdx = MyTools::CCharacter::ReadMemValue(MyTools::CCharacter::ReadMemValue(CARD_NAME_BASE) + 0x28);
		if (r8 == NULL || rdx == NULL)
			return _wsCardContent;

		if (!IsCard())
			return _wsCardContent;

		auto ulCardID = GetCardId() - 0xF3C;
		DWORD64 ulDetailPtr = MyTools::CCharacter::ReadMemValue((MyTools::CCharacter::ReadMemValue(CARD_NAME_INDEX_BASE + ulCardID * 2) & 0xFFFF) * 8 + 0x4 + rdx) + r8;
		if (ulDetailPtr == NULL)
			return _wsCardContent;

		WCHAR wszDetailText[512] = { 0 };
		MyTools::CCharacter::wstrcpy_my(wszDetailText, reinterpret_cast<WCHAR*>(ulDetailPtr & 0xFFFFFFFF), _countof(wszDetailText) - 2);
		_wsCardContent = wszDetailText;
	}
	return _wsCardContent;
}

UINT_PTR CCard::GetCardId() CONST _NOEXCEPT
{
	return _dwCardId & 0xFFFF;
}

UINT_PTR CCard::GetFullCardId() CONST _NOEXCEPT
{
	return _dwCardId;
}

bool CCard::IsCard() CONST _NOEXCEPT
{
	return GetCardId() <= MAX_CARDID && GetCardId() >= MIN_CARDID;
}

VOID CCard::Print()
{
	LOG_CF_D(L"Name[%s],ID[%X] Content:%s", GetCardName().c_str(), static_cast<DWORD>(GetFullCardId()), GetCardContent().c_str());
}

UINT_PTR CCard::GetBase() CONST _NOEXCEPT
{
	return _dwBase;
}
