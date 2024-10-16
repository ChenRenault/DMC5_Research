void __fastcall re_loadUvarData(__int64 a1, __int64 startOffset)
{
  __int64 addrOfs_8h; // rax
  __int64 addrOffset; // rdi
  __int64 addr_8h; // rax
  __int64 addrOfs_10h; // rax
  __int64 addr_10h; // rax
  unsigned int index; // r15d
  _QWORD *v9; // rcx
  __int64 v10; // rax
  __int64 v11; // rax
  __int64 v12; // rax
  __int64 v13; // rax
  __int64 v14; // rax
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
  __int64 addrOfs_20h; // rax
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
  __int64 addrOfs_40h; // rax
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
  __int64 v52; // rax
  __int64 v53; // rcx
  unsigned int k; // ebx
  __int64 v55; // rdx
  __int64 v56; // rax
  __int64 v57; // rcx
  char Buffer[256]; // [rsp+20h] [rbp-138h] BYREF

  if ( a1 )
  {
    addrOfs_8h = *(_QWORD *)(a1 + 8);
    addrOffset = startOffset;
    if ( !startOffset )
      addrOffset = a1;
    if ( addrOfs_8h )
      addr_8h = addrOffset + addrOfs_8h;
    else
      addr_8h = 0i64;
    *(_QWORD *)(a1 + 8) = addr_8h;
    addrOfs_10h = *(_QWORD *)(a1 + 0x10);
    if ( addrOfs_10h )
      addr_10h = addrOffset + addrOfs_10h;
    else
      addr_10h = 0i64;
    *(_QWORD *)(a1 + 16) = addr_10h;
    index = 0;
    for ( Buffer[0] = 0; index < *(unsigned __int16 *)(a1 + 0x34); ++index )
    {
      v9 = (_QWORD *)(*(_QWORD *)(a1 + 16) + 48i64 * index);
      v10 = v9[2];
      if ( v10 )
        v11 = addrOffset + v10;
      else
        v11 = 0i64;
      v9[2] = v11;
      v12 = v9[3];
      if ( v12 )
        v13 = addrOffset + v12;
      else
        v13 = 0i64;
      v9[3] = v13;
      v14 = v9[4];
      if ( v14 )
        v15 = (_QWORD *)(addrOffset + v14);
      else
        v15 = 0i64;
      v9[4] = v15;
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
    addrOfs_20h = *(_QWORD *)(a1 + 0x20);
    addr_20h = (_QWORD *)(addrOfs_20h + addrOffset);
    if ( !addrOfs_20h )
      addr_20h = 0i64;
    *(_QWORD *)(a1 + 0x20) = addr_20h;
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
    addrOfs_40h = *(_QWORD *)(a1 + 40);
    addr_40h = (_QWORD *)(addrOfs_40h + addrOffset);
    if ( !addrOfs_40h )
      addr_40h = 0i64;
    *(_QWORD *)(a1 + 40) = addr_40h;
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
    v52 = *(_QWORD *)(a1 + 24);
    v53 = v52 + addrOffset;
    if ( !v52 )
      v53 = 0i64;
    *(_QWORD *)(a1 + 24) = v53;
    for ( k = 0; k < *(unsigned __int16 *)(a1 + 54); re_loadUvarData(
                                                       *(_QWORD *)(*(_QWORD *)(a1 + 24) + 8i64 * k++),
                                                       0i64) )
    {
      v55 = *(_QWORD *)(a1 + 24);
      v56 = *(_QWORD *)(v55 + 8i64 * k);
      v57 = v56 + addrOffset;
      if ( !v56 )
        v57 = 0i64;
      *(_QWORD *)(v55 + 8i64 * k) = v57;
    }
  }
}