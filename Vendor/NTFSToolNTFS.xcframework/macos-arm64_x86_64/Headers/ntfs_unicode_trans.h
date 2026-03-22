/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_UNICODE_TRANS_H
#define NTFSDRV_NTFS_UNICODE_TRANS_H

#include "ntfs_core_types.h"
#include "ntfs_fs_layout.h"

extern BOOL ntfsdrv_names_are_equal(const ntfschar *s1, size_t s1_len,
		const ntfschar *s2, size_t s2_len, const IGNORE_CASE_BOOL ic,
		const ntfschar *upcase, const u32 upcase_size);

extern int ntfsdrv_names_full_collate(const ntfschar *name1, const u32 name1_len,
		const ntfschar *name2, const u32 name2_len,
		const IGNORE_CASE_BOOL ic,
		const ntfschar *upcase, const u32 upcase_len);

extern int ntfsdrv_ucsncmp(const ntfschar *s1, const ntfschar *s2, size_t n);

extern int ntfsdrv_ucsncasecmp(const ntfschar *s1, const ntfschar *s2, size_t n,
		const ntfschar *upcase, const u32 upcase_size);

extern u32 ntfsdrv_ucsnlen(const ntfschar *s, u32 maxlen);

extern ntfschar *ntfsdrv_ucsndup(const ntfschar *s, u32 maxlen);

extern void ntfsdrv_name_upcase(ntfschar *name, u32 name_len,
		const ntfschar *upcase, const u32 upcase_len);

extern void ntfsdrv_name_locase(ntfschar *name, u32 name_len,
		const ntfschar *locase, const u32 locase_len);

extern int ntfsdrv_ucstombs(const ntfschar *ins, const int ins_len, char **outs,
		int outs_len);
extern int ntfsdrv_mbstoucs(const char *ins, ntfschar **outs);

extern char *ntfsdrv_uppercase_mbs(const char *low,
		const ntfschar *upcase, u32 upcase_len);

extern void ntfsdrv_upcase_table_build(ntfschar *uc, u32 uc_len);
extern u32 ntfsdrv_upcase_build_default(ntfschar **upcase);

extern ntfschar *ntfsdrv_str2ucs(const char *s, int *len);

extern void ntfsdrv_ucsfree(ntfschar *ucs);

extern BOOL ntfsdrv_forbidden_chars(const ntfschar *name, int len, BOOL strict);
extern BOOL ntfsdrv_forbidden_names(ntfsdrv_volume *vol,
				const ntfschar *name, int len, BOOL strict);
extern BOOL ntfsdrv_collapsible_chars(ntfsdrv_volume *vol,
				const ntfschar *shortname, int shortlen,
				const ntfschar *longname, int longlen);

extern int ntfsdrv_set_char_encoding(const char *locale);

#if defined(__APPLE__) || defined(__DARWIN__)
/**
 * Mac OS X only.
 *
 * Turn filename Unicode normalization form conversion on or off.
 *   normalize=0 : off
 *   normalize=1 : on
 * If turned on, all filenames returned by ntfstool-driver are converted to NFD
 * normalization form, and all filenames received by ntfstool-driver are converted to NFC
 * normalization form. Because Windows and most other operating systems use the NFC form,
 * while Mac OS X mainly uses NFD, this conversion improves compatibility between Mac applications and
 * the NTFS driver.
 * 
 * @param normalize Decides whether the string functions perform automatic filename normalization when converting to and from UTF-8.
 *        0 disables normalization, 1 enables it.
 * @return -1 if the argument is invalid or an error occurs, 0 on success.
 */
extern int ntfsdrv_macosx_normalize_filenames(int normalize);

/**
 * Mac OS X only.
 * 
 * Normalize the input string "utf8_string" to either NFD or NFC normalization form.
 * The "composed" parameter decides whether the output is in composed form (NFC, composed == 1)
 * or decomposed form (NFD, composed == 0).
 * The input is assumed to be well-formed, NULL-terminated UTF-8. The output is a new UTF-8 string
 * allocated through ntfsdrv_calloc. When it is no longer needed,
 * the caller is responsible for releasing it with free(...).
 *
 * @param utf8_string Input string, in any normalization form.
 * @param target Pointer where the resulting string is stored.
 * @param composed Decides which form to normalize the input string to.
 *        0 means composed form (NFC), 1 means decomposed form (NFD).
 * @return -1 if normalization fails for any reason, otherwise the length of the normalized string stored in target.
 */
extern int ntfsdrv_macosx_normalize_utf8(const char *utf8_string, char **target, int composed);
#endif /* defined(__APPLE__) || defined(__DARWIN__) */

#endif /* defined NTFSDRV_NTFS_UNICODE_TRANS_H */

