# Sessions

## Session identity

The SDK generates a random `run_id` for each initialized runtime. It is not saved with the device ID. Connection retries, approval polling and network recovery keep that ID. Reinitializing the SDK starts another run, creates a new server session and closes the previous session for that device. A retired run cannot reconnect into the replacement session. An explicit disconnect or a server timeout can close a session within a run; reconnecting then creates a fresh session for that same run.

The server expires sessions after five minutes without an authenticated client request. Recovery before expiration keeps the session and its token. Older clients without `run_id` retain legacy behavior until the device uses run identity; afterwards a client that omits the ID receives HTTP 409 rather than joining another run.

## Installation credential and transport

Each installation generates a high-entropy `device_secret` before its first connection. The SDK stores it in Windows Credential Manager under a target derived from the normalized server origin/path and device ID. The approval poll must present the same credential; the server stores only its SHA-256 digest. A caller may provision a 32–256 character printable-ASCII credential with `setDeviceSecret(...)` when it already owns equivalent secure storage.

Production server URLs must use HTTPS and cannot contain credentials, a query, or a fragment. Plain HTTP is accepted only for `localhost`, `127.0.0.1`, or `::1` after `setAllowInsecureLocalhost(true)`. Redirects are not followed, requests have bounded timeouts, response bodies are limited to 2 MiB, and authentication header values reject CR/LF.

Existing approved devices are moved back to pending once by the server migration. Re-approve the upgraded installation only after confirming it is the expected device.
