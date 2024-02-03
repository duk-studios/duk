# Duk

Duk is a collection of tools for game development/rendering in general.

# Modules
There are many modules, each one exposing a specific set of features:
- [duk_macros](./duk_macros/README.md):
- [duk_hash](./duk_hash/README.md):
- [duk_events](./duk_events/README.md):
- [duk_task](./duk_task/README.md):
- [duk_tools](./duk_tools/README.md):
- [duk_platform](./duk_platform/README.md):
- [duk_log](./duk_log/README.md):
- [duk_resource](duk_resource/README.md):
- [duk_rhi](./duk_rhi/README.md):
- [duk_scene](./duk_scene/README.md):
- [duk_renderer](./duk_renderer/README.md):
- [duk_import](./duk_import/README.md):
- [duk_engine](./duk_engine/README.md):

# Building
Requirements:
- C++ 20 compiler
- CMake 3.24+
- Python 3

Duk development is mainly done with MinGW, GCC and MSVC _should_ also work (minor issues might arise, PRs are welcome).

# Contributing
New features must have a corresponding open issue

Branch name must be in the following format:
```
(new|chg|fix)/#(issue_number)-(short-message)
```
e.g. 
```
new/#0-add-new-things
fix/#1-fix-broken-things
chg/#2-change-existing-things
```
Please keep the description short and with wording that explains what the commit _does_ (as opposed to what you did).

e.g.
```
new/#4-add-feature // good
new/#5-added-some-things-somewhere-that-do-stuff // not very good
```

Commit messages follow a similar format:
```
(new|chg|fix): #(issue_number): (short message)
```

e.g.
```
new: #0: add specific thing
chg: #0: remove old obsolete thing
fix: #0: fix thing that was not working
```
