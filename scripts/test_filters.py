import json
import re
from pathlib import Path

# Load filter lists
DATA_DIR = Path('samples') / 'filters'

def read_lines(path):
    if path.exists():
        return [line.strip() for line in path.read_text().splitlines() if line.strip()]
    return []

BLOCKED_DOMAINS = set(read_lines(DATA_DIR / 'urlfilters' / 'domains'))
BLOCKED_URLS = set(read_lines(DATA_DIR / 'urlfilters' / 'urls'))
BANNED_WORDS = set(read_lines(DATA_DIR / 'banned_words.txt'))
SOFT_BANNED_WORDS = set(read_lines(DATA_DIR / 'soft_banned_words.txt'))
BANNED_SUBWORDS = set(read_lines(DATA_DIR / 'banned_subwords.txt'))
SOFT_THRESHOLD = 2

# Helper functions
def normalize(text, repl='_'):
    return ''.join(c if c.isalnum() else repl for c in text)

def get_domain(url):
    domain = url
    if '://' in url:
        domain = url.split('://',1)[1]
    return domain.split('/',1)[0]

def split_words(text):
    return {w.lower() for w in re.split(r'[^A-Za-z0-9]+', text) if w}


def filter_url(url):
    reasons = []
    domain = get_domain(url)
    if domain in BLOCKED_DOMAINS:
        reasons.append('blocked_domain')
    if url in BLOCKED_URLS:
        reasons.append('blocked_url')
    words = split_words(url)
    if any(w in BANNED_WORDS for w in words):
        reasons.append('banned_word')
    soft_count = sum(1 for w in words if w in SOFT_BANNED_WORDS)
    if soft_count >= SOFT_THRESHOLD:
        reasons.append('soft_banned_words')
    normalized = normalize(url)
    if any(sub in normalized for sub in BANNED_SUBWORDS):
        reasons.append('banned_subword')
    passed = not reasons
    return passed, reasons


def test_jsonl(path):
    print(f"Testing JSONL file: {path}")
    for line in Path(path).read_text().splitlines():
        obj = json.loads(line)
        url = obj.get('url','')
        passed, reasons = filter_url(url)
        status = 'PASS' if passed else f"BLOCKED ({', '.join(reasons)})"
        print(f"  {url} -> {status}")


def parse_warc(path):
    records = []
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    parts = content.split('WARC/1.0')
    for part in parts:
        if not part.strip():
            continue
        lines = part.strip().splitlines()
        url = None
        for line in lines:
            if line.startswith('WARC-Target-URI:'):
                url = line.split(':',1)[1].strip()
                break
        if url:
            records.append(url)
    return records


def test_warc(path):
    print(f"Testing WARC file: {path}")
    for url in parse_warc(path):
        passed, reasons = filter_url(url)
        status = 'PASS' if passed else f"BLOCKED ({', '.join(reasons)})"
        print(f"  {url} -> {status}")


if __name__ == '__main__':
    test_jsonl('samples/sample.jsonl')
    print()
    test_warc('samples/sample.warc')
