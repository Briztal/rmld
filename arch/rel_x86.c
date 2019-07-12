
#include <types.h>

#include <loader.h>

static u8 rel16(void *dst, u64 val, u8 relative)
{

	u64 abs;
	u16 final_value;

	/*If the value is relative, val is a signed number;*/
	if (relative) {

		/*Cast the provided value;*/
		s64 sval = (s64) val;

		/*Determine the absolute value;*/
		abs = (u64) ((sval < 0) ? -sval : sval);

		/*Determine the final value;*/
		final_value = (u16) (s16) sval;

	} else {

		/*If the value is absolute, no need for a signed check;*/
		abs = val;
		final_value = (u16) val;

	}

	/*Check the value for an overflow;*/
	if (abs > (u64) ((u16) -1)) {
		return 1;
	}

	/*Apply the relocation;*/
	*((u16 *) dst) = final_value;

	return 0;

}

static u8 rel32(void *dst, u64 val, u8 relative)
{

	u64 abs;
	u32 final_value;

	/*If the value is relative, val is a signed number;*/
	if (relative) {

		/*Cast the provided value;*/
		s64 sval = (s64) val;

		/*Determine the absolute value;*/
		abs = (u64) ((sval < 0) ? -sval : sval);

		/*Determine the final value;*/
		final_value = (u32) (s32) sval;

	} else {

		/*If the value is absolute, no need for a signed check;*/
		abs = val;
		final_value = (u32) val;

	}

	/*Check the value for an overflow;*/
	if (abs > (u64) ((u32) -1)) {
		return 1;
	}

	/*Apply the relocation;*/
	*((u32 *) dst) = final_value;

	return 0;

}

static u8 rel64(void *dst, u64 val, u8 relative)
{

	u64 abs;
	u64 final_value;

	/*If the value is relative, val is a signed number;*/
	if (relative) {

		/*Cast the provided value;*/
		s64 sval = (s64) val;

		/*Determine the absolute value;*/
		abs = (u64) ((sval < 0) ? -sval : sval);

		/*Determine the final value;*/
		final_value = (u64) (s64) sval;

	} else {

		/*If the value is absolute, no need for a signed check;*/
		abs = val;
		final_value = (u64) val;

	}

	/*Check the value for an overflow;*/
	if (abs > (u64) ((u64) -1)) {
		return 1;
	}

	/*Apply the relocation;*/
	*((u64 *) dst) = final_value;

	return 0;

}

/**
 * loader_apply_relocation : apply the relocation @rel_type to @rel_addr,
 * regarding symbol at @sym_addr and @addend; If the relocation fails to be
 * applied, the function stops throws the related error;
 * This function is processor-defined;
 * @param rel_addr : the address to apply the relocation to;
 * @param sym_addr : the address of the symbol the relocation concerns;
 * @param addend : the relocation addend, null if none;
 * @param rel_type : the relocation type;
 * @return 0 if the relocation was applied correctly, LOADER_ERROR_REL_BAD_TYPE
 * if bad relocation type, LOADER_ERROR_REL_VALUE_OVERFLOW if relocation value
 * overflow.
 */
u8 loader_apply_relocation(
		u64 rel_addr,
		u64 sym_addr,
		s64 addend,
		u32 rel_type
)
{

	u8 error;
	u8 relative;
	u64 rel_value;

	/*The function that will apply the relocation;*/
	u8 (*rel_f)(void *, u64, u8);

	/*
	 * Each case will :
	 *  - update the relocation function;
	 *  - update operands to fit the generic value calculation;
	 */
	switch (rel_type) {
		
		case 2: /*R_AMD64_PC32 :*/
			rel_f = &rel32;
			relative = 1;
			break;

		case 4:    /*R_AMD64_PLT32 :*/
			rel_f = &rel32;
			relative = 1;
			break;

		default: /*Unsupported relocation :*/

			/*Set the related flag and quit;*/
			return LOADER_ERROR_REL_BAD_TYPE;

	}

	/*Compute the relocation value;*/
	rel_value = sym_addr + addend - rel_addr;

	/*Apply the relocation;*/
	error = (*rel_f)((void *) rel_addr, rel_value, relative);

	/*If the value was too high for the relocation size :*/
	if (error) {

		/*Set the flag and fail;*/
		return LOADER_ERROR_REL_VALUE_OVERFLOW;
	}

	/*Complete;*/
	return 0;

}
