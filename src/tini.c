#include "tini.h"

#include <string.h>
#include <ctype.h>

TINI_EX_FUNC void tini_init(Tini *ini) {
    ini->count = 0;
}

inline static void trim_whitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

TINI_EX_FUNC bool tini_load(Tini *ini, FILE *file) {
    if (!file) return false;

    char line[TINI_MAX_LINE];
    char current_section[TINI_MAX_LINE] = TINI_ROOT_SECTION;

    while (fgets(line, sizeof(line), file)) {
        trim_whitespace(line);

        if (line[0] == '#' || line[0] == ';' || line[0] == '\0') {
            continue;
        }

        if (line[0] == '[') {
            char *end = strchr(line, ']');
            if (end) {
                *end = '\0';
                strncpy(current_section, line + 1, sizeof(current_section) - 1);
                current_section[sizeof(current_section) - 1] = '\0';
            }
        } else {
            char *equals = strchr(line, '=');
            if (equals) {
                *equals = '\0';
                char *key = line;
                char *value = equals + 1;

                trim_whitespace(key);
                trim_whitespace(value);

                if (ini->count < TINI_MAX_KEYS) {
                    TiniEntry *entry = &ini->entries[ini->count++];
                    strncpy(entry->section, current_section, sizeof(entry->section) - 1);
                    strncpy(entry->key, key, sizeof(entry->key) - 1);
                    strncpy(entry->value, value, sizeof(entry->value) - 1);
                }
            }
        }
    }

    return true;
}

TINI_EX_FUNC const char *tini_get(Tini *ini, const char *section, const char *key) {
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0 &&
            strcmp(ini->entries[i].key, key) == 0) {
            return ini->entries[i].value;
        }
    }
    return NULL;
}

TINI_EX_FUNC bool tini_set(Tini *ini, const char *section, const char *key, const char *value) {
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0 &&
            strcmp(ini->entries[i].key, key) == 0) {
            strncpy(ini->entries[i].value, value, sizeof(ini->entries[i].value) - 1);
            ini->entries[i].value[sizeof(ini->entries[i].value) - 1] = '\0';
            return true;
        }
    }

    if (ini->count < TINI_MAX_KEYS) {
        TiniEntry *entry = &ini->entries[ini->count++];
        strncpy(entry->section, section, sizeof(entry->section) - 1);
        strncpy(entry->key, key, sizeof(entry->key) - 1);
        strncpy(entry->value, value, sizeof(entry->value) - 1);
        entry->section[sizeof(entry->section) - 1] = '\0';
        entry->key[sizeof(entry->key) - 1] = '\0';
        entry->value[sizeof(entry->value) - 1] = '\0';
        return true;
    }

    return false;
}

bool tini_dump(Tini *ini, FILE *file) {
    if (!file) return false;

    char current_section[TINI_MAX_LINE] = "";

    for (int i = 0; i < ini->count; i++) {
        TiniEntry *entry = &ini->entries[i];
        if (strcmp(current_section, entry->section) != 0) {
            if (current_section[0] != '\0') fprintf(file, "\n");
            if (strcmp(entry->section, TINI_ROOT_SECTION) != 0) {
                fprintf(file, "[%s]\n", entry->section);
            }
            strncpy(current_section, entry->section, sizeof(current_section) - 1);
            current_section[sizeof(current_section) - 1] = '\0';
        }
        fprintf(file, "%s = %s\n", entry->key, entry->value);
    }

    return true;
}

TINI_EX_FUNC bool tini_has(Tini *ini, const char *section, const char *key) {
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0 &&
            strcmp(ini->entries[i].key, key) == 0) {
            return true;
        }
    }
    return false;
}

TINI_EX_FUNC bool tini_remove(Tini *ini, const char *section, const char *key) {
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0 &&
            strcmp(ini->entries[i].key, key) == 0) {
            for (int j = i; j < ini->count - 1; j++) {
                ini->entries[j] = ini->entries[j + 1];
            }
            ini->count--;
            return true;
        }
    }
    return false;
}