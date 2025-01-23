#ifndef TINI_H
#define TINI_H

#include <stdbool.h>
#include <stdio.h>

#if !defined(TINI_EXPORT)
#if !defined(TINI_NODYLIB)
#if defined(_WIN32)
#if defined(_TINI_BUILD)
#define TINI_EXPORT __declspec(dllexport)
#else
#define TINI_EXPORT __declspec(dllimport)
#endif // _TINI_BUILD
#else
#if defined(_TINI_BUILD)
#define TINI_EXPORT __attribute__((visibility("default")))
#else
#define TINI_EXPORT
#endif // _TINI_BUILD
#endif // _WIN32
#else
#define TINI_EXPORT
#endif // TINI_NODYLIB
#endif // TINI_EXPORT

#if !defined(TINI_CALLTYPE)
#if defined(_MSC_VER)
#define TINI_CALLTYPE __stdcall
#else
#define TINI_CALLTYPE 
#endif // _MSC_VER
#endif // TINI_CALLTYPE

#if !defined(TINI_INLINE)
#if defined(TINI_NODYLIB)
#define TINI_INLINE inline
#else
#define TINI_INLINE
#endif // TINI_NODYLIB
#endif // TINI_INLINE

#if !defined(TINI_EXTERN_DEFINED)
#define TINI_EXTERN_DEFINED
#if !defined(TINI_EXTERN_C_BEG)
#if defined(__cplusplus)
#define TINI_EXTERN_C_BEG extern "C" {
#else
#define TINI_EXTERN_C_BEG
#endif // __cplusplus
#endif // TINI_EXTERN_C_BEG
#if !defined(TINI_EXTERN_C_END)
#if defined(__cplusplus)
#define TINI_EXTERN_C_END }
#else
#define TINI_EXTERN_C_END
#endif // __cplusplus
#endif // TINI_EXTERN_C_END
#endif // TINI_EXTERN_DEFINED

#if !defined(TINI_EX_FUNC)
#define TINI_EX_FUNC TINI_INLINE TINI_CALLTYPE
#endif // TINI_EX_FUNC

#define TINI_MAX_LINE 256
#define TINI_MAX_SECTIONS 64
#define TINI_MAX_KEYS 128
#define TINI_ROOT_SECTION "@$ROOT$@"

/* Extern C for C++: Begin */
TINI_EXTERN_C_BEG

typedef struct TiniEntry TINI_EXPORT {
    char section[TINI_MAX_LINE];
    char key[TINI_MAX_LINE];
    char value[TINI_MAX_LINE];
} TiniEntry;

typedef struct Tini TINI_EXPORT {
    TiniEntry entries[TINI_MAX_KEYS];
    int count;
} Tini;

/**
 * @brief Initializes the Tini structure.
 * 
 * @param ini Pointer to a Tini structure.
 */
TINI_EX_FUNC void tini_init(Tini *ini);

/**
 * @brief Loads an INI file from a FILE* into the Tini structure.
 * 
 * @param ini Pointer to a Tini structure.
 * @param file A FILE* opened for reading.
 * @return true if the file was loaded successfully, false otherwise.
 */
TINI_EX_FUNC bool tini_load(Tini *ini, FILE *file);

/**
 * @brief Retrieves a value for a given section and key.
 * 
 * @param ini Pointer to a Tini structure.
 * @param section Name of the section.
 * @param key Name of the key.
 * @return Pointer to the value if found, NULL otherwise.
 */
TINI_EX_FUNC const char *tini_get(Tini *ini, const char *section, const char *key);

/**
 * @brief Sets or updates a key-value pair in a specific section.
 * 
 * @param ini Pointer to a Tini structure.
 * @param section Name of the section.
 * @param key Name of the key.
 * @param value Value to be set.
 * @return true if the value was set successfully, false otherwise.
 */
TINI_EX_FUNC bool tini_set(Tini *ini, const char *section, const char *key, const char *value);

/**
 * @brief Dumps the INI structure to a FILE*.
 * 
 * @param ini Pointer to a Tini structure.
 * @param file A FILE* opened for writing.
 * @return true if the dump was successful, false otherwise.
 */
TINI_EX_FUNC bool tini_dump(Tini *ini, FILE *file);

/* Extern C for C++: End */
TINI_EXTERN_C_END

#endif // TINI_H
