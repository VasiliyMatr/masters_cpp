CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG v1.15.2
  VERSION 1.15.2
  OPTIONS "INSTALL_GTEST OFF"
)

CPMAddPackage("gh:fmtlib/fmt#11.0.2")
