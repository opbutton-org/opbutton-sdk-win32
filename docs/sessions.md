# Sessions

## Session identity

The SDK generates a random `run_id` for each initialized runtime. It is not saved with the device ID. Connection retries, approval polling and network recovery keep that ID. Reinitializing the SDK starts another run, creates a new server session and closes the previous session for that device. A retired run cannot reconnect into the replacement session. An explicit disconnect or a server timeout can close a session within a run; reconnecting then creates a fresh session for that same run.

The server expires sessions after five minutes without an authenticated client request. Recovery before expiration keeps the session and its token. Older clients without `run_id` retain legacy behavior until the device uses run identity; afterwards a client that omits the ID receives HTTP 409 rather than joining another run.
