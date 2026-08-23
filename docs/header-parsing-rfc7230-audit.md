# HTTP Header Parsing Audit (RFC 7230)

Date: 2026-07-31  
Project: 42-Webserv  
Scope: Request header parsing behavior in `src/HTTP/Request.cpp`

## Short Answer

The current header parsing is **partially aligned** with RFC 7230 Section 3.2, but it is **not fully compliant**.

What is correct:
- Header name syntax is validated as an RFC token.
- Header names are normalized to lowercase for case-insensitive handling.
- Leading/trailing optional whitespace (OWS) around values is trimmed.
- Parsing is line-based using CRLF and waits for full lines.

What is missing or incorrect:
- Duplicate header handling is not RFC-safe (last value overwrites previous values).
- Obsolete line folding (`obs-fold`) is not handled explicitly (no reject/unfold policy).
- No explicit header field length/section limits (recommended for security).
- `Host` requirement check is implemented with a case bug (`"Host"` searched while keys are stored lowercase).

---

## RFC 7230 Section 3.2 Simplified

RFC grammar:

- `header-field = field-name ":" OWS field-value OWS`
- `field-name = token`
- `OWS = optional spaces/tabs`

In plain words:
1. A header line must have a name, then `:`, then a value.
2. Header names are case-insensitive (`Host`, `host`, `HOST` are same field name).
3. No spaces are allowed between field name and colon.
4. Spaces/tabs around the value are allowed and should be ignored by parser extraction.
5. Duplicate headers are only safe in specific cases (list-style fields, or special exceptions like `Set-Cookie` in responses).
6. Old multiline folding (`CRLF + space/tab`) is obsolete and should be rejected or normalized according to recipient role.
7. Implementations should enforce practical limits to reduce smuggling and resource abuse risks.

---

## What The Current Code Does

### 1) Splits header lines by first colon
- Function: `splitHeaderLine`
- Behavior: finds first `:`, name = left side, value = right side trimmed.

Why this is generally correct:
- RFC field value can contain many visible characters, including additional colons, so splitting on the first colon is the right strategy.

### 2) Validates `field-name` as token
- Function: `isTokenChar` + `isValidHeaderName`
- Behavior: accepts alnum plus token symbols from RFC (`!#$%&'*+-.^_`|~`).

Why this is correct:
- Matches `tchar` set from RFC 7230 for token-based names.

### 3) Trims OWS around field value
- Function: `trim`
- Behavior: strips leading/trailing SP/HTAB from value substring.

Why this is correct:
- RFC says OWS around field value should be excluded when extracting value.

### 4) Stores header names in lowercase
- Code path: `_headers[toLower(name)] = value`

Why this is correct:
- Header names are case-insensitive by RFC.

### 5) Parses headers only after full CRLF line exists
- Function: `parseHeaders`
- Behavior: if no CRLF found yet, returns `INCOMPLETE`.

Why this is good:
- Avoids parsing partial lines and supports incremental socket input.

---

## Rule-By-Rule Compliance Receipt

### 3.2 Header Fields

Requirement:
- `field-name ":" OWS field-value OWS`

Status: **Mostly compliant**

Evidence:
- First colon split and value trimming are present.
- Name token validation present.

Gap:
- `field-value` allowed octets are not deeply validated (this can be acceptable for generic parse stage, but role-dependent checks may still be needed later).

### 3.2.1 Field Extensibility

Requirement:
- Unknown fields should be accepted/forwarded by intermediaries (with connection-specific exceptions).

Status: **Compliant for generic request intake**

Evidence:
- Parser does not whitelist names, so unknown names are accepted if syntactically valid.

### 3.2.2 Field Order and Duplicates

Requirement:
- Different field-name order is not significant.
- Duplicate same-name fields need careful semantics; list fields may be combined preserving order.

Status: **Partially compliant**

Evidence:
- Order between different names is not used by parser.

Gap:
- Duplicate headers overwrite previous value due to `std::map<string,string>` assignment.
- This loses order and values, and is unsafe for fields where combining is required or forbidden.

### 3.2.3 Whitespace

Requirement:
- OWS may appear around field value.
- No whitespace before colon (see 3.2.4 enforcement).

Status: **Compliant for OWS extraction**

Evidence:
- Trimming SP/HTAB around values.
- Whitespace before colon in name is rejected indirectly because `field-name` token validation fails.

### 3.2.4 Field Parsing

Requirement:
- Generic parser first, per-field semantics later.
- Reject request with whitespace between field-name and colon.
- Handle `obs-fold` (reject or replace with SP by role policy).

Status: **Partially compliant**

Evidence:
- Generic extraction approach is used.
- Space before colon is rejected by token validation.

Gaps:
- No explicit `obs-fold` handling policy (reject or unfold) is implemented.
- If a continuation line starts with SP/HTAB, parser tries to treat it as a new header line and fails; failure happens, but without explicit policy/diagnostic.

### 3.2.5 Field Limits

Requirement:
- Implement practical limits and return 4xx for oversized headers.

Status: **Not implemented**

Evidence:
- No explicit max line length, total header bytes, or header count checks in parser.

### 3.2.6 Field Value Components

Requirement:
- Value grammar (token, quoted-string, comments) is field-specific and often parsed later.

Status: **Acceptable at generic stage**

Evidence:
- Parser performs generic extraction and does not parse per-field grammars yet.

---

## Important Bug Found (Not Strictly RFC 3.2, But Affects HTTP/1.1 Validity)

Behavior:
- Headers are stored lowercase (`host`), but end-of-headers check looks for `"Host"`.

Impact:
- HTTP/1.1 request with valid `Host` can be incorrectly rejected.

Why:
- Key casing mismatch in lookup.

---

## Security and Robustness Implications

1. Duplicate overwrite can hide or alter semantics (risk in request routing/auth parsing paths).
2. Missing explicit obs-fold policy can cause ambiguous handling in edge cases.
3. Missing header size limits increases resource exhaustion and smuggling surface.

---

## Suggested Improvements

1. Store duplicates safely:
- Use `std::map<std::string, std::vector<std::string> >` or equivalent.
- For list-based fields, combine only where RFC allows, preserving order.
- Keep exceptions (for responses, `Set-Cookie`) separate.

2. Add explicit obs-fold policy:
- For request parser role: either reject with 400 or unfold to SP before semantic parsing.

3. Add limits:
- Max header line length.
- Max total header section bytes.
- Max header count.
- Return clear error status (400/431 style behavior in HTTP layer).

4. Fix Host lookup consistency:
- Lookup `host` if names are normalized to lowercase.

5. Improve diagnostics:
- Replace debug prints with structured parser error reason codes.

---

## Worked Examples

### Valid
`Host: example.com`  
`Content-Type: text/plain`

Reason:
- Valid token names, colon format, value OWS acceptable.

### Invalid (must reject)
`Host : example.com`

Reason:
- Whitespace before colon means field-name is not a token (`Host ` invalid).

### Duplicate risk example
`X-Test: one`  
`X-Test: two`

Current behavior:
- Stored value becomes only `two` (last write wins).

RFC-aware behavior:
- Preserve both values and combine only if field semantics allow.

### Obsolete fold example
`X-Long: one\r\n\ttwo`

Current behavior:
- Next folded line is parsed as new header and fails token validation.

RFC-aware behavior:
- Explicitly reject as obsolete fold or unfold to SP per policy.

---

## Glossary (Simplified)

- Header field: One line like `Name: value`.
- Field name: The left side (`Name`), case-insensitive.
- Field value: The right side (`value`) after trimming outer OWS.
- Token: Allowed character set for names/identifiers.
- OWS: Optional whitespace (space/tab), may appear around value.
- RWS: Required whitespace (at least one space/tab) in some grammars.
- BWS: Historical whitespace that senders should not generate.
- CRLF: `\r\n`, HTTP line ending.
- obs-fold: Obsolete multiline continuation (`CRLF` + SP/HTAB).
- Generic parsing: Split headers structurally first, validate semantics later.
- Request smuggling: Attacks exploiting parser differences between components.

---

## Final Verdict

The current implementation demonstrates a **good RFC-oriented foundation** for header parsing (token validation, case-insensitive naming, OWS trimming, CRLF incremental parsing).  
It is **not fully RFC 7230 compliant yet** due to duplicate header handling, explicit `obs-fold` policy, and missing field limits, plus a concrete `Host` lookup bug.

---

## Appendix A: Extra Examples

### 1) Header Name Token Rules

Valid:
- `Host: example.com`
- `Content-Type: text/html`
- `X_Custom-Token: abc123`

Invalid:
- `Content Type: text/html` (space in field-name)
- `Host : example.com` (space before colon)
- `X(Test): 1` (parentheses are not valid token chars)

### 2) Case-Insensitive Names

These are equivalent by RFC rules:
- `Host: example.com`
- `host: example.com`
- `HOST: example.com`

Typical normalization result:
- internal key: `host`
- internal value: `example.com`

### 3) OWS Trimming Around Values

Input line:
- `Content-Type:    text/html\t`

Extracted value:
- `text/html`

Input line:
- `X-Note:\t\thello world   `

Extracted value:
- `hello world`

Internal spaces are preserved:
- `X-Note: hello   world` -> `hello   world`

### 4) Obsolete Line Folding (obs-fold)

Deprecated examples:
- `X-Long: part1\r\n part2`
- `X-Long: part1\r\n\tpart2`

Modern parser policy should be explicit:
- reject request, or
- unfold each fold into spaces before interpretation.

### 5) Duplicate Header Behavior

Input:
- `Accept: text/html`
- `Accept: application/json`

Current project behavior:
- last value wins in map storage.

RFC-aware behavior:
- preserve both values and only combine where field semantics allow.

### 6) Size/Count Limit Examples

Typical defensive limits:
- max header line bytes: `8192`
- max total header section bytes: `32768`
- max header count: `100`

Oversize requests should be rejected (commonly 400 or 431).
