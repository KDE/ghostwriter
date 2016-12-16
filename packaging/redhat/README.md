# ghostwriter Packaging: RPM

This specfile can be used to build ghostwriter RPM package for Red Hat based distributions. Tested on Fedora 25.

## Updating version
1\. Update the specfile with the new version and reset release to 0
```Diff
diff --git a/packaging/redhat/ghostwriter.spec b/packaging/redhat/ghostwriter.spec
index 2224c7d..670902e 100644
--- a/packaging/redhat/ghostwriter.spec
+++ b/packaging/redhat/ghostwriter.spec
@@ -3,8 +3,8 @@
 
 Summary: ghostwriter: A cross-platform, aesthetic, distraction-free Markdown editor
 Name: ghostwriter
-Version: 1.4.2
-Release: 1%{?dist}
+Version: 1.4.3
+Release: 0%{?dist}
 License: GPLv3+
 Group: Development/Tools
 URL: http://wereturtle.github.io/ghostwriter/
```
2\. Bump specfile using `rpmdev-bumpspec`
```sh
rpmdev-bumpspec \
    --comment="upgrade to v1.4.3" \
    --userstring="$(git config user.name) <$(git config user.email)>" \
    ghostwriter.spec
```
3\. Commit your changes
