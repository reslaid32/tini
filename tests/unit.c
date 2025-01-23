#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tini.h"

// Helper function to create a test INI file and return the file pointer
FILE* create_test_ini_file() {
    FILE *file = tmpfile();
    assert(file != NULL && "Failed to create temporary test INI file");

    // Write some sections and keys
    fprintf(file, "[section1]\nkey1=value1\nkey2=value2\n");
    fprintf(file, "[section2]\nkey3=value3\n");
    rewind(file); // Move the file pointer back to the beginning for reading
    return file;
}

// Test tini_init
void test_tini_init() {
    Tini ini;
    tini_init(&ini);

    // Verify that the structure is initialized
    assert(ini.count == 0 && "Tini structure should be initialized with count 0");
    printf("test_tini_init passed\n");
}

// Test tini_load
void test_tini_load() {
    Tini ini;
    tini_init(&ini);

    // Create a test INI file
    FILE *file = create_test_ini_file();

    // Load the INI file
    bool loaded = tini_load(&ini, file);
    fclose(file);

    // Verify the load was successful
    assert(loaded == true && "INI file should load successfully");
    assert(ini.count > 0 && "INI file should contain entries after loading");

    // Verify the data
    const char *value = tini_get(&ini, "section1", "key1");
    assert(value != NULL && "key1 in section1 should be found");
    assert(strcmp(value, "value1") == 0 && "key1 should have value 'value1'");

    value = tini_get(&ini, "section2", "key3");
    assert(value != NULL && "key3 in section2 should be found");
    assert(strcmp(value, "value3") == 0 && "key3 should have value 'value3'");

    printf("test_tini_load passed\n");
}

// Test tini_get
void test_tini_get() {
    Tini ini;
    tini_init(&ini);

    // Create and load the INI file
    FILE *file = create_test_ini_file();
    tini_load(&ini, file);
    fclose(file);

    // Verify key retrieval
    const char *value = tini_get(&ini, "section1", "key2");
    assert(value != NULL && "key2 in section1 should be found");
    assert(strcmp(value, "value2") == 0 && "key2 should have value 'value2'");

    // Verify missing key retrieval
    value = tini_get(&ini, "section1", "nonexistent_key");
    assert(value == NULL && "Nonexistent key should return NULL");

    printf("test_tini_get passed\n");
}

// Test tini_set
void test_tini_set() {
    Tini ini;
    tini_init(&ini);

    // Set some keys and values
    bool result = tini_set(&ini, "section1", "key1", "new_value1");
    assert(result == true && "Setting key1 in section1 should succeed");

    const char *value = tini_get(&ini, "section1", "key1");
    assert(value != NULL && "key1 in section1 should be found after setting");
    assert(strcmp(value, "new_value1") == 0 && "key1 should have the new value");

    // Attempt to set a new section with key1
    result = tini_set(&ini, "nonexistent_section", "key1", "value");
    assert(result == true && "Setting a key to a non-existent section should create a new section");
    assert(strcmp(tini_get(&ini, "nonexistent_section", "key1"), "value") == 0);

    printf("test_tini_set passed\n");
}

// Test tini_dump
void test_tini_dump() {
    Tini ini;
    tini_init(&ini);

    // Set some keys and values
    tini_set(&ini, "section1", "key1", "value1");
    tini_set(&ini, "section1", "key2", "value2");

    // Dump to a temporary file
    FILE *file = tmpfile();
    assert(file != NULL && "Failed to create temporary file for dumping INI data");
    bool dumped = tini_dump(&ini, file);
    rewind(file); // Rewind after writing
    fclose(file);

    // Verify the dump was successful by reading back
    assert(dumped == true && "INI dump should be successful");

    file = tmpfile();
    assert(file != NULL && "Failed to open dumped file for reading");

    // Open the dumped file and verify its content
    char line[TINI_MAX_LINE];
    bool found_section1 = false;
    bool found_key1 = false;
    bool found_key2 = false;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "[section1]")) {
            found_section1 = true;
        }
        if (strstr(line, "key1 = value1")) {
            found_key1 = true;
        }
        if (strstr(line, "key2 = value2")) {
            found_key2 = true;
        }
    }
    fclose(file);

    assert(found_section1 && "Dumped file should contain section1");
    assert(found_key1 && "Dumped file should contain key1=value1");
    assert(found_key2 && "Dumped file should contain key2=value2");

    printf("test_tini_dump passed\n");
}

void rununit() {
    test_tini_init();
    test_tini_load();
    test_tini_get();
    test_tini_set();
    test_tini_dump();
}
