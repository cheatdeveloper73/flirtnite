#pragma once

#include "includes.h"
#include "memory.h"
#include "game.h"

// 88 04 0F 48 ? ? ? 8D 47 01 <- leads to byte

#include "defs.h"

UnrealEngine::FMinimalViewInfo DecryptCamera()
{
	__int64 a1;
	char a3;

	int v3; // eax
	_BYTE* v4; // rbx
	int v5; // esi
	uint64_t v6; // rdi
	int v7; // ecx
	char* v8; // rdi
	__int64 v9; // r8
	char v10; // al
	char v11; // al
	__int64 v12; // r10
	char v13; // r9
	int v14; // eax
	char v15; // cl
	_BYTE* v16; // rdi
	__int64 v17; // r8
	__int64 v18; // rbx
	char v19; // al
	__int64 v20; // r9
	int v21; // edx
	int v22; // eax
	int v23; // ecx
	char* v24; // rdi
	__int64 v25; // r8
	char v26; // al
	__int64 v27; // r8
	char* v28; // rdi

	_QWORD qword_E986998 = Memory.Read<_QWORD>(Memory.BaseAddress + 0xE986998);
	if (!qword_E986998) return {};

	_BYTE byte_E718828 = Memory.Read<_BYTE>(Memory.BaseAddress + 0xE718828);

	_BYTE* a2 = new uint8_t[400];
	Memory._HyperV->ReadMem((PVOID)qword_E986998, a2, 400);

	v4 = a2;

	v5 = 0;
	v6 = (uint64_t)a2;

	uint64_t enc_cam_addr;

	if (byte_E718828)
	{
		switch (byte_E718828)
		{

		case 1:
			v23 = -544581434;
			v24 = (char*)(v6 + 11 - (uint64_t)a2);
			v25 = 52;
			do
			{
				v26 = v23;
				v23 += 11;
				v3 = v4[(_QWORD)v24] ^ v26;
				*v4++ = v3;
				--v25;
			} while (v25);
			break;

		case 2:
			v19 = *a2;
			v20 = *((uint64_t*)a2 + 1);
			v21 = 187545330;
			do
			{
				*v4 = Memory.Read<_BYTE>(26 * ((v5 & 1u) + 1) - (unsigned __int64)(unsigned int)v5 + v20 + 52) - v21;
				v22 = 16 * v5++;
				v3 = ~v22;
				++v4;
				v21 ^= v3;
			} while (v5 < 52);
			break;

		case 3:
			v16 = (uint8_t*)(v6 + 11);
			v17 = 52;
			v18 = (uint64_t)a2 - (uint64_t)v16;
			do
			{
				v3 = *v16 ^ *(v16 - 9);
				(v16++)[v18] = v3;
				--v17;
			} while (v17);
			break;

		case 4:
			v11 = *a2;
			v12 = *((uint64_t*)a2 + 1);
			v13 = -12;
			do
			{
				v14 = 52;
				v15 = v13;
				if (v5 % 3u != 1)
					v14 = 104;
				v13 += 17;
				v3 = v5 + v14;
				++v5;
				*v4++ = Memory.Read<_BYTE>(v3 + v12) ^ v15;
			} while (v5 < 52);
			break;

		case 5:
			v7 = -1468869369;
			v8 = (char*)(v6 + 11 - (uint64_t)a2);
			v9 = 52;
			do
			{
				v10 = v7;
				v7 -= 9;
				v3 = v8[(_QWORD)v4] ^ v10;
				*v4++ = v3;
				--v9;
			} while (v9);
			break;
		}
	}
	else
	{
		v27 = 52;
		v28 = (char*)(v6 + 11 - (uint64_t)a2);
		do
		{
			v3 = __ROL1__(v4[(_QWORD)v28], 4);
			*v4++ = v3;
			--v27;
		} while (v27);
	}

	UnrealEngine::FMinimalViewInfo decrypted_camera;

	if (a2)
	{
		decrypted_camera = *(UnrealEngine::FMinimalViewInfo*)a2;
	}

	delete[] a2;
	return decrypted_camera;
}