void __fastcall re_loadUvarData(via::uvar::UvarData *a1, __int64 startOffset)
{
  uint64_t addrOfs_8h; // rax
  __int64 addrOffset; // rdi
  uint64_t addr_8h; // rax
  uint64_t addrOfs_10h; // rax
  uint64_t addr_10h; // rax
  unsigned int index; // r15d
  via::uvar::Data1 *pData1; // rcx
  uint64_t v10; // rax
  uint64_t v11; // rax
  uint64_t addrOfs_18h; // rax
  uint64_t v13; // rax
  uint64_t v14; // rax
  _QWORD *v15; // rax
  _QWORD *v16; // r14
  __int64 v17; // rax
  __int64 v18; // rcx
  __int64 v19; // rax
  __int64 v20; // rcx
  unsigned int i; // esi
  __int64 v22; // rbx
  __int64 v23; // rax
  __int64 v24; // rax
  __int64 v25; // rax
  unsigned int j; // ecx
  __int64 v27; // rax
  __int64 v28; // rdx
  __int64 v29; // rdx
  uint64_t addrOfs_20h; // rax
  _QWORD *addr_20h; // rcx
  _QWORD *v32; // rax
  __int64 v33; // rcx
  __int64 v34; // rdx
  __int64 v35; // rcx
  __int64 v36; // rdx
  __int64 v37; // rcx
  __int64 v38; // rcx
  __int64 v39; // rcx
  __int64 v40; // rcx
  uint64_t addrOfs_40h; // rax
  _QWORD *addr_40h; // rcx
  _QWORD *v43; // rax
  __int64 v44; // rcx
  __int64 v45; // rdx
  __int64 v46; // rcx
  __int64 v47; // rdx
  __int64 v48; // rcx
  __int64 v49; // rcx
  __int64 v50; // rcx
  __int64 v51; // rcx
  uint64_t v52; // rax
  uint64_t v53; // rcx
  unsigned int k; // ebx
  uint64_t v55; // rdx
  __int64 v56; // rax
  __int64 v57; // rcx
  char Buffer[256]; // [rsp+20h] [rbp-138h] BYREF

  if ( a1 )
  {
    addrOfs_8h = a1->header.addrOfs_8h;
    addrOffset = startOffset;
    if ( !startOffset )
      addrOffset = (__int64)a1;
    if ( addrOfs_8h )
      addr_8h = addrOffset + addrOfs_8h;
    else
      addr_8h = 0i64;
    a1->header.addrOfs_8h = addr_8h;
    addrOfs_10h = a1->header.addrOfs_10h;
    if ( addrOfs_10h )
      addr_10h = addrOffset + addrOfs_10h;
    else
      addr_10h = 0i64;
    a1->header.addrOfs_10h = addr_10h;
    index = 0;
    for ( Buffer[0] = 0; index < a1->header.count_34h; ++index )
    {
      pData1 = (via::uvar::Data1 *)(a1->header.addrOfs_10h + 0x30i64 * index);
      v10 = pData1->addrOfs_10h;
      if ( v10 )
        v11 = addrOffset + v10;
      else
        v11 = 0i64;
      pData1->addrOfs_10h = v11;
      addrOfs_18h = pData1->addrOfs_18h;
      if ( addrOfs_18h )
        v13 = addrOffset + addrOfs_18h;
      else
        v13 = 0i64;
      pData1->addrOfs_18h = v13;
      v14 = pData1->addrOfs_20h;
      if ( v14 )
        v15 = (_QWORD *)(addrOffset + v14);
      else
        v15 = 0i64;
      pData1->addrOfs_20h = (uint64_t)v15;
      v16 = v15;
      if ( v15 )
      {
        v17 = *v15;
        v18 = v17 + addrOffset;
        if ( !v17 )
          v18 = 0i64;
        *v16 = v18;
        v19 = v16[1];
        v20 = v19 + addrOffset;
        if ( !v19 )
          v20 = 0i64;
        v16[1] = v20;
        for ( i = 0; i < *((unsigned __int16 *)v16 + 8); ++i )
        {
          v22 = *v16 + 32i64 * i;
          if ( *(_QWORD *)v22 )
            v23 = addrOffset + *(_QWORD *)v22;
          else
            v23 = 0i64;
          *(_QWORD *)v22 = v23;
          v24 = *(_QWORD *)(v22 + 8);
          if ( v24 )
            v25 = addrOffset + v24;
          else
            v25 = 0i64;
          *(_QWORD *)(v22 + 8) = v25;
          if ( !*(_DWORD *)(v22 + 16) && *(_QWORD *)v22 )
          {
            re_sprintf(Buffer, "via.userdata.%s", *(const char **)v22);
            *(_DWORD *)(v22 + 16) = j_re_string_hash_01(Buffer);
          }
          for ( j = 0; j < *(unsigned __int16 *)(v22 + 26); ++j )
          {
            v27 = *(_QWORD *)(v22 + 8) + 16i64 * j;
            if ( *(_BYTE *)(v27 + 4) == 20 )
            {
              v28 = *(_QWORD *)(v27 + 8);
              if ( v28 )
                v29 = addrOffset + v28;
              else
                v29 = 0i64;
              *(_QWORD *)(v27 + 8) = v29;
            }
          }
        }
      }
    }
    addrOfs_20h = a1->header.addrOfs_20h;
    addr_20h = (_QWORD *)(addrOfs_20h + addrOffset);
    if ( !addrOfs_20h )
      addr_20h = 0i64;
    a1->header.addrOfs_20h = (uint64_t)addr_20h;
    v32 = addr_20h;
    if ( addr_20h )
    {
      v33 = *addr_20h;
      v34 = v33 + addrOffset;
      if ( !v33 )
        v34 = 0i64;
      *v32 = v34;
      v35 = v32[1];
      v36 = v35 + addrOffset;
      if ( !v35 )
        v36 = 0i64;
      v32[1] = v36;
      v37 = v32[2];
      if ( v37 )
        v38 = addrOffset + v37;
      else
        v38 = 0i64;
      v32[2] = v38;
      v39 = v32[3];
      if ( v39 )
        v40 = addrOffset + v39;
      else
        v40 = 0i64;
      v32[3] = v40;
    }
    addrOfs_40h = a1->header.addrOfs_28h;
    addr_40h = (_QWORD *)(addrOfs_40h + addrOffset);
    if ( !addrOfs_40h )
      addr_40h = 0i64;
    a1->header.addrOfs_28h = (uint64_t)addr_40h;
    v43 = addr_40h;
    if ( addr_40h )
    {
      v44 = *addr_40h;
      v45 = v44 + addrOffset;
      if ( !v44 )
        v45 = 0i64;
      *v43 = v45;
      v46 = v43[1];
      v47 = v46 + addrOffset;
      if ( !v46 )
        v47 = 0i64;
      v43[1] = v47;
      v48 = v43[2];
      if ( v48 )
        v49 = addrOffset + v48;
      else
        v49 = 0i64;
      v43[2] = v49;
      v50 = v43[3];
      if ( v50 )
        v51 = addrOffset + v50;
      else
        v51 = 0i64;
      v43[3] = v51;
    }
    v52 = a1->header.addrOfs_18h;
    v53 = v52 + addrOffset;
    if ( !v52 )
      v53 = 0i64;
    a1->header.addrOfs_18h = v53;
    for ( k = 0;
          k < a1->header.count_36h;
          re_loadUvarData(*(via::uvar::UvarData **)(a1->header.addrOfs_18h + 8i64 * k++), 0i64) )
    {
      v55 = a1->header.addrOfs_18h;
      v56 = *(_QWORD *)(v55 + 8i64 * k);
      v57 = v56 + addrOffset;
      if ( !v56 )
        v57 = 0i64;
      *(_QWORD *)(v55 + 8i64 * k) = v57;
    }
  }
}