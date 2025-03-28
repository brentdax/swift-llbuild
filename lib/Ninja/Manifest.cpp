//===-- Manifest.cpp ------------------------------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "llbuild/Ninja/Manifest.h"

#include "llbuild/Basic/LLVM.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

using namespace llbuild;
using namespace llbuild::ninja;

bool Rule::isValidParameterName(StringRef name) {
  return name == "command" ||
    name == "description" ||
    name == "deps" ||
    name == "depfile" ||
    name == "generator" ||
    name == "pool" ||
    name == "restat" ||
    name == "rspfile" ||
    name == "rspfile_content";
}

Manifest::Manifest() {
  // Create the built-in console pool, and add it to the pool map.
  consolePool = new (getAllocator()) Pool("console");
  assert(consolePool != nullptr);
  if (consolePool)
    consolePool->setDepth(1);
  pools["console"] = consolePool;

  // Create the built-in phony rule, and add it to the rule map.
  phonyRule = new (getAllocator()) Rule("phony");
  getRootScope().getRules()["phony"] = phonyRule;
}

Node* Manifest::findNode(StringRef workingDirectory, StringRef path0) {
  StringRef path;
  SmallString<256> absPathTmp;

  if (llvm::sys::path::is_absolute(path0)) {
    path = path0;
  } else {
    absPathTmp = path0;
    llvm::sys::fs::make_absolute(workingDirectory, absPathTmp);
    assert(absPathTmp[0] == '/');
    path = absPathTmp;
  }

  auto it = nodes.find(path);
  if (it == nodes.end()) {
    return nullptr;
  }
  return it->second;
}

Node* Manifest::findOrCreateNode(StringRef workingDirectory, StringRef path0) {
  StringRef path;
  SmallString<256> absPathTmp;

  if (llvm::sys::path::is_absolute(path0)) {
    path = path0;
  } else {
    absPathTmp = path0;
    llvm::sys::fs::make_absolute(workingDirectory, absPathTmp);
    assert(absPathTmp[0] == '/');
    path = absPathTmp;
  }

  auto& result = nodes[path];
  if (!result)
    result = new (getAllocator()) Node(path, path0);
  return result;
}
