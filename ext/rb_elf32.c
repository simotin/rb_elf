#include <stdio.h>
#include "ruby.h"
#include "elf32lib.h"

// For mmap
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// =============================================================================
// static variables
// =============================================================================
static VALUE rb_elfModule;				// Module ELF
static VALUE rb_cElf32;					// Class Elf32
static VALUE rb_cElf32Ehdr;				// Class Elf32Ehdr
static VALUE rb_cElf32Shdr;				// Class Elf32Shdr
static VALUE rb_cElf32Sym;				// Class Elf32Sym
static VALUE rb_cElf32Rel;				// Class Elf32Rel
static VALUE rb_cElf32Rela;				// Class Elf32Rela

// =============================================================================
// Elf32Ehdr
// =============================================================================
static VALUE elf32ehdr_alloc(VALUE self);
static VALUE rb_elf32ehdr_new(void);
static size_t rb_elf32ehdr_size(const void *ptr);
static void rb_elf32ehdr_free(void *ptr);
static VALUE elf32ehdr_struct2obj(const Elf32_Ehdr *pEhdr);
static VALUE elf32ehdr_get_ident(VALUE self);
static VALUE elf32ehdr_set_ident(VALUE self, VALUE ident_ary);
static VALUE elf32ehdr_set_type(VALUE self, VALUE type);
static VALUE elf32ehdr_get_type(VALUE self);
static VALUE elf32ehdr_get_machine(VALUE self);
static VALUE elf32ehdr_set_machine(VALUE self, VALUE machine);
static VALUE elf32ehdr_set_phoff(VALUE self, VALUE phoff);
static VALUE elf32ehdr_get_version(VALUE self);
static VALUE elf32ehdr_set_version(VALUE self, VALUE version);
static VALUE elf32ehdr_get_entry(VALUE self);
static VALUE elf32ehdr_set_entry(VALUE self, VALUE entry);
static VALUE elf32ehdr_get_phoff(VALUE self);
static VALUE elf32ehdr_set_shoff(VALUE self, VALUE shoff);
static VALUE elf32ehdr_get_shoff(VALUE self);
static VALUE elf32ehdr_get_flags(VALUE self);
static VALUE elf32ehdr_set_flags(VALUE self, VALUE flags);
static VALUE elf32ehdr_get_ehsize(VALUE self);
static VALUE elf32ehdr_set_ehsize(VALUE self, VALUE ehsize);
static VALUE elf32ehdr_get_phentsize(VALUE self);
static VALUE elf32ehdr_set_phentsize(VALUE self, VALUE phentsize);
static VALUE elf32ehdr_get_phnum(VALUE self);
static VALUE elf32ehdr_set_phnum(VALUE self, VALUE phnum);
static VALUE elf32ehdr_get_shentsize(VALUE self);
static VALUE elf32ehdr_set_shentsize(VALUE self, VALUE shentsize);
static VALUE elf32ehdr_get_shnum(VALUE self);
static VALUE elf32ehdr_set_shnum(VALUE self, VALUE shnum);
static VALUE elf32ehdr_get_shstrndx(VALUE self);
static VALUE elf32ehdr_set_shstrndx(VALUE self, VALUE shstrndx);

// =============================================================================
// Elf32Shdr
// =============================================================================
static VALUE elf32shdr_alloc(VALUE self);
static VALUE rb_elf32shdr_new(void);
static size_t rb_elf32shdr_size(const void *ptr);
static void rb_elf32shdr_free(void *ptr);
static VALUE elf32shdr_struct2obj(const Elf32_Shdr *pShdr);
static VALUE elf32shdr_get_sh_name(VALUE self);
static VALUE elf32shdr_set_sh_name(VALUE self, VALUE type);
static VALUE elf32shdr_get_sh_type(VALUE self);
static VALUE elf32shdr_set_sh_type(VALUE self, VALUE type);
static VALUE elf32shdr_get_sh_flags(VALUE self);
static VALUE elf32shdr_set_sh_flags(VALUE self, VALUE flags);
static VALUE elf32shdr_get_sh_addr(VALUE self);
static VALUE elf32shdr_set_sh_addr(VALUE self, VALUE addr);
static VALUE elf32shdr_get_sh_offset(VALUE self);
static VALUE elf32shdr_set_sh_offset(VALUE self, VALUE offset);
static VALUE elf32shdr_get_sh_size(VALUE self);
static VALUE elf32shdr_set_sh_size(VALUE self, VALUE size);
static VALUE elf32shdr_get_sh_link(VALUE self);
static VALUE elf32shdr_set_sh_link(VALUE self, VALUE link);
static VALUE elf32shdr_get_sh_info(VALUE self);
static VALUE elf32shdr_set_sh_info(VALUE self, VALUE info);
static VALUE elf32shdr_get_sh_addralign(VALUE self);
static VALUE elf32shdr_set_sh_addralign(VALUE self, VALUE addralign);
static VALUE elf32shdr_get_sh_entsize(VALUE self);
static VALUE elf32shdr_set_sh_entsize(VALUE self, VALUE entsize);

// =============================================================================
// Elf32Sym
// =============================================================================
static VALUE elf32sym_alloc(VALUE self);
static VALUE rb_elf32sym_new(void);
static size_t rb_elf32sym_size(const void *ptr);
static void rb_elf32sym_free(void *ptr);
static VALUE elf32sym_struct2obj(const Elf32_Sym *pSym);
static VALUE elf32sym_show(VALUE self);
static VALUE elf32sym_get_st_name(VALUE self);
static VALUE elf32sym_set_st_name(VALUE self, VALUE name);
static VALUE elf32sym_get_st_value(VALUE self);
static VALUE elf32sym_set_st_value(VALUE self, VALUE value);
static VALUE elf32sym_get_st_size(VALUE self);
static VALUE elf32sym_set_st_size(VALUE self, VALUE size);
static VALUE elf32sym_get_st_info(VALUE self);
static VALUE elf32sym_set_st_info(VALUE self, VALUE info);
static VALUE elf32sym_get_scope(VALUE self);
static VALUE elf32sym_set_scope(VALUE self, VALUE info);
static VALUE elf32sym_get_type(VALUE self);
static VALUE elf32sym_set_type(VALUE self, VALUE info);
static VALUE elf32sym_get_st_other(VALUE self);
static VALUE elf32sym_set_st_other(VALUE self, VALUE other);
static VALUE elf32sym_get_st_shndx(VALUE self);
static VALUE elf32sym_set_st_shndx(VALUE self, VALUE shndx);
static VALUE elf32sym_to_bin(VALUE self);
static VALUE elf32sym_has_ref(VALUE self);

// =============================================================================
// Elf32Rel
// =============================================================================
static VALUE elf32rel_alloc(VALUE self);
static VALUE rb_elf32rel_new(void);
static size_t rb_elf32rel_size(const void *ptr);
static void rb_elf32rel_free(void *ptr);
static VALUE elf32rel_struct2obj(const Elf32_Rel *pRel);
static VALUE elf32rel_get_r_offset(VALUE self);
static VALUE elf32rel_set_r_offset(VALUE self, VALUE offset);
static VALUE elf32rel_get_r_info(VALUE self);
static VALUE elf32rel_set_r_info(VALUE self, VALUE info);

// =============================================================================
// Elf32Rela
// =============================================================================
static VALUE elf32rela_alloc(VALUE self);
static VALUE rb_elf32rela_new(void);
static size_t rb_elf32rela_size(const void *ptr);
static void rb_elf32rela_free(void *ptr);
static VALUE elf32rela_struct2obj(const Elf32_Rela *pRela);
static VALUE elf32rela_get_r_offset(VALUE self);
static VALUE elf32rela_set_r_offset(VALUE self, VALUE offset);
static VALUE elf32rela_get_r_info(VALUE self);
static VALUE elf32rela_set_r_info(VALUE self, VALUE info);
static VALUE elf32rela_get_r_addend(VALUE self);
static VALUE elf32rela_set_r_addend(VALUE self, VALUE addend);
static VALUE elf32rela_get_symbolIdx(VALUE self);
static VALUE elf32rela_set_symbolIdx(VALUE self, VALUE new_idx);
static VALUE elf32rela_get_type(VALUE self);
static VALUE elf32rela_set_type(VALUE self, VALUE type);
static VALUE elf32rela_show(VALUE self);
static VALUE elf32rela_to_bin(VALUE self);

// =============================================================================
// Elf32
// =============================================================================
static VALUE elf32_alloc(VALUE klass);
static void rb_elf32_free(void *ptr);
static size_t rb_elf32_size(const void *ptr);
static VALUE elf32_initialize(VALUE self, VALUE filepath);
static VALUE elf32_show_Ehdr(VALUE self);
static VALUE elf32_get_symtab(VALUE self);
static VALUE elf32_ary2ehdrtab(VALUE self, VALUE ary);
static VALUE elf32_ary2shdrtab(VALUE self, VALUE ary);
static VALUE elf32_shdrtab2ary(VALUE self, VALUE shdrtab);
static VALUE elf32_ary2symtab(VALUE self, VALUE ary);
static VALUE elf32_symtab2ary(VALUE self, VALUE symtab);
static VALUE elf32_ary2reltab(VALUE self, VALUE ary);
static VALUE elf32_ary2relatab(VALUE self, VALUE ary);

// =============================================================================
// raise exception
// =============================================================================
static void raise_exception(const char *fname, int lnum)
{
	// TODO error handle
	rb_exc_raise(rb_str_new2("TODO Exception!!"));
}

// =============================================================================
// debug printf
// =============================================================================
static void dbg_puts(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[DEBUG] ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
	return;
}

// =============================================================================
// Elf32Ehdr type info
// =============================================================================
static const rb_data_type_t rb_elf32ehdr_type = {
    "ELF/Elf32Ehdr",
    {
		0,							// dmark
    	rb_elf32ehdr_free,			// dfree
    	rb_elf32ehdr_size,			// dsize
    	{0},						// reserved
    },
    0,								// parent
	0,								// for user
	RUBY_TYPED_FREE_IMMEDIATELY,	// free when unused.
};

// =============================================================================
// Elf32Ehdr alloc
// =============================================================================
static VALUE elf32ehdr_alloc(VALUE self)
{
	Elf32_Shdr *ptr;
	return TypedData_Make_Struct(self, Elf32_Shdr, &rb_elf32ehdr_type, ptr);
}

// =============================================================================
// Elf32Ehdr new
// =============================================================================
static VALUE rb_elf32ehdr_new(void)
{
	return elf32ehdr_alloc(rb_cElf32Ehdr);
}

// =============================================================================
// Elf32Ehdr free
// =============================================================================
static void rb_elf32ehdr_free(void *ptr)
{
	free(ptr);
	return;
}

// =============================================================================
// Elf32Ehdr size
// =============================================================================
static size_t rb_elf32ehdr_size(const void *ptr)
{
	return sizeof(Elf32_Ehdr);
}

// =============================================================================
// Elf32Ehdr create object from Elf32_Ehdr
// =============================================================================
static VALUE elf32ehdr_struct2obj(const Elf32_Ehdr *pEhdr)
{
	VALUE obj;
	Elf32_Ehdr *ptr;
	obj = rb_elf32ehdr_new();
	TypedData_Get_Struct(obj, Elf32_Ehdr, &rb_elf32ehdr_type, ptr);
	memcpy(ptr, pEhdr, sizeof(Elf32_Ehdr));
	return obj;
}

// =============================================================================
// Elf32Ehdr Get e_ident
// =============================================================================
static VALUE elf32ehdr_get_ident(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	int i;
	VALUE ident_ary;

	ident_ary = rb_ary_new();
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	for (i = 0; i < EI_NIDENT; i++) {
		rb_ary_push(ident_ary, INT2FIX(pEhdr->e_ident[i]));
	}

	return ident_ary;
}

// =============================================================================
// Elf32Ehdr Set e_ident
// =============================================================================
static VALUE elf32ehdr_set_ident(VALUE self, VALUE ident_ary)
{
	Elf32_Ehdr *pEhdr;
	int i, len;
	
	Check_Type(ident_ary, T_ARRAY);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	len = RARRAY_LEN(ident_ary);
	if (len != EI_NIDENT)
	{
		raise_exception(__FUNCTION__, __LINE__);
		return Qnil;
	}

	for(i = 0; i < EI_NIDENT; i++) {
		pEhdr->e_ident[i] = NUM2CHR(rb_ary_entry(ident_ary, i));
	}

	return self;
}

// =============================================================================
// Elf32Ehdr Get e_type
// =============================================================================
static VALUE elf32ehdr_get_type(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_type);
}

// =============================================================================
// Elf32Ehdr Set e_type
// =============================================================================
static VALUE elf32ehdr_set_type(VALUE self, VALUE type)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(type, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_type = NUM2INT(type);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_machine
// =============================================================================
static VALUE elf32ehdr_get_machine(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_machine);
}

// =============================================================================
// Elf32Ehdr Set e_machine
// =============================================================================
static VALUE elf32ehdr_set_machine(VALUE self, VALUE machine)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(machine, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_machine = NUM2INT(machine);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_version
// =============================================================================
static VALUE elf32ehdr_get_version(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_version);
}

// =============================================================================
// Elf32Ehdr Set e_version
// =============================================================================
static VALUE elf32ehdr_set_version(VALUE self, VALUE version)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(version, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_version = NUM2INT(version);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_entry
// =============================================================================
static VALUE elf32ehdr_get_entry(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_entry);
}

// =============================================================================
// Elf32Ehdr Set e_entry
// =============================================================================
static VALUE elf32ehdr_set_entry(VALUE self, VALUE entry)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(entry, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_entry = NUM2INT(entry);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_phoff
// =============================================================================
static VALUE elf32ehdr_get_phoff(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_phoff);
}


// =============================================================================
// Elf32Ehdr Set e_phoff
// =============================================================================
static VALUE elf32ehdr_set_phoff(VALUE self, VALUE phoff)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(phoff, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_phoff = NUM2INT(phoff);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_shoff
// =============================================================================
static VALUE elf32ehdr_get_shoff(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_shoff);
}

// =============================================================================
// Elf32Ehdr Set e_shoff
// =============================================================================
static VALUE elf32ehdr_set_shoff(VALUE self, VALUE shoff)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(shoff, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_shoff = NUM2INT(shoff);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_flags
// =============================================================================
static VALUE elf32ehdr_get_flags(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_flags);
}

// =============================================================================
// Elf32Ehdr Set e_flags
// =============================================================================
static VALUE elf32ehdr_set_flags(VALUE self, VALUE flags)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(flags, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_flags = NUM2INT(flags);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_ehsize
// =============================================================================
static VALUE elf32ehdr_get_ehsize(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_ehsize);
}

// =============================================================================
// Elf32Ehdr Set e_ehsize
// =============================================================================
static VALUE elf32ehdr_set_ehsize(VALUE self, VALUE ehsize)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(ehsize, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_ehsize = NUM2INT(ehsize);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_phentsize
// =============================================================================
static VALUE elf32ehdr_get_phentsize(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_phentsize);
}

// =============================================================================
// Elf32Ehdr Set e_phentsize
// =============================================================================
static VALUE elf32ehdr_set_phentsize(VALUE self, VALUE phentsize)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(phentsize, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_phentsize = NUM2INT(phentsize);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_phnum
// =============================================================================
static VALUE elf32ehdr_get_phnum(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_phnum);
}

// =============================================================================
// Elf32Ehdr Set e_phnum
// =============================================================================
static VALUE elf32ehdr_set_phnum(VALUE self, VALUE phnum)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(phnum, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_phnum = NUM2INT(phnum);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_shentsize
// =============================================================================
static VALUE elf32ehdr_get_shentsize(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_shentsize);
}

// =============================================================================
// Elf32Ehdr Set e_shentsize
// =============================================================================
static VALUE elf32ehdr_set_shentsize(VALUE self, VALUE shentsize)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(shentsize, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_shentsize = NUM2INT(shentsize);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_shnum
// =============================================================================
static VALUE elf32ehdr_get_shnum(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_shnum);
}

// =============================================================================
// Elf32Ehdr Set e_shnum
// =============================================================================
static VALUE elf32ehdr_set_shnum(VALUE self, VALUE shnum)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(shnum, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_shnum = NUM2INT(shnum);
	return self;
}

// =============================================================================
// Elf32Ehdr Get e_shstrndx
// =============================================================================
static VALUE elf32ehdr_get_shstrndx(VALUE self)
{
	Elf32_Ehdr *pEhdr;
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	return INT2NUM(pEhdr->e_shstrndx);
}

// =============================================================================
// Elf32Ehdr Set e_shstrndx
// =============================================================================
static VALUE elf32ehdr_set_shstrndx(VALUE self, VALUE shstrndx)
{
	Elf32_Ehdr *pEhdr;
	Check_Type(shstrndx, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Ehdr, &rb_elf32ehdr_type, pEhdr);
	pEhdr->e_shstrndx = NUM2INT(shstrndx);
	return self;
}

// =============================================================================
// Elf32Shdr type info
// =============================================================================
static const rb_data_type_t rb_elf32shdr_type = {
    "ELF/Elf32Shdr",
    {
		0,							// dmark
    	rb_elf32shdr_free,			// dfree
    	rb_elf32shdr_size,			// dsize
    	{0},						// reserved
    },
    0,								// parent
	0,								// for user
	RUBY_TYPED_FREE_IMMEDIATELY,	// free when unused.
};

// =============================================================================
// Elf32Shdr alloc
// =============================================================================
static VALUE elf32shdr_alloc(VALUE self)
{
	Elf32_Shdr *ptr;
	return TypedData_Make_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, ptr);
}

// =============================================================================
// Elf32Shdr new
// =============================================================================
static VALUE rb_elf32shdr_new(void)
{
	return elf32shdr_alloc(rb_cElf32Shdr);
}

// =============================================================================
// Elf32Shdr free
// =============================================================================
static void rb_elf32shdr_free(void *ptr)
{
	free(ptr);
	return;
}

// =============================================================================
// Elf32Shdr size
// =============================================================================
static size_t rb_elf32shdr_size(const void *ptr)
{
	return sizeof(Elf32_Shdr);
}

// =============================================================================
// Elf32Shdr create object from Elf32_Shdr
// =============================================================================
static VALUE elf32shdr_struct2obj(const Elf32_Shdr *pShdr)
{
	VALUE obj;
	Elf32_Shdr *ptr;
	obj = rb_elf32shdr_new();
	TypedData_Get_Struct(obj, Elf32_Shdr, &rb_elf32shdr_type, ptr);
	memcpy(ptr, pShdr, sizeof(Elf32_Shdr));
	return obj;
}

// =============================================================================
// Elf32Shdr Get sh_name
// =============================================================================
static VALUE elf32shdr_get_sh_name(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_name);
}

// =============================================================================
// Elf32Shdr Set sh_name
// =============================================================================
static VALUE elf32shdr_set_sh_name(VALUE self, VALUE name)
{
	Elf32_Shdr *pShdr;
	Check_Type(name, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_name = NUM2INT(name);
	return self;
}

// =============================================================================
// Elf32Shdr Get sh_type
// =============================================================================
static VALUE elf32shdr_get_sh_type(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_type);
}

// =============================================================================
// Elf32Shdr Set sh_type
// =============================================================================
static VALUE elf32shdr_set_sh_type(VALUE self, VALUE type)
{
	Elf32_Shdr *pShdr;
	Check_Type(type, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_type = NUM2INT(type);
	return self;
}

// =============================================================================
// Elf32Shdr Get sh_flags
// =============================================================================
static VALUE elf32shdr_get_sh_flags(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_flags);
}

// =============================================================================
// Elf32Shdr Set sh_flags
// =============================================================================
static VALUE elf32shdr_set_sh_flags(VALUE self, VALUE flags)
{
	Elf32_Shdr *pShdr;
	Check_Type(flags, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_flags = NUM2INT(flags);
	return self;
}

// =============================================================================
// Elf32Shdr Get sh_addr
// =============================================================================
static VALUE elf32shdr_get_sh_addr(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_addr);
}

// =============================================================================
// Elf32Shdr Set sh_addr
// =============================================================================
static VALUE elf32shdr_set_sh_addr(VALUE self, VALUE addr)
{
	Elf32_Shdr *pShdr;
	Check_Type(addr, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_addr = NUM2INT(addr);
	return self;
}

// =============================================================================
// Elf32Shdr Get sh_offset
// =============================================================================
static VALUE elf32shdr_get_sh_offset(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_offset);
}

// =============================================================================
// Elf32Shdr Set sh_offset
// =============================================================================
static VALUE elf32shdr_set_sh_offset(VALUE self, VALUE offset)
{
	Elf32_Shdr *pShdr;
	Check_Type(offset, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_offset = NUM2INT(offset);
	return self;
}

// =============================================================================
// Elf32Shdr Get sh_size
// =============================================================================
static VALUE elf32shdr_get_sh_size(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_size);
}

// =============================================================================
// Elf32Shdr Set sh_size
// =============================================================================
static VALUE elf32shdr_set_sh_size(VALUE self, VALUE size)
{
	Elf32_Shdr *pShdr;
	Check_Type(size, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_size = NUM2INT(size);
	return self;
}

// =============================================================================
// Elf32Shdr Get sh_link
// =============================================================================
static VALUE elf32shdr_get_sh_link(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_link);
}

// =============================================================================
// Elf32Shdr Set sh_link
// =============================================================================
static VALUE elf32shdr_set_sh_link(VALUE self, VALUE link)
{
	Elf32_Shdr *pShdr;
	Check_Type(link, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_link = NUM2INT(link);
	return self;
}

// =============================================================================
// Elf32Shdr Get sh_info
// =============================================================================
static VALUE elf32shdr_get_sh_info(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_info);
}

// =============================================================================
// Elf32Shdr Set sh_info
// =============================================================================
static VALUE elf32shdr_set_sh_info(VALUE self, VALUE info)
{
	Elf32_Shdr *pShdr;
	Check_Type(info, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_info = NUM2INT(info);
	return self;
}

// =============================================================================
// Elf32Shdr Get sh_addralign
// =============================================================================
static VALUE elf32shdr_get_sh_addralign(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_addralign);
}

// =============================================================================
// Elf32Shdr Set sh_addralign
// =============================================================================
static VALUE elf32shdr_set_sh_addralign(VALUE self, VALUE addralign)
{
	Elf32_Shdr *pShdr;
	Check_Type(addralign, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_addralign = NUM2INT(addralign);
	return self;
}

// =============================================================================
// Elf32Shdr Get sh_entsize
// =============================================================================
static VALUE elf32shdr_get_sh_entsize(VALUE self)
{
	Elf32_Shdr *pShdr;
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	return INT2NUM(pShdr->sh_entsize);
}
// =============================================================================
// Elf32Shdr Set sh_entsize
// =============================================================================
static VALUE elf32shdr_set_sh_entsize(VALUE self, VALUE entsize)
{
	Elf32_Shdr *pShdr;
	Check_Type(entsize, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Shdr, &rb_elf32shdr_type, pShdr);
	pShdr->sh_entsize = NUM2INT(entsize);
	return self;
}

// =============================================================================
// Elf32Sym type info
// =============================================================================
static const rb_data_type_t rb_elf32sym_type = {
    "ELF/Elf32Sym",
    {
		0,							// dmark
    	rb_elf32sym_free,			// dfree
    	rb_elf32sym_size,			// dsize
    	{0},						// reserved
    },
    0,								// parent
	0,								// for user
	RUBY_TYPED_FREE_IMMEDIATELY,	// free when unused.
};

// =============================================================================
// Elf32Sym alloc
// =============================================================================
static VALUE elf32sym_alloc(VALUE self)
{
	Elf32_Sym *ptr;
	return TypedData_Make_Struct(self, Elf32_Sym, &rb_elf32sym_type, ptr);
}

// =============================================================================
// Elf32Sym new
// =============================================================================
static VALUE rb_elf32sym_new(void)
{
	return elf32sym_alloc(rb_cElf32Sym);
}

// =============================================================================
// Elf32Sym free
// =============================================================================
static void rb_elf32sym_free(void *ptr)
{
	free(ptr);
	return;
}

// =============================================================================
// Elf32Sym size
// =============================================================================
static size_t rb_elf32sym_size(const void *ptr)
{
	return sizeof(Elf32_Sym);
}

// =============================================================================
// Elf32Sym create object from Elf32_Sym
// =============================================================================
static VALUE elf32sym_struct2obj(const Elf32_Sym *pSym)
{
	VALUE obj;
	Elf32_Sym *ptr;
	obj = rb_elf32sym_new();
	TypedData_Get_Struct(obj, Elf32_Sym, &rb_elf32sym_type, ptr);
	memcpy(ptr, pSym, sizeof(Elf32_Sym));
	return obj;
}

// =============================================================================
// Elf32Sym show member values
// =============================================================================
static VALUE elf32sym_show(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	fprintf(stdout, "st_name:[%d], ", pSym->st_name);
	fprintf(stdout, "st_value:[%d], ", pSym->st_value);
	fprintf(stdout, "st_size:[%d], ", pSym->st_size);
	fprintf(stdout, "st_info:[%d], ", pSym->st_info);
	fprintf(stdout, "st_other:[%d], ", pSym->st_other);
	fprintf(stdout, "st_shndx:[%d]\n", pSym->st_shndx);
	return Qnil;
}

// =============================================================================
// Elf32Sym Get st_name
// =============================================================================
static VALUE elf32sym_get_st_name(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	return INT2NUM(pSym->st_name);
}

// =============================================================================
// Elf32Sym Set st_name
// =============================================================================
static VALUE elf32sym_set_st_name(VALUE self, VALUE name)
{
	Elf32_Sym *pSym;
	Check_Type(name, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	pSym->st_name = NUM2INT(name);
	return self;
}

// =============================================================================
// Elf32Sym Get st_value
// =============================================================================
static VALUE elf32sym_get_st_value(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	return INT2NUM(pSym->st_value);
}

// =============================================================================
// Elf32Sym Set st_value
// =============================================================================
static VALUE elf32sym_set_st_value(VALUE self, VALUE value)
{
	Elf32_Sym *pSym;
	Check_Type(value, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	pSym->st_value = NUM2INT(value);
	return self;
}

// =============================================================================
// Elf32Sym Get st_size
// =============================================================================
static VALUE elf32sym_get_st_size(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	return INT2NUM(pSym->st_size);
}

// =============================================================================
// Elf32Sym Set st_size
// =============================================================================
static VALUE elf32sym_set_st_size(VALUE self, VALUE size)
{
	Elf32_Sym *pSym;
	Check_Type(size, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	pSym->st_size = NUM2INT(size);
	return self;
}

// =============================================================================
// Elf32Sym Get st_info
// =============================================================================
static VALUE elf32sym_get_st_info(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	return INT2NUM(pSym->st_info);
}

// =============================================================================
// Elf32Sym Set st_info
// =============================================================================
static VALUE elf32sym_set_st_info(VALUE self, VALUE info)
{
	Elf32_Sym *pSym;
	Check_Type(info, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	pSym->st_info = NUM2INT(info);
	return self;
}

// =============================================================================
// Elf32Sym Get scope
// =============================================================================
static VALUE elf32sym_get_scope(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	return INT2NUM(ELF32_ST_BIND(pSym->st_info));
}

// =============================================================================
// Elf32Sym Set scope
// =============================================================================
static VALUE elf32sym_set_scope(VALUE self, VALUE scope)
{
	Elf32_Sym *pSym;
	Check_Type(scope, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	pSym->st_info = ELF32_ST_INFO( NUM2INT(scope), ELF32_ST_TYPE(pSym->st_info) );
	return self;
}

// =============================================================================
// Elf32Sym Get type
// =============================================================================
static VALUE elf32sym_get_type(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	return INT2NUM(ELF32_ST_TYPE(pSym->st_info));
}

// =============================================================================
// Elf32Sym Set type
// =============================================================================
static VALUE elf32sym_set_type(VALUE self, VALUE type)
{
	Elf32_Sym *pSym;
	Check_Type(type, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	pSym->st_info = ELF32_ST_INFO( ELF32_ST_BIND(pSym->st_info), ELF32_ST_TYPE(NUM2INT(type)) );
	return self;
}

// =============================================================================
// Elf32Sym Get st_other
// =============================================================================
static VALUE elf32sym_get_st_other(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	return INT2NUM(pSym->st_other);
}

// =============================================================================
// Elf32Sym Set st_other
// =============================================================================
static VALUE elf32sym_set_st_other(VALUE self, VALUE other)
{
	Elf32_Sym *pSym;
	Check_Type(other, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	pSym->st_other = NUM2INT(other);
	return self;
}

// =============================================================================
// Elf32Sym Get st_shndx
// =============================================================================
static VALUE elf32sym_get_st_shndx(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	return INT2NUM(pSym->st_shndx);
}

// =============================================================================
// Elf32Sym Set st_shndx
// =============================================================================
static VALUE elf32sym_set_st_shndx(VALUE self, VALUE shndx)
{
	Elf32_Sym *pSym;
	Check_Type(shndx, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	pSym->st_shndx = NUM2INT(shndx);
	return self;
}

// =============================================================================
// Elf32Sym Convert to binary(Array)
// =============================================================================
static VALUE elf32sym_to_bin(VALUE self)
{
	size_t i;
	Elf32_Sym *pSym;
	uint8_t *pBin;
	VALUE ary;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);

	ary = rb_ary_new();
	pBin = (uint8_t *)pSym;
	for(i = 0; i < sizeof(Elf32_Sym); i++) {
		rb_ary_push(ary, INT2FIX(pBin[i]));
	}
	return ary;
}

// =============================================================================
// Elf32Sym Check symbol has reference section index.
// =============================================================================
static VALUE elf32sym_has_ref(VALUE self)
{
	Elf32_Sym *pSym;
	TypedData_Get_Struct(self, Elf32_Sym, &rb_elf32sym_type, pSym);
	switch(pSym->st_shndx)
	{
	case SHN_UNDEF:
	case SHN_LORESERVE:
	// case SHN_LOPROC: same as SHN_LORESERVE
	// case SHN_BEFORE: same as SHN_LORESERVE
	case SHN_AFTER:
	case SHN_HIPROC:
	case SHN_LOOS:
	case SHN_HIOS:
	case SHN_ABS:
	case SHN_COMMON:
	case SHN_XINDEX:
	// case SHN_HIRESERVE: same as SHN_XINDEX
		return Qfalse;
	default:
		return Qtrue;
	}

	return Qtrue;
}
// =============================================================================
// Elf32Rel type info
// =============================================================================
static const rb_data_type_t rb_elf32rel_type = {
    "ELF/Elf32Rel",
    {
		0,							// dmark
    	rb_elf32rel_free,			// dfree
    	rb_elf32rel_size,			// dsize
    	{0},						// reserved
    },
    0,								// parent
	0,								// for user
	RUBY_TYPED_FREE_IMMEDIATELY,	// free when unused.
};

// =============================================================================
// Elf32Rel alloc
// =============================================================================
static VALUE elf32rel_alloc(VALUE self)
{
	Elf32_Rel *ptr;
	return TypedData_Make_Struct(self, Elf32_Rel, &rb_elf32rel_type, ptr);
}

// =============================================================================
// Elf32Rel new
// =============================================================================
static VALUE rb_elf32rel_new(void)
{
	return elf32rel_alloc(rb_cElf32Rel);
}

// =============================================================================
// Elf32Rel free
// =============================================================================
static void rb_elf32rel_free(void *ptr)
{
	dbg_puts( "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, "In..." );
	free(ptr);
	dbg_puts( "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, "Out..." );
	return;
}

// =============================================================================
// Elf32Rel size
// =============================================================================
static size_t rb_elf32rel_size(const void *ptr)
{
	return sizeof(Elf32_Rel);
}

// =============================================================================
// Elf32Rel create object from Elf32_Rel
// =============================================================================
static VALUE elf32rel_struct2obj(const Elf32_Rel *pRel)
{
	VALUE obj;
	Elf32_Rel *ptr;
	obj = rb_elf32rel_new();
	TypedData_Get_Struct(obj, Elf32_Rel, &rb_elf32rel_type, ptr);
	memcpy(ptr, pRel, sizeof(Elf32_Rel));
	return obj;
}

// =============================================================================
// Elf32Rel Get r_offset
// =============================================================================
static VALUE elf32rel_get_r_offset(VALUE self)
{
	Elf32_Rel *pRel;
	TypedData_Get_Struct(self, Elf32_Rel, &rb_elf32rel_type, pRel);
	return INT2NUM(pRel->r_offset);
}

// =============================================================================
// Elf32Rel Set r_offset
// =============================================================================
static VALUE elf32rel_set_r_offset(VALUE self, VALUE offset)
{
	Elf32_Rel *pRel;
	Check_Type(offset, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Rel, &rb_elf32rel_type, pRel);
	pRel->r_offset = NUM2INT(offset);
	return self;
}

// =============================================================================
// Elf32Rel Get r_info
// =============================================================================
static VALUE elf32rel_get_r_info(VALUE self)
{
	Elf32_Rel *pRel;
	TypedData_Get_Struct(self, Elf32_Rel, &rb_elf32rel_type, pRel);
	return INT2NUM(pRel->r_info);
}

// =============================================================================
// Elf32Rel Set r_info
// =============================================================================
static VALUE elf32rel_set_r_info(VALUE self, VALUE info)
{
	Elf32_Rel *pRel;
	Check_Type(info, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Rel, &rb_elf32rel_type, pRel);
	pRel->r_info = NUM2INT(info);
	return self;
}


// =============================================================================
// Elf32Rela type info
// =============================================================================
static const rb_data_type_t rb_elf32rela_type = {
    "ELF/Elf32Rela",
    {
		0,							// dmark
    	rb_elf32rela_free,			// dfree
    	rb_elf32rela_size,			// dsize
    	{0},						// reserved
    },
    0,								// parent
	0,								// for user
	RUBY_TYPED_FREE_IMMEDIATELY,	// free when unused.
};

// =============================================================================
// Elf32Rela alloc
// =============================================================================
static VALUE elf32rela_alloc(VALUE self)
{
	Elf32_Rela *ptr;
	return TypedData_Make_Struct(self, Elf32_Rela, &rb_elf32rela_type, ptr);
}

// =============================================================================
// Elf32Rela new
// =============================================================================
static VALUE rb_elf32rela_new(void)
{
	return elf32rela_alloc(rb_cElf32Rela);
}

// =============================================================================
// Elf32Rela free
// =============================================================================
static void rb_elf32rela_free(void *ptr)
{
	free(ptr);
	return;
}

// =============================================================================
// Elf32Rela size
// =============================================================================
static size_t rb_elf32rela_size(const void *ptr)
{
	return sizeof(Elf32_Rela);
}

// =============================================================================
// Elf32Rela create object from Elf32_Rela
// =============================================================================
static VALUE elf32rela_struct2obj(const Elf32_Rela *pRela)
{
	VALUE obj;
	Elf32_Rela *ptr;
	obj = rb_elf32rela_new();
	TypedData_Get_Struct(obj, Elf32_Rela, &rb_elf32rela_type, ptr);
	memcpy(ptr, pRela, sizeof(Elf32_Rela));
	return obj;
}

// =============================================================================
// Elf32Rela Get r_offset
// =============================================================================
static VALUE elf32rela_get_r_offset(VALUE self)
{
	Elf32_Rela *pRela;
	TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
	return INT2NUM(pRela->r_offset);
}

// =============================================================================
// Elf32Rela Set r_offset
// =============================================================================
static VALUE elf32rela_set_r_offset(VALUE self, VALUE offset)
{
	Elf32_Rela *pRela;
	Check_Type(offset, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
	pRela->r_offset = NUM2INT(offset);
	return self;
}

// =============================================================================
// Elf32Rela Get r_info
// =============================================================================
static VALUE elf32rela_get_r_info(VALUE self)
{
	Elf32_Rela *pRela;
	TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
	return INT2NUM(pRela->r_info);
}

// =============================================================================
// Elf32Rela Set r_info
// =============================================================================
static VALUE elf32rela_set_r_info(VALUE self, VALUE info)
{
	Elf32_Rela *pRela;
	Check_Type(info, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
	pRela->r_info = NUM2INT(info);
	return self;
}

// =============================================================================
// Elf32Rela Get symbol index (higher 24bit bits of r_info)
// =============================================================================
static VALUE elf32rela_get_symbolIdx(VALUE self)
{
	Elf32_Rel *pRel;
	Elf32_Rela *pRela;
	VALUE	idx;
	if (strcmp("ELF::Elf32Rel", rb_obj_classname(self)) == 0)
	{
		TypedData_Get_Struct(self, Elf32_Rel, &rb_elf32rel_type, pRel);
		idx = INT2NUM( (pRel->r_info & 0xFFFFFF00)  >> 8 );
	}
	else if (strcmp("ELF::Elf32Rela", rb_obj_classname(self)) == 0)
	{
		TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
		idx = INT2NUM( (pRela->r_info & 0xFFFFFF00)  >> 8 );
	}
	else
	{
		raise_exception(__FUNCTION__, __LINE__);
	}
	return idx;
}

// =============================================================================
// Elf32Rela Set symbol index (higher 24bit bits of r_info)
// =============================================================================
static VALUE elf32rela_set_symbolIdx(VALUE self, VALUE idx)
{
	Elf32_Rel *pRel;
	Elf32_Rela *pRela;
	Check_Type(idx, T_FIXNUM);
	if (strcmp("ELF::Elf32Rel", rb_obj_classname(self)) == 0)
	{
		TypedData_Get_Struct(self, Elf32_Rel, &rb_elf32rel_type, pRel);
		pRel->r_info = ((NUM2INT(idx) << 8) & 0xFFFFFF00) | (pRel->r_info & 0x000000FF);
	}
	else if (strcmp("ELF::Elf32Rela", rb_obj_classname(self)) == 0)
	{
		TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
		pRela->r_info = ((NUM2INT(idx) << 8) & 0xFFFFFF00) | (pRela->r_info & 0x000000FF);
	}
	else
	{
		raise_exception(__FUNCTION__, __LINE__);
	}
	return self;
}

// =============================================================================
// Elf32Rela Get symbol index (lower 8bit bits of r_info)
// =============================================================================
static VALUE elf32rela_get_type(VALUE self)
{
	Elf32_Rel *pRel;
	Elf32_Rela *pRela;
	VALUE	idx;
	if (strcmp("ELF::Elf32Rel", rb_obj_classname(self)) == 0)
	{
		TypedData_Get_Struct(self, Elf32_Rel, &rb_elf32rel_type, pRel);
		idx = INT2NUM((pRel->r_info & 0x000000FF));
	}
	else if (strcmp("ELF::Elf32Rela", rb_obj_classname(self)) == 0)
	{
		TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
		idx = INT2NUM((pRela->r_info & 0x000000FF));
	}
	else
	{
		raise_exception(__FUNCTION__, __LINE__);
	}
	return idx;
}

// =============================================================================
// Elf32Rela Set type (lower 8bit bits of r_info)
// =============================================================================
static VALUE elf32rela_set_type(VALUE self, VALUE type)
{
	Elf32_Rel *pRel;
	Elf32_Rela *pRela;
	Check_Type(type, T_FIXNUM);
	if (strcmp("ELF::Elf32Rel", rb_obj_classname(self)) == 0)
	{
		TypedData_Get_Struct(self, Elf32_Rel, &rb_elf32rel_type, pRel);
		pRel->r_info = (pRel->r_info & 0xFFFFFF00) | (NUM2INT(type) & 0x000000FF);
	}
	else if (strcmp("ELF::Elf32Rela", rb_obj_classname(self)) == 0)
	{
		TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
		pRela->r_info = (pRela->r_info & 0xFFFFFF00) | (NUM2INT(type) & 0x000000FF);
	}
	else
	{
		raise_exception(__FUNCTION__, __LINE__);
	}
	return self;
}

// =============================================================================
// Elf32Rela Get r_addend
// =============================================================================
static VALUE elf32rela_get_r_addend(VALUE self)
{
	Elf32_Rela *pRela;
	TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
	return INT2NUM(pRela->r_addend);
}

// =============================================================================
// Elf32Rela Set r_addend
// =============================================================================
static VALUE elf32rela_set_r_addend(VALUE self, VALUE addend)
{
	Elf32_Rela *pRela;
	Check_Type(addend, T_FIXNUM);
	TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
	pRela->r_addend = NUM2INT(addend);
	return self;
}

// =============================================================================
// Show reala table by readelf -r format
// =============================================================================
static VALUE elf32rela_show(VALUE self)
{
	// TODO ref table.
	#if 0
	Elf32_Rela *pRela;
	TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);
	fprintf(stdout, " Offset     Info    Type            Sym.Value  Sym. Name + Addend\n");
	#endif
	return Qnil;
}

// =============================================================================
// Elf32Rela Convert to binary(Array)
// =============================================================================
static VALUE elf32rela_to_bin(VALUE self)
{
	size_t i;
	Elf32_Rela *pRela;
	uint8_t *pBin;
	VALUE ary;
	TypedData_Get_Struct(self, Elf32_Rela, &rb_elf32rela_type, pRela);

	ary = rb_ary_new();
	pBin = (uint8_t *)pRela;
	for(i = 0; i < sizeof(Elf32_Rela); i++) {
		rb_ary_push(ary, INT2FIX(pBin[i]));
	}
	return ary;
}

// =============================================================================
// ELF32 data type
// =============================================================================
static const rb_data_type_t rb_elf32_type = {
    "ELF/Elf32",
    {
		0,							// dmark
    	rb_elf32_free,				// dfree
    	rb_elf32_size,				// dsize
    	{0},						// reserved
    },
    0,								// parent
	0,								// for user
	RUBY_TYPED_FREE_IMMEDIATELY,	// free when unused.
};

// =============================================================================
// ELF32 alloc
// =============================================================================
static VALUE elf32_alloc(VALUE klass)
{
	ST_ELF32 *ptr;
	return TypedData_Make_Struct(klass, ST_ELF32, &rb_elf32_type, ptr);
}

// =============================================================================
// ELF32 free
// =============================================================================
static void rb_elf32_free(void *ptr)
{
	ST_ELF32 *pElf32;
	dbg_puts( "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, "In..." );

	pElf32 = (ST_ELF32 *)ptr;
    elf32_munmapFile(pElf32, pElf32->length);
	free(pElf32);
	dbg_puts( "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, "Out..." );
	return;
}

// =============================================================================
// ELF32 size
// =============================================================================
static size_t rb_elf32_size(const void *ptr)
{
	ST_ELF32 *pElf32 = (ST_ELF32 *)ptr;
	return sizeof(ST_ELF32) + pElf32->length;
}

// =============================================================================
// ELF32 Constructor
// =============================================================================
static VALUE elf32_initialize(VALUE self, VALUE filepath)
{
	int ret;
	ST_ELF32 *pElf32;
	Check_Type(filepath, T_STRING);

	TypedData_Get_Struct(self, ST_ELF32, &rb_elf32_type, pElf32);
	ret = elf32_mmapFile(StringValuePtr(filepath), PROT_READ, MAP_PRIVATE, 0, (void *)&pElf32->pAddr, &pElf32->length);
	if (ret < 0) {
		raise_exception(__FUNCTION__, __LINE__);
	}
	return self;
}

// =============================================================================
// ELF32 dump ELF Header
// =============================================================================
static VALUE elf32_show_Ehdr(VALUE self)
{
	ST_ELF32 *pElf32;
	TypedData_Get_Struct(self, ST_ELF32, &rb_elf32_type, pElf32);
	elf32_showEhdr( (Elf32_Ehdr *)pElf32->pAddr );
	return Qnil;
}

// =============================================================================
// ELF32 Get symbol table (Elf32_Sym list)
// =============================================================================
static VALUE elf32_get_symtab(VALUE self)
{
	int ret;
	VALUE ary = Qnil;
	ST_ELF32 *pSelf;
	Elf32_Shdr *pShdr;
	uint32_t idx;
	Elf32_Sym *pSymtab;
	size_t size;
	const char *pSymtabName;

	dbg_puts( "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, "In..." );
	#if 0
	if (!NIL_P(name)) {
		pSymtabName = ".symtab";
	} else {
		pSymtabName = StringValuePtr(name);
	}
	#endif
	pSymtabName = ".symtab";

	TypedData_Get_Struct(self, ST_ELF32, &rb_elf32_type, pSelf);
	ary = rb_ary_new();
	ret = elf32_searchShdr(pSelf->pAddr, (char *)pSymtabName, &pShdr, &idx);
	if (ret < 0) {
		// TODO 
		// Not Found
		return Qnil;
	}

	pSymtab = (Elf32_Sym *)(pSelf->pAddr + pShdr->sh_offset);
	size = pShdr->sh_size;
	while(0 < size)
	{
		rb_ary_push(ary, elf32sym_struct2obj(pSymtab));
		pSymtab++;
		size -= sizeof(Elf32_Sym);
	}

	dbg_puts( "%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, "In..." );
	return ary;
}

// =============================================================================
// ELF32 Create Elf32_Ehdr Array from Array(Byte Array)
// =============================================================================
static VALUE elf32_ary2ehdrtab(VALUE self, VALUE ary)
{
	int i,len;
	uint8_t *pBin;
	Elf32_Ehdr *pEhdr;

	Check_Type(ary, T_ARRAY);

	len = RARRAY_LEN(ary);
	pBin = malloc(len);
	for(i = 0; i < len; i++) {
		pBin[i] = NUM2CHR(rb_ary_entry(ary, i));
	}

	pEhdr = (Elf32_Ehdr *)pBin;
	return elf32ehdr_struct2obj(pEhdr);
}

// =============================================================================
// ELF32 Create Elf32_Shdr Array from Array(Byte Array)
// =============================================================================
static VALUE elf32_ary2shdrtab(VALUE self, VALUE ary)
{
	int i,len;
	int count;
	uint8_t *pBin;
	Elf32_Shdr *pShdr;
	VALUE shdrtab;

	Check_Type(ary, T_ARRAY);

	len = RARRAY_LEN(ary);
	pBin = malloc(len);
	for(i = 0; i < len; i++) {
		pBin[i] = NUM2CHR(rb_ary_entry(ary, i));
	}

	pShdr = (Elf32_Shdr *)pBin;
	shdrtab = rb_ary_new();
	count = len / sizeof(Elf32_Shdr);
	for (i = 0; i < count; i++) {
		rb_ary_push(shdrtab, elf32shdr_struct2obj(pShdr));
		pShdr++;
	}
	return shdrtab;
}
// =============================================================================
// ELF32 Create Byte Array from Symbol Table(Elf32_Shdr Array)
// =============================================================================
static VALUE elf32_shdrtab2ary(VALUE self, VALUE shdrtab)
{
	size_t i, j, len;
	uint8_t *pBin;
	Elf32_Shdr *pShdr;
	VALUE ary_bin;

	Check_Type(shdrtab, T_ARRAY);
	len = RARRAY_LEN(shdrtab);
	ary_bin = rb_ary_new();

	for(i = 0; i < len; i++) {
		TypedData_Get_Struct(rb_ary_entry(shdrtab, i), Elf32_Shdr, &rb_elf32shdr_type, pShdr);
		pBin = (uint8_t *)pShdr;
		for (j = 0; j < sizeof(Elf32_Shdr); j++) {
			rb_ary_push(ary_bin, INT2FIX(pBin[j]));
		}
	}
	return ary_bin;
}

// =============================================================================
// ELF32 Create Elf32_Sym Array from Array(Byte Array)
// =============================================================================
static VALUE elf32_ary2symtab(VALUE self, VALUE ary)
{
	int i,len;
	int count;
	uint8_t *pBin;
	Elf32_Sym *pSym;
	VALUE symtab;

	Check_Type(ary, T_ARRAY);

	len = RARRAY_LEN(ary);
	pBin = malloc(len);
	for(i = 0; i < len; i++) {
		pBin[i] = NUM2CHR(rb_ary_entry(ary, i));
	}

	pSym = (Elf32_Sym *)pBin;
	symtab = rb_ary_new();
	count = len / sizeof(Elf32_Sym);
	for (i = 0; i < count; i++) {
		rb_ary_push(symtab, elf32sym_struct2obj(pSym));
		pSym++;
	}
	return symtab;
}
// =============================================================================
// ELF32 Create Byte Array from Symbol Table(Elf32_Sym Array)
// =============================================================================
static VALUE elf32_symtab2ary(VALUE self, VALUE symtab)
{
	size_t i, j, len;
	uint8_t *pBin;
	Elf32_Sym *pSym;
	VALUE ary_bin;

	Check_Type(symtab, T_ARRAY);
	len = RARRAY_LEN(symtab);
	ary_bin = rb_ary_new();

	for(i = 0; i < len; i++) {
		TypedData_Get_Struct(rb_ary_entry(symtab, i), Elf32_Sym, &rb_elf32sym_type, pSym);
		pBin = (uint8_t *)pSym;
		for (j = 0; j < sizeof(Elf32_Sym); j++) {
			rb_ary_push(ary_bin, INT2FIX(pBin[j]));
		}
	}
	return ary_bin;
}

// =============================================================================
// ELF32 Create Elf32_Rel Array from Array(Byte Array)
// =============================================================================
static VALUE elf32_ary2reltab(VALUE self, VALUE ary)
{
	int i,len, count;
	uint8_t *pBin;
	Elf32_Rel *pRel;
	VALUE reltab;

	Check_Type(ary, T_ARRAY);

	len = RARRAY_LEN(ary);
	pBin = malloc(len);
	for(i = 0; i < len; i++) {
		pBin[i] = NUM2CHR(rb_ary_entry(ary, i));
	}

	pRel = (Elf32_Rel *)pBin;
	reltab = rb_ary_new();
	count = len / sizeof(Elf32_Rel);
	for (i = 0; i < count; i++) {
		rb_ary_push(reltab, elf32rel_struct2obj(pRel));
		pRel++;
	}
	return reltab;
}

// =============================================================================
// ELF32 Create Elf32_Rela Array from Array(Byte Array)
// =============================================================================
static VALUE elf32_ary2relatab(VALUE self, VALUE ary)
{
	int i,len, count;
	uint8_t *pBin;
	Elf32_Rela *pRela;
	VALUE relatab;

	Check_Type(ary, T_ARRAY);
	len = RARRAY_LEN(ary);
	pBin = malloc(len);
	for(i = 0; i < len; i++) {
		pBin[i] = NUM2CHR(rb_ary_entry(ary, i));
	}

	pRela = (Elf32_Rela *)pBin;
	relatab = rb_ary_new();
	count = len / sizeof(Elf32_Rela);
	for (i = 0; i < count; i++) {
		rb_ary_push(relatab, elf32rela_struct2obj(pRela));
		pRela++;
	}
	return relatab;
}

// =============================================================================
// Initialize ext Module
// =============================================================================
void Init_elf32( void ) {

	// define ELF module
	rb_elfModule = rb_define_module("ELF");

	// for Elf_Sym st_info
	rb_define_const(rb_elfModule, "STT_NOTYPE",		INT2FIX(STT_NOTYPE));
	rb_define_const(rb_elfModule, "STT_OBJECT",		INT2FIX(STT_OBJECT));
	rb_define_const(rb_elfModule, "STT_FUNC",		INT2FIX(STT_FUNC));
	rb_define_const(rb_elfModule, "STT_SECTION",	INT2FIX(STT_SECTION));
	rb_define_const(rb_elfModule, "STT_FILE",		INT2FIX(STT_FILE));
	rb_define_const(rb_elfModule, "STT_COMMON",		INT2FIX(STT_COMMON));
	rb_define_const(rb_elfModule, "STT_TLS",		INT2FIX(STT_TLS));
	rb_define_const(rb_elfModule, "STT_NUM",		INT2FIX(STT_NUM));
	rb_define_const(rb_elfModule, "STT_LOOS",		INT2FIX(STT_LOOS));
	rb_define_const(rb_elfModule, "STT_GNU_IFUNC",	INT2FIX(STT_GNU_IFUNC));
	rb_define_const(rb_elfModule, "STT_HIOS",		INT2FIX(STT_HIOS));
	rb_define_const(rb_elfModule, "STT_LOPROC",		INT2FIX(STT_LOPROC));
	rb_define_const(rb_elfModule, "STT_HIPROC",		INT2FIX(STT_HIPROC));

	// for Elf_Sym st_shndx
	rb_define_const(rb_elfModule, "SHN_UNDEF",		INT2FIX(SHN_UNDEF));
	rb_define_const(rb_elfModule, "SHN_LORESERVE",  INT2FIX(SHN_LORESERVE));
	rb_define_const(rb_elfModule, "SHN_LOPROC",     INT2FIX(SHN_LOPROC));
	rb_define_const(rb_elfModule, "SHN_BEFORE",     INT2FIX(SHN_BEFORE));
	rb_define_const(rb_elfModule, "SHN_AFTER",      INT2FIX(SHN_AFTER));
	rb_define_const(rb_elfModule, "SHN_HIPROC",     INT2FIX(SHN_HIPROC));
	rb_define_const(rb_elfModule, "SHN_LOOS",       INT2FIX(SHN_LOOS));
	rb_define_const(rb_elfModule, "SHN_HIOS",       INT2FIX(SHN_HIOS));
	rb_define_const(rb_elfModule, "SHN_ABS",        INT2FIX(SHN_ABS));
	rb_define_const(rb_elfModule, "SHN_COMMON",     INT2FIX(SHN_COMMON));
	rb_define_const(rb_elfModule, "SHN_XINDEX",     INT2FIX(SHN_XINDEX));
	rb_define_const(rb_elfModule, "SHN_HIRESERVE",  INT2FIX(SHN_HIRESERVE));

	// Initialize rb_cElf32
	rb_cElf32 = rb_define_class_under(rb_elfModule, "Elf32" , rb_cObject);
    rb_define_alloc_func(rb_cElf32, elf32_alloc);
	rb_define_method(rb_cElf32, "initialize", elf32_initialize, 1);
	rb_define_method(rb_cElf32, "show_Ehdr", elf32_show_Ehdr, 0);
	rb_define_method(rb_cElf32, "symtab", elf32_get_symtab, 0);
	rb_define_singleton_method( rb_cElf32, "to_ehdrtab", elf32_ary2ehdrtab, 1);
	rb_define_singleton_method( rb_cElf32, "to_shdrtab", elf32_ary2shdrtab, 1);
	rb_define_singleton_method( rb_cElf32, "to_symtab", elf32_ary2symtab, 1);
	rb_define_singleton_method( rb_cElf32, "shdrtab_to_bin", elf32_shdrtab2ary, 1);
	rb_define_singleton_method( rb_cElf32, "symtab_to_bin", elf32_symtab2ary, 1);
	rb_define_singleton_method( rb_cElf32, "to_reltab", elf32_ary2reltab, 1);
	rb_define_singleton_method( rb_cElf32, "to_relatab", elf32_ary2relatab, 1);

	// Initialize rb_cElf32Ehdr
	rb_cElf32Ehdr = rb_define_class_under(rb_elfModule, "Elf32Ehdr", rb_cObject);
    rb_define_alloc_func(rb_cElf32Ehdr, elf32ehdr_alloc);
	rb_define_method(rb_cElf32Ehdr, "ident", elf32ehdr_get_ident, 0);
	rb_define_method(rb_cElf32Ehdr, "ident=", elf32ehdr_set_ident, 1);
	rb_define_method(rb_cElf32Ehdr, "type", elf32ehdr_set_type, 0);
	rb_define_method(rb_cElf32Ehdr, "type=", elf32ehdr_get_type, 1);
	rb_define_method(rb_cElf32Ehdr, "machine", elf32ehdr_get_machine, 0);
	rb_define_method(rb_cElf32Ehdr, "machine=", elf32ehdr_set_machine, 1);
	rb_define_method(rb_cElf32Ehdr, "version", elf32ehdr_get_version, 0);
	rb_define_method(rb_cElf32Ehdr, "version=", elf32ehdr_set_version, 1);
	rb_define_method(rb_cElf32Ehdr, "entry", elf32ehdr_get_entry, 0);
	rb_define_method(rb_cElf32Ehdr, "entry=", elf32ehdr_set_entry, 1);
	rb_define_method(rb_cElf32Ehdr, "phoff", elf32ehdr_get_phoff, 0);
	rb_define_method(rb_cElf32Ehdr, "phoff=", elf32ehdr_set_phoff, 1);
	rb_define_method(rb_cElf32Ehdr, "shoff", elf32ehdr_set_shoff, 0);
	rb_define_method(rb_cElf32Ehdr, "shoff=", elf32ehdr_get_shoff, 1);
	rb_define_method(rb_cElf32Ehdr, "flags", elf32ehdr_get_flags, 0);
	rb_define_method(rb_cElf32Ehdr, "flags=", elf32ehdr_set_flags, 1);
	rb_define_method(rb_cElf32Ehdr, "ehsize", elf32ehdr_get_ehsize, 0);
	rb_define_method(rb_cElf32Ehdr, "ehsize=", elf32ehdr_set_ehsize, 1);
	rb_define_method(rb_cElf32Ehdr, "phentsize", elf32ehdr_get_phentsize, 0);
	rb_define_method(rb_cElf32Ehdr, "phentsize=", elf32ehdr_set_phentsize, 1);
	rb_define_method(rb_cElf32Ehdr, "phnum", elf32ehdr_get_phnum, 0);
	rb_define_method(rb_cElf32Ehdr, "phnum=", elf32ehdr_set_phnum, 1);
	rb_define_method(rb_cElf32Ehdr, "shentsize", elf32ehdr_get_shentsize, 0);
	rb_define_method(rb_cElf32Ehdr, "shentsize=", elf32ehdr_set_shentsize, 1);
	rb_define_method(rb_cElf32Ehdr, "shnum", elf32ehdr_get_shnum, 0);
	rb_define_method(rb_cElf32Ehdr, "shnum=", elf32ehdr_set_shnum, 1);
	rb_define_method(rb_cElf32Ehdr, "shstrndx", elf32ehdr_get_shstrndx, 0);
	rb_define_method(rb_cElf32Ehdr, "shstrndx=", elf32ehdr_set_shstrndx, 1);

	// Initialize rb_cElf32Shdr
	rb_cElf32Shdr = rb_define_class_under(rb_elfModule, "Elf32Shdr", rb_cObject);
    rb_define_alloc_func(rb_cElf32Shdr, elf32shdr_alloc);
	rb_define_method(rb_cElf32Shdr, "sh_name", elf32shdr_get_sh_name, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_name", elf32shdr_set_sh_name, 1);
	rb_define_method(rb_cElf32Shdr, "sh_type", elf32shdr_get_sh_type, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_type", elf32shdr_set_sh_type, 1);
	rb_define_method(rb_cElf32Shdr, "sh_flags", elf32shdr_get_sh_flags, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_flags", elf32shdr_set_sh_flags, 1);
	rb_define_method(rb_cElf32Shdr, "sh_addr", elf32shdr_get_sh_addr, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_addr", elf32shdr_set_sh_addr, 1);
	rb_define_method(rb_cElf32Shdr, "sh_offset", elf32shdr_get_sh_offset, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_offset", elf32shdr_set_sh_offset, 1);
	rb_define_method(rb_cElf32Shdr, "sh_size", elf32shdr_get_sh_size, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_size", elf32shdr_set_sh_size, 1);
	rb_define_method(rb_cElf32Shdr, "sh_link", elf32shdr_get_sh_link, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_link", elf32shdr_set_sh_link, 1);
	rb_define_method(rb_cElf32Shdr, "sh_info", elf32shdr_get_sh_info, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_info", elf32shdr_set_sh_info, 1);
	rb_define_method(rb_cElf32Shdr, "sh_addralign", elf32shdr_get_sh_addralign, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_addralign", elf32shdr_set_sh_addralign, 1);
	rb_define_method(rb_cElf32Shdr, "sh_entsize", elf32shdr_get_sh_entsize, 0);
	rb_define_method(rb_cElf32Shdr, "=sh_entsize", elf32shdr_set_sh_entsize, 1);

	// Initialize rb_cElf32Sym
	rb_cElf32Sym = rb_define_class_under(rb_elfModule, "Elf32Sym" , rb_cObject);
    rb_define_alloc_func(rb_cElf32Sym, elf32sym_alloc);
	rb_define_method(rb_cElf32Sym, "show", elf32sym_show, 0);
	rb_define_method(rb_cElf32Sym, "st_name", elf32sym_get_st_name, 0);
	rb_define_method(rb_cElf32Sym, "st_name=", elf32sym_set_st_name, 1);
	rb_define_method(rb_cElf32Sym, "st_value", elf32sym_get_st_value, 0);
	rb_define_method(rb_cElf32Sym, "st_value=", elf32sym_set_st_value, 1);
	rb_define_method(rb_cElf32Sym, "st_size", elf32sym_get_st_size, 0);
	rb_define_method(rb_cElf32Sym, "st_size=", elf32sym_set_st_size, 1);
	rb_define_method(rb_cElf32Sym, "st_info", elf32sym_get_st_info, 0);
	rb_define_method(rb_cElf32Sym, "st_info=", elf32sym_set_st_info, 1);
	rb_define_method(rb_cElf32Sym, "scope", elf32sym_get_scope, 0);
	rb_define_method(rb_cElf32Sym, "scope=", elf32sym_set_scope, 1);
	rb_define_method(rb_cElf32Sym, "type", elf32sym_get_type, 0);
	rb_define_method(rb_cElf32Sym, "type=", elf32sym_set_type, 1);
	rb_define_method(rb_cElf32Sym, "st_other", elf32sym_get_st_other, 0);
	rb_define_method(rb_cElf32Sym, "st_other=", elf32sym_set_st_other, 1);
	rb_define_method(rb_cElf32Sym, "st_shndx", elf32sym_get_st_shndx, 0);
	rb_define_method(rb_cElf32Sym, "st_shndx=", elf32sym_set_st_shndx, 1);
	rb_define_method(rb_cElf32Sym, "to_bin", elf32sym_to_bin, 0);
	rb_define_method(rb_cElf32Sym, "has_ref_section?", elf32sym_has_ref, 0);

	// Initialize rb_cElf32Rel
	rb_cElf32Rel = rb_define_class_under(rb_elfModule, "Elf32Rel" , rb_cObject);
    rb_define_alloc_func(rb_cElf32Rel, elf32rel_alloc);
	rb_define_method(rb_cElf32Rel, "r_offset", elf32rel_get_r_offset, 0);
	rb_define_method(rb_cElf32Rel, "r_offset=", elf32rel_set_r_offset, 1);
	rb_define_method(rb_cElf32Rel, "r_info", elf32rel_get_r_info, 0);
	rb_define_method(rb_cElf32Rel, "r_info=", elf32rel_set_r_info, 1);

	// Initialize rb_cElf32Rela
	rb_cElf32Rela = rb_define_class_under(rb_elfModule, "Elf32Rela" , rb_cObject);
    rb_define_alloc_func(rb_cElf32Rela, elf32rela_alloc);
	rb_define_method(rb_cElf32Rela, "r_offset", elf32rela_get_r_offset, 0);
	rb_define_method(rb_cElf32Rela, "r_offset=", elf32rela_set_r_offset, 1);
	rb_define_method(rb_cElf32Rela, "r_info", elf32rela_get_r_info, 0);
	rb_define_method(rb_cElf32Rela, "r_info=", elf32rela_set_r_info, 1);
	rb_define_method(rb_cElf32Rela, "symbol_idx", elf32rela_get_symbolIdx, 0);
	rb_define_method(rb_cElf32Rela, "symbol_idx=", elf32rela_set_symbolIdx, 1);
	rb_define_method(rb_cElf32Rela, "type", elf32rela_get_type, 0);
	rb_define_method(rb_cElf32Rela, "type=", elf32rela_set_type, 1);
	rb_define_method(rb_cElf32Rela, "r_addend", elf32rela_get_r_addend, 0);
	rb_define_method(rb_cElf32Rela, "r_addend=", elf32rela_set_r_addend, 1);
	rb_define_method(rb_cElf32Rela, "show", elf32rela_show, 0);
	rb_define_method(rb_cElf32Rela, "to_bin", elf32rela_to_bin, 0);
	return;
}
