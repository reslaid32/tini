
import os
import unittest
import tempfile
from src.tini import Tini

# Unit test for the Tini class
class TestTini(unittest.TestCase):
    def setUp(self):
        # Create a temporary INI file for testing
        self.ini_file = tempfile.NamedTemporaryFile(delete=False)
        self.ini: Tini = Tini()

    def tearDown(self):
        os.remove(self.ini_file.name)

    def test_load(self):
        # Test loading an INI file
        self.ini.set("section1", "key1", "value1")
        self.ini.dump(self.ini_file.name)

        self.ini.load(self.ini_file.name)

        # Check if the value was loaded correctly
        self.assertEqual(self.ini.get("section1", "key1"), "value1")

    def test_get(self):
        # Test getting a value from the INI
        self.ini.set("section1", "key2", "value2")
        self.assertEqual(self.ini.get("section1", "key2"), "value2")

    def test_set(self):
        # Test setting a value in the INI
        self.ini.set("section2", "key3", "value3")
        self.assertEqual(self.ini.get("section2", "key3"), "value3")

    def test_has(self):
        # Test if a key exists in the INI
        self.ini.set("section3", "key4", "value4")
        self.assertTrue(self.ini.has("section3", "key4"))
        self.assertFalse(self.ini.has("section3", "key_nonexistent"))

    def test_remove(self):
        # Test removing a key from the INI
        self.ini.set("section4", "key5", "value5")
        self.ini.remove("section4", "key5")
        self.assertIsNone(self.ini.get("section4", "key5"))

    def test_dump(self):
        # Test dumping INI to file
        self.ini.set("section5", "key6", "value6")
        dump_file = tempfile.NamedTemporaryFile(delete=False)
        self.ini.dump(dump_file.name)

        with open(dump_file.name, 'r') as f:
            content = f.read()
            self.assertIn("[section5]", content)
            self.assertIn("key6 = value6", content)

        os.remove(dump_file.name)

if __name__ == "__main__":
    unittest.main()

