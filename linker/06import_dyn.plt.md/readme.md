# 导入表

1. 导入函数

2. 导入结构体

3. 导入全局变量

4. 导入数组

## PLT重定位(导入表)


ndk/21.4.7075529/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/bits/elf_arm.h

定义了全部需要重定位的类型


```c
LOGD("================import================");
for (int i = 0; i < soinfo.plt_rela_count_; i++) {
    auto reloc                  = soinfo.plt_rela_[i];
    const uint32_t r_type       = ELF32_R_TYPE(reloc.r_info);
    const uint32_t r_sym_offset = ELF32_R_SYM(reloc.r_info);  //符号表偏移

    Elf32_Addr* const rel_target = reinterpret_cast<Elf32_Addr*>(soinfo.load_bias_ + reloc.r_offset);

    const Elf32_Sym* sym = nullptr;
    const char* sym_name = nullptr;
    Elf32_Addr sym_addr  = 0;
    // get symbol name
    if (r_sym_offset != 0) {
        sym      = soinfo.symtab_ + r_sym_offset;
        sym_name = soinfo.strtab_ + sym->st_name;
    }
    //LOGD("[%d] sym=%s type=%d offset=%p", i, sym_name, r_type, reloc.r_offset);

    // find symbol by name
    if (r_sym_offset == 0) {
        // Do nothing
    } else {
        for (size_t n = 0; n < soinfo.needed_so_len; n++) {
            sym_addr = (Elf32_Addr)dlsym(soinfo.needed_so[n], sym_name);
            if (sym_addr) {
                //LOGD("so=%s sym=%s, addr=%p", soinfo.needed_so_name[n], sym_name, sym_addr);
                break;
            }
        }
    }

    // relocate entry by type
    switch (r_type) {
        case R_ARM_JUMP_SLOT: /* Create PLT entry.  */
        {
            // LOGD("R_ARM_JUMP_SLOT %p to %p", rel_target[0], sym_addr);
            *reinterpret_cast<Elf32_Addr*>(rel_target) = sym_addr;
        } break;
        default:
            LOGE("Unsupported reloc entry: r_sym=%u(%s), r_type=%u, r_offset=0x%08lx, addend=0x%08lx.", r_sym_offset, sym_name, r_type, reloc.r_offset, nullptr);
            break;
    }

    if(sym != nullptr)
    {
        const char* sym_name = soinfo.strtab_ + sym->st_name;
        int type = sym->st_info & 0xf; //STT_NOTYPE, STT_OBJECT, STT_FUNC, STT_SECTION STT_FILE
        int bind = sym->st_info > 0x4; //STBLOCAL, STB_GLOBAL, STB_WEAK
        auto ndx = sym->st_shndx;        
        LOGD("\t[IMPORT]address=0x%08lx, size=%08lu, type=%10s bind=%10s ndx=%4x name=%s", rel_target, sym->st_size, s_type[type], s_bind[bind], ndx, sym_name);
    }
}
```

## Dump输出

void DumpRelocs(const MemSoInfo* soinfo, const char* type, Elf64_Rela* rela, size_t rela_count) {
    LOGD("Relocs(%s):", type);
    for (size_t i = 0; i < rela_count; ++i) {
        Elf64_Rela* reloc = rela + i;   //重定位信息
        void* const rel_target = reinterpret_cast<void*>(soinfo->load_bias_ + reloc->r_offset); //重定位地址
        const uint32_t r_type = ELF64_R_TYPE(reloc->r_info);                                    //重定位类型
        const uint32_t r_sym = ELF64_R_SYM(reloc->r_info);                                      //重定位符号偏移
        Elf64_Sym* sym = soinfo->symtab_ + r_sym;                                               //重定位符号
        const char* sym_name = soinfo->strtab_ + sym->st_name;                                  //符号名称
        LOGD("\treloc rel_target=%p, type=%u, sym=%u, sym_name=%s, addend=0x%08lx.", rel_target, r_type, r_sym, sym_name, reloc->r_addend); //打印
    }
}

## linker4.4 官方实现(剔除了一些跨平台实现)

```c
static int soinfo_relocate(soinfo* si, Elf32_Rel* rel, unsigned count,
                           soinfo* needed[])
{
    Elf32_Sym* symtab = si->symtab;
    const char* strtab = si->strtab;
    Elf32_Sym* s;
    Elf32_Rel* start = rel;
    soinfo* lsi;

    for (size_t idx = 0; idx < count; ++idx, ++rel) {
        unsigned type = ELF32_R_TYPE(rel->r_info);
        unsigned sym = ELF32_R_SYM(rel->r_info);
        Elf32_Addr reloc = static_cast<Elf32_Addr>(rel->r_offset + si->load_bias);
        Elf32_Addr sym_addr = 0;
        char* sym_name = NULL;

        DEBUG("Processing '%s' relocation at index %d", si->name, idx);
        if (type == 0) { // R_*_NONE
            continue;
        }
        if (sym != 0) {
            sym_name = (char *)(strtab + symtab[sym].st_name);
            s = soinfo_do_lookup(si, sym_name, &lsi, needed);
            if (s == NULL) {
                /* We only allow an undefined symbol if this is a weak
                   reference..   */
                s = &symtab[sym];
                if (ELF32_ST_BIND(s->st_info) != STB_WEAK) {
                    DL_ERR("cannot locate symbol \"%s\" referenced by \"%s\"...", sym_name, si->name);
                    return -1;
                }

                /* IHI0044C AAELF 4.5.1.1:

                   Libraries are not searched to resolve weak references.
                   It is not an error for a weak reference to remain
                   unsatisfied.

                   During linking, the value of an undefined weak reference is:
                   - Zero if the relocation type is absolute
                   - The address of the place if the relocation is pc-relative
                   - The address of nominal base address if the relocation
                     type is base-relative.
                  */

                switch (type) {
                case R_ARM_JUMP_SLOT:
                case R_ARM_GLOB_DAT:
                case R_ARM_ABS32:
                case R_ARM_RELATIVE:    /* Don't care. */
                    /* sym_addr was initialized to be zero above or relocation
                       code below does not care about value of sym_addr.
                       No need to do anything.  */
                    break;

                case R_ARM_COPY:
                    /* Fall through.  Can't really copy if weak symbol is
                       not found in run-time.  */
                default:
                    DL_ERR("unknown weak reloc type %d @ %p (%d)",
                                 type, rel, (int) (rel - start));
                    return -1;
                }
            } else {
                /* We got a definition.  */
#if 0
                if ((base == 0) && (si->base != 0)) {
                        /* linking from libraries to main image is bad */
                    DL_ERR("cannot locate \"%s\"...",
                           strtab + symtab[sym].st_name);
                    return -1;
                }
#endif
                sym_addr = static_cast<Elf32_Addr>(s->st_value + lsi->load_bias);
            }
            count_relocation(kRelocSymbol);
        } else {
            s = NULL;
        }

/* TODO: This is ugly. Split up the relocations by arch into
 * different files.
 */
        switch(type){
        case R_ARM_JUMP_SLOT:
            count_relocation(kRelocAbsolute);
            MARK(rel->r_offset);
            TRACE_TYPE(RELO, "RELO JMP_SLOT %08x <- %08x %s", reloc, sym_addr, sym_name);
            *reinterpret_cast<Elf32_Addr*>(reloc) = sym_addr;
            break;
        case R_ARM_GLOB_DAT:
            count_relocation(kRelocAbsolute);
            MARK(rel->r_offset);
            TRACE_TYPE(RELO, "RELO GLOB_DAT %08x <- %08x %s", reloc, sym_addr, sym_name);
            *reinterpret_cast<Elf32_Addr*>(reloc) = sym_addr;
            break;
        case R_ARM_ABS32:
            count_relocation(kRelocAbsolute);
            MARK(rel->r_offset);
            TRACE_TYPE(RELO, "RELO ABS %08x <- %08x %s", reloc, sym_addr, sym_name);
            *reinterpret_cast<Elf32_Addr*>(reloc) += sym_addr;
            break;
        case R_ARM_REL32:
            count_relocation(kRelocRelative);
            MARK(rel->r_offset);
            TRACE_TYPE(RELO, "RELO REL32 %08x <- %08x - %08x %s",
                       reloc, sym_addr, rel->r_offset, sym_name);
            *reinterpret_cast<Elf32_Addr*>(reloc) += sym_addr - rel->r_offset;
            break;
        case R_ARM_RELATIVE:
            count_relocation(kRelocRelative);
            MARK(rel->r_offset);
            if (sym) {
                DL_ERR("odd RELATIVE form...");
                return -1;
            }
            TRACE_TYPE(RELO, "RELO RELATIVE %08x <- +%08x", reloc, si->base);
            *reinterpret_cast<Elf32_Addr*>(reloc) += si->base;
            break;
        case R_ARM_COPY:
            if ((si->flags & FLAG_EXE) == 0) {
                /*
                 * http://infocenter.arm.com/help/topic/com.arm.doc.ihi0044d/IHI0044D_aaelf.pdf
                 *
                 * Section 4.7.1.10 "Dynamic relocations"
                 * R_ARM_COPY may only appear in executable objects where e_type is
                 * set to ET_EXEC.
                 *
                 * TODO: FLAG_EXE is set for both ET_DYN and ET_EXEC executables.
                 * We should explicitly disallow ET_DYN executables from having
                 * R_ARM_COPY relocations.
                 */
                DL_ERR("%s R_ARM_COPY relocations only supported for ET_EXEC", si->name);
                return -1;
            }
            count_relocation(kRelocCopy);
            MARK(rel->r_offset);
            TRACE_TYPE(RELO, "RELO %08x <- %d @ %08x %s", reloc, s->st_size, sym_addr, sym_name);
            if (reloc == sym_addr) {
                Elf32_Sym *src = soinfo_do_lookup(NULL, sym_name, &lsi, needed);

                if (src == NULL) {
                    DL_ERR("%s R_ARM_COPY relocation source cannot be resolved", si->name);
                    return -1;
                }
                if (lsi->has_DT_SYMBOLIC) {
                    DL_ERR("%s invalid R_ARM_COPY relocation against DT_SYMBOLIC shared "
                           "library %s (built with -Bsymbolic?)", si->name, lsi->name);
                    return -1;
                }
                if (s->st_size < src->st_size) {
                    DL_ERR("%s R_ARM_COPY relocation size mismatch (%d < %d)",
                           si->name, s->st_size, src->st_size);
                    return -1;
                }
                memcpy((void*)reloc, (void*)(src->st_value + lsi->load_bias), src->st_size);
            } else {
                DL_ERR("%s R_ARM_COPY relocation target cannot be resolved", si->name);
                return -1;
            }
            break;
        default:
            DL_ERR("unknown reloc type %d @ %p (%d)",
                   type, rel, (int) (rel - start));
            return -1;
        }
    }
    return 0;
}
```