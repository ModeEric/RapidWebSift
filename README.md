# ⚡ RapidWebSift

STILL UNDER DEVELOPEMENT! The first version isn't complete yet, I am just building in public.

**Sift through massive web datasets—rapidly.**  
RapidWebSift is a high-performance C++ pipeline for extracting clean, high-quality web text from raw WARC files (like those from [Common Crawl](https://commoncrawl.org/)), tailored for NLP-scale datasets.

---

## 🎯 Project Goals

- ⚡ **Speed-first design** – Stream and filter web data with minimal overhead.
- 🧠 **Reproduce FineWeb-level quality** – Extract usable text comparable to top-tier datasets like [FineWeb](https://huggingface.co/datasets/HuggingFaceFW/fineweb), but at a fraction of the cost.
- 🧹 **Smarter filtering** – Use scalable URL and content filters (Bloom filters, MPHF, heuristics, and ML).
- 🧱 **Modular system** – Cleanly separated components for WARC reading, filtering, HTML parsing, content detection, and output.
- 🔁 **Composable & pluggable** – Easy to swap in ML models or new filter rules as needed.
- 🔍 **NLP-focused** – Output formats geared for language model training: clean plain text, structured text, or semi-structured HTML/JSON.

---

## 📦 Components

| Module              | Description |
|---------------------|-------------|
| `warc_reader`       | Stream and parse WARC records efficiently (4MB chunks) |
| `url_filter`        | Filters out spammy or low-quality URLs using Bloom filters, MPHF, and subword rules |
| `html_extractor`    | Extracts readable content from raw HTML (using DOM heuristics and tag rules) |
| `content_scorer`    | Ranks content blocks using density and structure scoring |
| `text_cleaner`      | Flattens and normalizes extracted text for output |
| *(coming soon)*     | ML classifiers for page type and boilerplate detection |

---

## 🔧 Example Use Case

```bash
# Build the pipeline
cmake -B build && cmake --build build

# Run on a local WARC file
./build/rapidwebsift /path/to/your/input.warc.gz
