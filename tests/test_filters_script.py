import unittest
import json
from scripts import test_filters

class FilterURLTests(unittest.TestCase):
    def test_sample_urls(self):
        samples = {
            "http://good.example.com/page": True,
            "http://bad.example.com/ads": False,
            "http://blocked.example.com/bad": False,
            "http://cool.test.com/cheap-offer": False,
            "http://site.com/spammy": False,
        }
        for url, expected in samples.items():
            passed, _ = test_filters.filter_url(url)
            self.assertEqual(passed, expected, msg=url)

    def test_jsonl_sample(self):
        # Check that parsing the JSONL returns expected order of pass/fail
        expected = [True, False, False, False, False]
        results = []
        with open('samples/sample.jsonl') as fh:
            for line in fh:
                url = json.loads(line).get('url')
                passed, _ = test_filters.filter_url(url)
                results.append(passed)
        self.assertEqual(results, expected)

    def test_warc_sample(self):
        urls = test_filters.parse_warc('samples/sample.warc')
        expected = [
            "http://good.example.com/page",
            "http://bad.example.com/ads",
            "http://cool.test.com/cheap-offer",
            "http://site.com/spammy",
        ]
        self.assertEqual(urls, expected)
        statuses = [test_filters.filter_url(u)[0] for u in urls]
        self.assertEqual(statuses, [True, False, False, False])

if __name__ == '__main__':
    unittest.main()

