# 🗺️ Project Roadmap

This document outlines the planned and completed components for the web data extraction and filtering pipeline. The project is organized into logical stages/modules, beginning with WARC processing.

---

## 📦 1. WARC Filtering Module

Efficiently read and stream WARC records, extract relevant metadata and payloads, and apply basic filtering.

### ✅ Completed
- [x] Stream read WARC file in large chunks (4 MB buffer)
- [x] Handle incomplete WARC records across buffer boundaries
- [x] Detect individual WARC records using `"WARC/"` delimiter
- [x] Parse essential WARC headers:
  - [x] `WARC-Record-ID`
  - [x] `WARC-Target-URI`
  - [x] `WARC-Date`
  - [x] `Content-Length`
- [x] Detect payload start (`\r\n\r\n` or `\n\n`)
- [x] Extract payload safely only if full content is available in current chunk
- [x] Handle malformed or partial records gracefully
- [x] Count successfully filtered records

### 🟡 In Progress / Todo
- [ ] Apply `filterUrl(uri)` — *[note: logic exists, but implementation is incomplete or skipped]*
- [ ] Write extracted payload to downstream processor (HTML parser)
- [ ] Support compressed `.warc.gz` files via zlib or libarchive
- [ ] Extract and emit structured metadata per record (as JSON or C++ struct)
- [ ] Add CLI or test harness for modular testing

---

## 🔍 2. URL Filter Module

Efficient multi-stage filtering system using bloom filters, MPHF (minimal perfect hash functions), and subword matching to reject bad URLs early in the pipeline.

### ✅ Completed
- [x] Load filter lists from disk:
  - [x] Blocked domains
  - [x] Blocked full URLs
  - [x] Hard-banned words
  - [x] Soft-banned words (threshold-based)
  - [x] Banned subwords
- [x] Initialize bloom filters from raw lists
- [x] Initialize MPHF from word lists using `cmph`
- [x] Save and load filters from cache (bloom, sets, MPHF)
- [x] Efficient domain and full URL filtering via bloom
- [x] Word-based filtering using MPHF (with fallback to linear search)
- [x] Soft-banned threshold logic (e.g., block if ≥2 soft matches)
- [x] Subword matching with normalized URL tokens
- [x] Central `filterUrl()` logic (returns `true` if URL is safe)

### 🟡 In Progress / Todo
- [ ] Add unit tests for `filterUrl()` edge cases
- [ ] Add diagnostics/logging for rejected URLs
- [ ] Optional: Make soft ban threshold configurable
- [ ] Optional: Fallback to raw list load if MPHF file missing
- [ ] Optimize: Combine bloom + MPHF checks into fewer token loops
- [ ] Parallelize MPHF/bloom setup (if initializing massive lists)

---

➡️ Coming soon: `html_extractor`, `content_scorer`, `text_cleaner`, `ml_modules`, `pipeline_runner`
