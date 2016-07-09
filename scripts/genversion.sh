#!/bin/bash

version=$(git describe --tags --long |  tr "-" " ")
tag=$(echo $version | cut -d' ' -f1)
rev=$(echo $version | cut -d' ' -f2)
hash=$(echo $version | cut -d' ' -f3)
echo \"$tag.$rev \($hash\)\" 