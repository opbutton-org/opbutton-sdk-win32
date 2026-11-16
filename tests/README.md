The UI regression tests build the SDK's UI sources directly. They do not use the HTTP client, LiveKit, or an active session and can run on Windows, macOS, and Linux.

```sh
cmake -S tests -B build-ui-tests -DCMAKE_BUILD_TYPE=Debug
cmake --build build-ui-tests
ctest --test-dir build-ui-tests --output-on-failure
```

The cases cover callback reentry, mutable callback state, handler lifetime and destruction, concurrent registration/dispatch, and generated tab IDs. Test checks remain enabled in Release builds.

The ownership executable also checks allocator origin after allocator switches,
legacy callback allocators, alignment, allocation failures and overflow,
intrusive reference counts, and widget callback/provider cleanup.
